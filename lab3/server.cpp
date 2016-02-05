#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <queue>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/signal.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
//#define MESSAGE             "Hello World! Hello CS360!"
#define QUEUE_SIZE          5

#define MAX_MSG_SZ      1024

sem_t empty, full, mutex;


bool isWhitespace(char c)
{ switch (c)
    {
        case '\r':
        case '\n':
        case ' ':
        case '\0':
            return true;
        default:
            return false;
    }
}

// Strip off whitespace characters from the end of the line
void chomp(char *line)
{
    int len = strlen(line);
    while (isWhitespace(line[len]))
    {
        line[len--] = '\0';
    }
}

// Read the line one character at a time, looking for the CR
// You dont want to read too far, or you will mess up the content
char * GetLine(int fds)
{
    char tline[MAX_MSG_SZ];
    char *line;
    
    int messagesize = 0;
    int amtread = 0;
    //std::cout << "Before While" <<std::endl;
    while((amtread = read(fds, tline + messagesize, 1)) < MAX_MSG_SZ)
    {
	//std::cout <<messagesize<<std::endl;
        if (amtread > 0)
            messagesize += amtread;
	else  if(amtread == 0)
	{
	    break;
	}
        else
        {
            perror("Socket Error is:");
            fprintf(stderr, "Read Failed on file descriptor %d messagesize = %d\n", fds, messagesize);
            exit(2);
        }
        //fprintf(stderr,"%d[%c]", messagesize,message[messagesize-1]);
        if (tline[messagesize - 1] == '\n')
            break;
    }
    //std::cout << "After While" <<std::endl;
    tline[messagesize] = '\0';
    chomp(tline);
    line = (char *)malloc((strlen(tline) + 1) * sizeof(char));
    strcpy(line, tline);
    //fprintf(stderr, "GetLine: [%s]\n", line);
    //std::cout << "\nLine is: " << line << " Line size: " << strlen(line);
    return line;
}
    
// Change to upper case and replace with underlines for CGI scripts
void UpcaseAndReplaceDashWithUnderline(char *str)
{
    int i;
    char *s;
    
    s = str;
    for (i = 0; s[i] != ':'; i++)
    {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = 'A' + (s[i] - 'a');
        
        if (s[i] == '-')
            s[i] = '_';
    }
    
}


// When calling CGI scripts, you will have to convert header strings
// before inserting them into the environment.  This routine does most
// of the conversion
char *FormatHeader(char *str, const char *prefix)
{
    char *result = (char *)malloc(strlen(str) + strlen(prefix));
    char* value = strchr(str,':') + 1;
    UpcaseAndReplaceDashWithUnderline(str);
    *(strchr(str,':')) = '\0';
    sprintf(result, "%s%s=%s", prefix, str, value);
    return result;
}

// Get the header lines from a socket
//   envformat = true when getting a request from a web client
//   envformat = false when getting lines from a CGI program

void GetHeaderLines(std::vector<char *> &headerLines, int skt, bool envformat)
{
    //std::cout << "In GetHeaderLines\n";
    // Read the headers, look for specific ones that may change our responseCode
    char *line;
    char *tline;
    
    //std::cout << "Before GetLine\n";
    tline = GetLine(skt);
    //std::cout << "After GetLine\n";
    while(strlen(tline) != 0)
    {
        if (strstr(tline, "Content-Length") || 
            strstr(tline, "Content-Type"))
        {
            if (envformat)
                line = FormatHeader(tline, "");
            else
                line = strdup(tline);
        }
        else
        {
            if (envformat)
                line = FormatHeader(tline, "HTTP_");
            else
            {
                line = (char *)malloc((strlen(tline) + 10) * sizeof(char));
                sprintf(line, "HTTP_%s", tline);                
            }
        }
        //fprintf(stderr, "Header --> [%s]\n", line);
        
        headerLines.push_back(line);
        free(tline);
        tline = GetLine(skt);
    }
    free(tline);
}

void get404Response(int hSocket)
{
	std::stringstream soutput;
	soutput << "HTTP/1.1 404 Not Found\r\n";
	soutput << "Content-Type: text/html\r\n";
	soutput << "Content-Length: 58\r\n\r\n";
	soutput << "<html><body><h1>404 File Not Found Error</h1></body></html>";
	//return soutput.str();
	write(hSocket, soutput.str().c_str(), strlen(soutput.str().c_str()));
}


