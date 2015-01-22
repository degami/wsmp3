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
  char reqbuf[BUFSIZE],recvBuffer[BUFSIZE];
  char *hostname="127.0.0.1";
  int sock;
  struct sockaddr_in clientAddr;
  struct hostent* nlp_host;

  /*if(argc!=2)
    {
      printf("Uso %s <nomefile>\n",argv[0]);
      exit(0);
    }

  printf("File richiesto:%s\n",argv[1]);*/

  nlp_host= gethostbyname(hostname);
  bzero(&clientAddr,sizeof(clientAddr));
  clientAddr.sin_family=AF_INET;
  clientAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  clientAddr.sin_addr.s_addr=((struct in_addr *)(nlp_host->h_addr))->s_addr;
  clientAddr.sin_port=htons(MYPORT);

  sock=socket(AF_INET,SOCK_STREAM,0);

  connect(sock,(void *)&clientAddr,sizeof(clientAddr));
  printf("Connesso...Richiesta...\n");
  sprintf(reqbuf,"ping-WsMp3? /");
  printf("Messaggio:[%s]\n",reqbuf);
  bytesSent=send(sock,reqbuf,strlen(reqbuf),0);

  i=0;
  times=0;
  do
    {
      for(i=0;i<BUFSIZE;i++) recvBuffer[i]='\0';
      i=recv(sock,recvBuffer,BUFSIZE,0);
      if(i==0) break;
      times++;
      //printf("\n[%d] Ricevuto:%d bytes\n",times,i);
      fprintf(stdout,"%s",recvBuffer);
    }while(i!=0);

  close(sock);
  return 0;
}
