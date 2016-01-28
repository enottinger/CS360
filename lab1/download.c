#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <sstream>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         1024
#define HOST_NAME_SIZE      255
#define REQUEST_SIZE	    511
#define PAGE_SIZE	    255

int isInt(char *string)
{
        int i, stringLength = strlen(string);

        for(i = 0; i < stringLength; i++)
        {
                if(isdigit(string[i]) == 0 || ispunct(string[i]) != 0)
                        break;
        }

        if(i != stringLength)
                return 1;
        else
                return 0;

}

int  main(int argc, char* argv[])
{
    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo = NULL;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char* request = (char*) malloc(REQUEST_SIZE);
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
    int debugFlag = 0;// if -d
    int countFlag = 0;// if -c
    int c, times_to_download = 1, err = 0;
    char strPage[PAGE_SIZE];
    int flags = 0;
    int i;
    int count = 0;
    //Parameters: hostname, portnumber, url, flags (-d, -c)

    if(argc < 4)
      {
        printf("\nUsage: download [-d] [-c number_of_downloads] host-name host-port absolute-path\n");
        return 0;
      }
    else
      {
	//std::cout << "hello";
	//printf("Client parameter load: start ");	
	while((c = getopt ( argc, argv, "c:d" ) ) != -1)
	{
	   switch(c)
	   {
		case 'c':
		   countFlag = 1;
		   times_to_download = atoi(optarg);
		   flags++;
		   break;
		case 'd':
		   debugFlag = 1;
		   flags++;
		   break;
		case '?':
		   err = 1;
		   break;
	   }
	}
	if(argc - (flags + countFlag) < 4)
	{
	  printf("\nUsage: download [-d] [-c number-of-downloads] host-name port-number absolute-path\n");
	  return 0;
	}
	//printf("--- After getopt");
        strcpy(strHostName,argv[optind]);
        if(debugFlag == 1)
	{
	//  printf("%s, ", strHostName);
	}
	if(isInt(argv[optind + 1]) == 1)
	{
	  printf("\nInvalid port number\n");
	  return 0;
	}
        nHostPort=atoi(argv[optind + 1]);
        if(debugFlag == 1)
	{
	//  printf("%X, ", nHostPort);
	}
	strcpy(strPage, argv[optind + 2]);
	if(debugFlag == 1)
	{
	//  printf("%s\n", strPage);
	}
      }
    //printf("\nDebug flag is %d", debugFlag);
    
    for(i = 0; i < times_to_download; i++)
    {

    	//printf("\nMaking a socket");
    	/* make a socket */
    	hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
    	if(hSocket == SOCKET_ERROR)
    	{
	   if(countFlag == 1)
	{
	    printf("Successfully downloaded: %d out of %d times\n",count,times_to_download);
	}
           printf("\nCould not make a socket\n");
           return 0;
    	}

    	/* get IP address from name */
    	pHostInfo=gethostbyname(strHostName);
    	if(pHostInfo == NULL)
    	{
	   if(countFlag == 1)
	{
	    printf("Successfully downloaded: %d out of %d times\n",count,times_to_download);
	}
   	   printf("\nInvalid hostname\n");
	   return 0;
    	}
    	/* copy address into long */
        memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    	/* fill address struct */
    	Address.sin_addr.s_addr=nHostAddress;
    	Address.sin_port=htons(nHostPort);
    	Address.sin_family=AF_INET;


    
    	//printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

    	/* connect to host */
    	if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) 
    	   == SOCKET_ERROR)
    	{
    	    if(countFlag == 1)
	{
	    printf("Successfully downloaded: %d out of %d times\n",count,times_to_download);
	}
	    printf("\nCould not connect to host\n");
    	    return 0;
    	}
	
	    // Need /r/n

	    //Need to write a request

	int n;
	n = sprintf(request, "GET %s HTTP/1.1\r\nHost:%s:%d\r\n\r\n",
		strPage,strHostName,nHostPort);

	//strcpy(request, "GET ");
	//strcat(request, strPage);
    	//strcat(request, " HTTP/1.1\r\nHost:");
    	//strcat(request, strHostName);
    	//strcat(request, "\r\n\r\n");

    	if(debugFlag == 1)
    	{
	   printf("\nHeader: \n%s", request);
    	}

    

    	write(hSocket,request,n);//May need to replace n with REQUESTSIZE


    	/* read from socket into buffer
    	** number returned by read() and write() is the number of bytes
   	 ** read or written, with -1 being that an error occured */
   	 //nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
    	
	FILE* webinput = fdopen(hSocket, "r");
	//char* line = NULL;
	int clength = -1;

	
	if(webinput == NULL)
	{
	   perror("Error with creating FILE*");
	}	
	else
	{
	  
	 // while(fgets(line, 100, webinput) != NULL)
	 //std::string line;
	 char* line = NULL;
	 size_t size = NULL;
	 if(debugFlag == 1){
	    printf("\nHeaders: \n");
	 }
	 while(getline(&line, &size,  webinput)) 
	 {
		//puts(line);
		if(debugFlag == 1)
		{
		  printf("%s",line);
		}
		std::string sline(line);
		free(line);
		line = NULL;
		std::stringstream ss(sline);
		std::string nextWord;
		//printf("\nYo Yo ma");
		ss >> nextWord;
		//printf("\nObtained first word");
 		if(nextWord == "")
		{
		   break;
		}
		else if(nextWord == "Content-Length:")
		{
		   //printf("\nFound target");
		   ss >> clength;
		   //printf("**ContentLength: %d",clength);
	        }
	  }
	}


   	 if(debugFlag == 1)
   	 {
            //printf("\nProjected read amount is: %d", clength);
         }
	
	if(clength != -1)
	{
	   char* pBuffer = (char*)malloc(clength);
	   nReadAmount=fread(pBuffer,sizeof(char),clength,webinput);
	   if(nReadAmount != clength)
	   {
		printf("ERROR - ReadAmount");
	   }
	   //nReadAmount = read(hSocket, pBuffer, clength);	   

    	   if(countFlag == 0 || debugFlag == 1)
    	   {
		//printf("\nRead amount is: %d", nReadAmount);
    	   	printf("\nReceived \"%s\" from server\n",pBuffer);
    	   }
   	   /* write what we received back to the server */
   	   // write(hSocket,pBuffer,nReadAmount);
   	   //printf("\nWriting \"%s\" to server",pBuffer);
    
	   //free everything
	   free(pBuffer);
	   count++;
        }
	
	

    	//printf("\nClosing socket\n");
    	/* close socket */                       
    	if(close(hSocket) == SOCKET_ERROR)
    	{
    	    if(countFlag == 1)
	    {
	    	printf("Successfully downloaded: %d out of %d times\n",count,times_to_download);
	    }
	    printf("\nCould not close socket\n");
    	    return 0;
    	}
    }
	if(countFlag == 1)
	{
	    printf("Successfully downloaded: %d out of %d times\n",count,times_to_download);
	}
	free(request);
}
