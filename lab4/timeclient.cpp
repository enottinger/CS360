#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <math.h>

//#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define HOST_NAME_SIZE      255

#define NCONNECTIONS	   20
#define NSOCKETS	10
#define NSTD 3	




int  main(int argc, char* argv[])
{
    int SOCKET_ERROR = -1;
   
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
    int count, c, err = 0;
    int debugFlag = 0;
    std::string path;

    if(argc < 5)
      {
        printf("\nUsage: client host-name host-port path count [-d]\n"); 
       return 0;
      }
    else
      {
	while((c = getopt (argc, argv, "d") ) != -1)
	{
	   switch(c)
	   {
		case 'd':
		   debugFlag = 1;
		   break;
		case '?':
		   err = 1;
		   break;
	   }
	}
	if(argc - debugFlag < 5)
	{
	    printf("\nUsage: client host-name host-port path count [-d]\n");
	    return 0;
	}
	strcpy(strHostName,argv[optind]);
        nHostPort=atoi(argv[optind + 1]);
	path = argv[optind + 2];
	count = atoi(argv[optind + 3]);
      }


    struct timeval oldtime[count + NSTD];
    struct timeval newtime[count];
    int hSocket[count];                 /* handle to socket */

    printf("\nMaking a socket");
    int epollfd = epoll_create(1);
    for(int i = 0; i < count; i++)
    {
    /* make a socket */
    hSocket[i]=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(hSocket[i] == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    }
	
    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    //printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

    for(int i = 0; i < count; i++)
    {

    /* connect to host */
    if(connect(hSocket[i],(struct sockaddr*)&Address,sizeof(Address)) 
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }

     
 //char request[] = "GET /foo.html HTTP/1.1\r\n";
	std::stringstream request;
	request << "GET " << path << " HTTP/1.0\r\nHost:" <<strHostName<<":";
	request << nHostPort <<"\r\n\r\n";
   write(hSocket[i],request.str().c_str(),strlen(request.str().c_str())); 
	gettimeofday(&oldtime[hSocket[i]],NULL); 
	struct epoll_event event;
	event.data.fd = hSocket[i];
	event.events = EPOLLIN;
	int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, hSocket[i],&event);
	if(ret)
		perror("epoll_ctl");
    }
    printf("\nWriting \"%s\" to server",pBuffer);
    
    double totalTime = 0;
    int responses = 0;
    //double average = 0;

    double vals[count];

    for(int i =0; i < count; i++)
    {
	struct epoll_event event;
	int nr_events = epoll_wait(epollfd, &event, 1, -1);
	
	int total = 0;
	int rval;
	char buffer[10000];
	do{
	    rval = read(event.data.fd,buffer,10000);
	    total += rval;
	}while(rval > 0);

	

    
	struct timeval newtime;
	gettimeofday(&newtime, NULL);
	double usec = (newtime.tv_sec - oldtime[event.data.fd].tv_sec)*(double)1000000+(newtime.tv_usec - oldtime[event.data.fd].tv_usec);
	double sec = usec / 1000000;
	vals[i] = sec;
	totalTime += sec;
	if(debugFlag == 1)
	{
	    printf("\nGot %d from %d\nClosing socket\n",total, event.data.fd);
	    std::cout << "new time tv_sec = "<<newtime.tv_sec<<std::endl;
	    std::cout << "new time tv_usec = "<<newtime.tv_usec<<std::endl;
	    std::cout << "old time tv_sec = "<<oldtime[event.data.fd].tv_sec<<std::endl;
	    std::cout << "old time tv_usec = "<<oldtime[event.data.fd].tv_usec<<std::endl;
	    std::cout << "usecs : " <<usec <<std::endl;
	    std::cout << "Time "<<sec<<std::endl;
	}
	
    	/* close socket */                       
    	if(close(event.data.fd) == SOCKET_ERROR)
    	{
    	    printf("\nCould not close socket\n");
    	    return 0;
    	}
    }
    double average = totalTime/count;
    double stdevnum = 0; // st dev numerator
    for (int i = 0; i < count; i++)
    {
	double diff = vals[i] - average;
	stdevnum += diff * diff;
    }
    double stdev = stdevnum/count;
    stdev = sqrt(stdev);
    std::cout << "Average: " << totalTime/count <<std::endl;
    std::cout << "StDev: " << stdev << std::endl;
}
