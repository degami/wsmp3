#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define MYPORT 8000   /* the port users will be connecting to */

#define BUFSIZE 32768
#define DEBUG 1
#define FORKON 8192
#define VERSION "v.0.0.1"

int main(int argc, char *argv[],char *envp[])
{
  int bytesSent,i,times;
  char reqbuf[BUFSIZE],recvBuffer[BUFSIZE],msg[BUFSIZE];
  char *hostname="127.0.0.1";
  int sock;
  struct sockaddr_in clientAddr;
  struct hostent* nlp_host;

  nlp_host= gethostbyname(hostname);
  bzero(&clientAddr,sizeof(clientAddr));
  clientAddr.sin_family=AF_INET;
  clientAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  clientAddr.sin_addr.s_addr=((struct in_addr *)(nlp_host->h_addr))->s_addr;
  clientAddr.sin_port=htons(MYPORT);

  //  printf("Connesso...\n");
  while(1)
    {
      for(i=0;i<BUFSIZE;i++) {reqbuf[i]='\0';msg[i]='\0';}      
      printf("> "); fgets(msg,1023,stdin);msg[strlen(msg)-1]='\0';
      sprintf(reqbuf,"CHA /%s",msg);
      sock=socket(AF_INET,SOCK_STREAM,0);
      connect(sock,(void *)&clientAddr,sizeof(clientAddr));
      bytesSent=send(sock,reqbuf,strlen(reqbuf),0);
      
      i=0;
      do
	{
	  for(i=BUFSIZE;i>=0;i--) recvBuffer[i]='\0';
	  i=recv(sock,recvBuffer,BUFSIZE,0);
	  if(i==0) break;
	  fprintf(stdout,"%s\n",recvBuffer);
	}while(i!=0);

      close(sock);
    }
  return 0;
}