void getFileResponse(int hSocket, std::string requestExtension, struct stat filestat)
{
	std::stringstream soutput;
	std::string extension = requestExtension.substr(requestExtension.find_last_of(".") + 1);
	std::string contentType = "NULL";
	if(extension == "html"){
		contentType = "text/html";
	}
	else if(extension == "txt")
	{
		contentType = "text/plain";
	}
	else if(extension == "jpg")
	{
		contentType = "image/jpg";
	}
	else if(extension == "gif")
	{
		contentType = "image/gif";
	}
	if(contentType != "NULL")
	{
		soutput << "HTTP/1.1 200 OK\r\n";
		soutput << "Content-Type: "<< contentType << "\r\n";
		soutput << "Content-Length: " << filestat.st_size <<"\r\n\r\n";
	
		FILE *fp = fopen(requestExtension.c_str(),"r");
		char* buffer = (char*)malloc(filestat.st_size);
		fread(buffer, filestat.st_size, 1, fp);
		//int size = filestat.st_size;
		//int readamount = read(fp, buffer, size);	

		//soutput << buffer;
		//std::cout << requestExtension.c_str() << " is a regular file \n";
		//std::cout << "file size = "<<filestat.st_size <<"\n";
		
		//printf("Got\n%s",buffer); //Write to socket in server
		std::cout <<soutput.str();
		//return soutput.str();
		write(hSocket, soutput.str().c_str(), strlen(soutput.str().c_str()));
		write(hSocket, buffer, filestat.st_size);
	}
	else
	{
		get404Response(hSocket);	
	}
}

void getDirectoryResponse(int hSocket, std::string fullExtension, std::string requestExtension, int nHostPort, struct stat filestat, struct sockaddr_in Address, std::string dir)
{
	std::stringstream soutput;
	soutput << "HTTP/1.1 200 OK\r\n";
	soutput << "Content-Type: ";

	std::stringstream sstr;
	sstr << "<html>\n\t<body>\n";
		
	DIR *dirp;
	struct dirent *dp;
	dirp = opendir(fullExtension.c_str());
	int len=20;
	char addbuffer[len];

	std::string temp = "52.10.211.151";

	inet_ntop(AF_INET, &(Address.sin_addr), addbuffer, len);
	while((dp = readdir(dirp)) != NULL)
	{
		std::string name = dp->d_name;
		if(name == "index.html")
		{
			std::cout << "Triggered ";
			std::stringstream newEX;
			newEX << fullExtension << "/" << name;
			struct stat newStat;
			stat(newEX.str().c_str(), &newStat);
			getFileResponse(hSocket, newEX.str(), newStat);
			return;
		}
		//std::cout << "\n" << dp->d_name;
		sstr << "<a href=\"";
		//std::cout << "<a href=\"";
		//sstr << addbuffer << ":" <<nHostPort;
		//sstr << temp << ":" <<nHostPort;
		//std::cout << addbuffer << ":" <<ntohs(Address.sin_port); 
		sstr <<requestExtension;
		if(requestExtension != "" && requestExtension[requestExtension.size() - 1] != '/')
			sstr << "/";
		sstr << dp->d_name;
		struct stat newStat;
		stat(dp->d_name, &newStat);
		if(S_ISDIR(newStat.st_mode))
		{	
			sstr << "/";
		}
		sstr << "\">";
		//sstr << dir <<dp->d_name << "\">";
		//std::cout << dir <<dp->d_name << "/>";
		sstr << dp->d_name << "</a><br>\n";
		//std::cout << dp->d_name << "</a><br>\n";

	}
	(void)closedir(dirp);

	sstr << "</body></html>";

	int contentLength = sstr.str().length();
	soutput	<< contentLength <<"\r\n\r\n";
	soutput << sstr.str();

	write(hSocket, soutput.str().c_str(), strlen(soutput.str().c_str()));
	//return soutput.str();
}

class myqueue{
	std::queue <int> stlqueue;
	public:
	void push(int sock){
		sem_wait(&empty);
		sem_wait(&mutex);
		stlqueue.push(sock);
		sem_post(&mutex);
		sem_post(&full);
	}
	int pop(){
		sem_wait(&full);
		sem_wait(&mutex);
		int rval = stlqueue.front();
		stlqueue.pop();
		sem_post(&mutex);
		sem_post(&empty);
		return(rval);
	}


}sockqueue;

struct arg_struct{
    std::string dir;
    struct sockaddr_in address;
    int threadid;
    int nHostPort;
    char* pBuffer;
};


