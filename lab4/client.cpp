#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

//#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define HOST_NAME_SIZE      255

#define NCONNECTIONS	   20
int  main(int argc, char* argv[])
{
    int SOCKET_ERROR = -1;
	int hSocket[NCONNECTIONS];                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;

    if(argc < 3)
      {
        printf("\nUsage: client host-name host-port\n");
        return 0;
      }
    else
      {
        strcpy(strHostName,argv[1]);
        nHostPort=atoi(argv[2]);
      }

    printf("\nMaking a socket");
    int epollfd = epoll_create(1);
    for(int i = 0; i < NCONNECTIONS; i++)
    {
    /* make a socket */
    hSocket[i]=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(hSocket[i] == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

    /* connect to host */
    if(connect(hSocket[i],(struct sockaddr*)&Address,sizeof(Address)) 
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }

    /* read from socket into buffer
    ** number returned by read() and write() is the number of bytes
    ** read or written, with -1 being that an error occured */
    //nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
    //printf("\nReceived \"%s\" from server\n",pBuffer);
    /* write what we received back to the server */
    struct epoll_event event;
	event.data.fd = hSocket[i];
	event.events = EPOLLIN;
	int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, hSocket[i],&event);   
 char request[] = "GET /foo.html HTTP/1.0\r\n\r\n";
    write(hSocket[i],request,strlen(request));
    }
    printf("\nWriting \"%s\" to server",pBuffer);
    for(int i =0; i < NCONNECTIONS; i++)
    {
	struct epoll_event event;
	int nr_events = epoll_wait(epollfd, &event, 1, -1);
	char buffer[10000];
	int rval = read(event.data.fd,buffer,10000);

    printf("\nGot %d from %d\nClosing socket\n",rval, event.data.fd);
    /* close socket */                       
    if(close(event.data.fd) == SOCKET_ERROR)
    {
        printf("\nCould not close socket\n");
        return 0;
    }
	}
}
