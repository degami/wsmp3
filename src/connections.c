#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#include <netdb.h>
#include "connections.h"
#include "defines.h"

/*
 * Funzione che visualizza il menu per una connessione di telnet. il timeout e' fissato a 10 secondi
 */
void menu(int *sock)
{
  int i;
  time_t start_tempo;
  char recvBuffer[BUFSIZE];
  char buffer[1024];
  char end[3]={255,253,18};
  char *menuopts[]={"1.Statics\n",
		    "2.Set Debug\n",
		    "3.Set HappyMode\n",
		    "4.Set Logging\n",
		    "5.Set Chat\n",
		    "6.Quit\n"};
  char showmenu[1024];
  if(PDEBUG) printf("In menu\n");
  fcntl(*sock,F_SETFL,O_NONBLOCK);
  while(1)
    {
      sprintf(showmenu,"\n-> WsMp3 <-\n\n%s%s%s%s%s%s\n>",
	      menuopts[0],
	      menuopts[1],
	      menuopts[2],
	      menuopts[3],
	      menuopts[4],
	      menuopts[5]);
      i=-1;
      send(*sock,showmenu,strlen(showmenu),0);
      time(&start_tempo);
      do
	{  
	  i=recv(*sock,recvBuffer,BUFSIZE,0);
	  if(i!=-1) break;
	  time(&tempo);
	  if((( (int)tempo - (int)start_tempo ))>10) break;
	}while( ((int)tempo - (int)start_tempo )<10 );

      if(i==-1) 
	{
	  if(PDEBUG) printf("Uscito da menu()\n");
	  send(*sock,"TIMEOUT!\n",9,0);
	  send(*sock,end,3,0);
	  return;
	}
      else if(i==3 && recvBuffer[0]=='1')
	{
	  sprintf(showmenu,"rx bytes: %d ,\ttx bytes: %d \nDEBUG:%d, HAPPYMODE:%d, LOGGING:%d, CHATMODE:%d\n",
		  programWIDE_totalReceived,programWIDE_totalSent,DEBUG,HAPPYMODE,logflag,CHATMODE);
	  send(*sock,showmenu,strlen(showmenu),0);
	}
      else if(i==3 && recvBuffer[0]=='2')
	{
	  if(!DAEMON)
	    {
	      if(DEBUG) 
		{
		  send(*sock,"DEBUG is now OFF.\n",strlen("DEBUG is now OFF.\n"),0);
		  DEBUG=0;
		}
	      else 
		{    
		  send(*sock,"DEBUG is now ON.\n",strlen("DEBUG is now ON.\n"),0);
		  DEBUG=1;
		  if(CHATMODE)
		    {
		      CHATMODE=0;
		      send(*sock,"CHATMODE is now OFF.\n",strlen("CHATMODE is now OFF.\n"),0);
		    }
		}
	    }
	  else send(*sock,"Sorry, started as daemon.\n",strlen("Sorry started as daemon.\n"),0);
	}
      else if(i==3 && recvBuffer[0]=='3')
	{
	  if(HAPPYMODE==0) 
	    {
	      send(*sock,"HAPPYMODE is now ON.\n",strlen("HAPPYMODE is now ON.\n"),0);
	      HAPPYMODE=1;
	    }
	  else 
	    {
	      send(*sock,"HAPPYMODE is now OFF.\n",strlen("HAPPYMODE is now OFF.\n"),0);
	      HAPPYMODE=0;
	    }
	}
      else if(i==3 && recvBuffer[0]=='4')
	{
	  if(logflag) 
	    {
	      logflag=0;
	      fclose(logfile);
	      send(*sock,"LOGGING is now OFF.\n",strlen("LOGGING is now OFF.\n"),0);
	    }
	  else
	    {
	      logflag=1;
	      sprintf(buffer,"%s",getenv("PWD"));                     //controllo che la dir corrente
	      if(strstr(buffer,getenv("HOME"))!=buffer)            //sia sotto-dir della mia home
		sprintf(buffer,"%s/wsmp3-logfile.log",getenv("HOME"));  //se si scrivo nella dir
	      else sprintf(buffer,"%s/wsmp3-logfile.log",getenv("PWD"));//se no scrivo nella mia home
	      logfile=fopen(buffer,"a+");
	      if(logfile==NULL)
		{
		  send(*sock,"An error occured.LOGGING is now OFF.\n",
		       strlen("An error occured.LOGGING is now OFF.\n"),0);
		  logflag=0;
		}
	      else send(*sock,"LOGGING is now ON.\n",strlen("LOGGING is now ON.\n"),0);
	    }
	}
      else if(i==3 && recvBuffer[0]=='5')
	{
	  if(!DAEMON)
	    {
	      if(CHATMODE)
		{
		  CHATMODE=0;
		  send(*sock,"CHATMODE is now OFF.\n",strlen("CHATMODE is now OFF.\n"),0);
		}
	      else
		{
		  CHATMODE=1;
		  FORKON=0;
		  DEBUG=0;
		  send(*sock,"CHATMODE is now ON.\n",strlen("CHATMODE is now ON.\n"),0);
		}
	    }
	  else send(*sock,"Sorry, started as daemon.\n",strlen("Sorry started as daemon.\n"),0);
	}
      else if(i==3 && recvBuffer[0]=='6')
	{
	  if(PDEBUG) printf("Uscito da menu()\n");
	  send(*sock,"Quitting...\n",12,0);
	  send(*sock,end,3,0);
	  return;
	}
      else
	{
	  send(*sock,
	       "Answer only 1,2,3,4,5 or 6 please!\n",
	       strlen("Answer only 1,2,3,4,5 or 6 please!\n")
	       ,0); 
	}
    }
}