void *connectToClient(void *arguments)
{

    struct arg_struct *args = (struct arg_struct *)arguments;
    struct sockaddr_in Address = args->address; 
    std::string dir = args->dir;
    int nHostPort = args->nHostPort;
    char* pBuffer = args->pBuffer;

    for(;;)
    {
        printf("\nWaiting for a connection\n");
        /* get the connected socket */
        
	//May need to be in the main 


	int hSocket = sockqueue.pop();

       // printf("\nGot a connection from %X (%d)\n",
       //       Address.sin_addr.s_addr,
       //       ntohs(Address.sin_port));
        memset(pBuffer,0,sizeof(pBuffer));
        int size;
	
	std::vector<char*> headers;
	//std::cout <<"\n Get Headers \n";
	GetHeaderLines( headers, hSocket, false);	
	//std::cout <<"\n Got Headers \n";

	
	if(headers.size() > 0)
	{
	std::stringstream ss(headers[0]);
	std::stringstream soutput;
	std::string requestType;
	std::string requestExtension;
	std::string requestHTTPType;
	ss >> requestType >> requestExtension >> requestHTTPType;

	std::cout << "\nRequest Type: " << requestType << "\nrequestExtension: " <<requestExtension <<"\nRequestHTTPType: "<<requestHTTPType<<std::endl;

	struct stat filestat;

	std::string fullExtension = requestExtension;
	fullExtension.insert(0,dir);

	if(stat(fullExtension.c_str(), &filestat)) {
		std::cout << "\nIn 404\n";
		get404Response(hSocket);
		//write(hSocket, output.c_str(), strlen(output.c_str()));	
	}
	else if(S_ISREG(filestat.st_mode)) {
		std::cout << "\nIn File\n";
		getFileResponse(hSocket, fullExtension, filestat);
		//write(hSocket, output.c_str(), strlen(output.c_str()));
	}
	else if(S_ISDIR(filestat.st_mode)) {
		std::cout << "\nIn Directory\n";
		getDirectoryResponse(hSocket, fullExtension, requestExtension, nHostPort, filestat, Address, dir);	
		//write(hSocket, output.c_str(), strlen(output.c_str()));
	}

	}
	printf("\nClosing the socket");
	memset(pBuffer,0,sizeof(pBuffer));
	//sprintf(pBuffer,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent Length: 34\r\n\r\n<html><body>Hello<\\body><\\html>\n");
	//write(hSocket,pBuffer, strlen(pBuffer));
	linger lin;
	unsigned int y=sizeof(lin);
	lin.l_onoff=1;
	lin.l_linger=10;
	setsockopt(hSocket, SOL_SOCKET, SO_LINGER,&lin,sizeof(lin));
	shutdown(hSocket, SHUT_RDWR);
        /* close socket */
        if(close(hSocket) == SOCKET_ERROR)
        {
         printf("\nCould not close socket\n");
         return 0;
        }
    }
}

void handler(int status);

int main(int argc, char* argv[])
{
    int hSocket,hServerSocket;  /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize=sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;
    std::string dir;
    int NTHREADS;
    int NQUEUE;

    struct sigaction sigold, signew;
	
    signew.sa_handler=handler;
    signew.sa_flags = SA_RESTART;
    sigaddset(&signew.sa_mask,SIGINT);
    signew.sa_flags = SA_RESTART;
    sigaction(SIGINT, &signew,&sigold);
    sigaction(SIGHUP,&signew,&sigold);
    sigaction(SIGPIPE, &signew, &sigold);

    if(argc < 4)
      {
        printf("\nUsage: server host-port threads dir\n");
        return 0;
      }
    else
      {
        nHostPort=atoi(argv[1]);
	NTHREADS = atoi(argv[2]);
	dir = argv[3];
	NQUEUE = NTHREADS*2;
      }

    printf("\nStarting server");

    printf("\nMaking socket");
    /* make a socket */
    hServerSocket=socket(AF_INET,SOCK_STREAM,0);

    if(hServerSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* fill address struct */
    Address.sin_addr.s_addr=INADDR_ANY;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nBinding to port %d",nHostPort);

	int optval = 1;
	setsockopt (hServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    /* bind to a port */
    if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) 
                        == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
 /*  get port number */
    getsockname( hServerSocket, (struct sockaddr *) &Address,(socklen_t *)&nAddressSize);
    printf("opened socket as fd (%d) on port (%d) for stream i/o\n",hServerSocket, ntohs(Address.sin_port) );

        printf("Server\n\
              sin_family        = %d\n\
              sin_addr.s_addr   = %d\n\
              sin_port          = %d\n"
              , Address.sin_family
              , Address.sin_addr.s_addr
              , ntohs(Address.sin_port)
            );


    printf("\nMaking a listen queue of %d elements",QUEUE_SIZE);
    /* establish listen queue */
    if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
    {
        printf("\nCould not listen\n");
        return 0;
    }

	//#define NTHREADS 10
	//#define NQUEUE 20
	long threadid;
	pthread_t threads[NTHREADS];
	sem_init(&mutex, PTHREAD_PROCESS_PRIVATE, 1);
	sem_init(&full, PTHREAD_PROCESS_PRIVATE, 0);
	sem_init(&empty, PTHREAD_PROCESS_PRIVATE, NQUEUE);

	struct arg_struct *args = new struct arg_struct();
	args->dir = dir;
	args->address = Address;
	args->pBuffer = pBuffer;
	args->nHostPort = nHostPort;

	for(threadid = 0; threadid < NTHREADS; threadid++)
	{
		args->threadid = threadid;
		pthread_create(&threads[threadid], NULL, 
			connectToClient, (void *)args);
	}


	//Set up socket, bind, listen
 	for(;;){
		hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);
		sockqueue.push(hSocket);
	}	

	delete args;

}

void handler (int status)
{
	printf("received signal %d\n",status);
	
}