int pingTo(char *adress,int port)
{
  int psock,i;  
  time_t start_tempo;
  char recvbuf[BUFSIZE];

  if(!openConnection(adress,port,&psock)) return 0;
  send(psock,"ping-WsMp3? /",strlen("ping-WsMp3? /"),0);
  
  time(&start_tempo);
  fcntl(sock,F_SETFL,O_NONBLOCK);
  do
    { 
      i=recv(psock,recvbuf,BUFSIZE,0);
      if(i!=-1) break;
      time(&tempo);
      if((( (int)tempo - (int)start_tempo ))>60) break;
    }while(((int)tempo-(int)start_tempo)<60);
  fcntl(sock,F_SETFL,O_APPEND);
  close(psock);

  if(i==-1) return 0;
  else if(strstr(recvbuf,"WsMp3:")==recvbuf) return 1;
  return 0;
}


void checkConnections()
{
  int i,j,port;
  char addr[16],*ptr1,*ptr2;
  port=0;

  i=0;
  while(connlist[i]!=NULL)
    {
      if((ptr2=strstr(connlist[i],":"))!=NULL)
	{
	  ptr1=connlist[i];
	  strncpy(addr,connlist[i],ptr2-ptr1);
	  ptr2++;
	  while(*ptr2!='\0')
	    {
	      port= port*10 + (int)(*ptr2-'0');
	      ptr2++;
	    }

	  // <adr>:<port>'\0'

	  if(!pingTo(addr,port))
	    {
	      for(j=i;connlist[j]!=NULL;j++) connlist[j]=connlist[j+1];
	    } 
	} 
      else
	{
	  if(!pingTo(connlist[i],8000))
	    {
	      for(j=i;connlist[j]!=NULL;j++) connlist[j]=connlist[j+1];
	    }
	}
    }
}


int openConnection(char *adress,int port,int *toSock)
{  
  struct sockaddr_in clientAddr;
  struct hostent* nlp_host;

  if(PDEBUG) printf("in openConnection(%s,%d,bo..)\n",adress,port);

  if((nlp_host=gethostbyname(adress))==NULL) 
    {
      fprintf(usr_cnsl,"Bad address\n");
      return 0;
    }

  bzero(&clientAddr,sizeof(clientAddr));
  clientAddr.sin_family=AF_INET;
  clientAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  clientAddr.sin_addr.s_addr=((struct in_addr *)(nlp_host->h_addr))->s_addr;
  clientAddr.sin_port=htons(port);

  *toSock=socket(AF_INET,SOCK_STREAM,0);
  if((*toSock)==-1)
    {
      fprintf(usr_cnsl,"Bad socket\n");
      return 0;
    }

  if(connect(*toSock,(void *)&clientAddr,sizeof(clientAddr))==-1)
    {
      fprintf(usr_cnsl,"Bad connect\n");
      return 0;
    }

  if(PDEBUG) printf("Connessione aperta.\n");
  return 1;
}
