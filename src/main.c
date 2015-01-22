/***************************************************************************
 web_server.c  -  a web server wich is more (or less) than only a web server
                             -------------------
    begin                : ????????????????
    copyright            : (C) 2002 by Mirko De Grandis
    email                : dega_mi@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/utsname.h>
#include <errno.h>

#include "defines.h"
#include "utils.h"
#include "send_funct.h"
#include "handlers.h"
#include "connections.h"
#include "cw_glob_var.h"
#include "winfunct.h"

/*
 * aggiunta variabile rootdir per la directory in cui sono i file da inviare e la directory data/
 * ora l'eseguibile può stare in una delle directory *bin/
 *
 * variabili di ambiente: 
 *  wsmp3_rootDIR ==> rootdir
 *  wsmp3_accepted ==> directory che possono essere linkate nella rootdir (per CGI-BIN)
 */
int main(int argc, char *argv[],char *envp[])
{
  int i,j,chati;
  char recvBuffer[BUFSIZE];
  char buf[BUFSIZE],*listadiraccepted;
  int dupsock,tmp_porta,tmp_forkon;
  int serverSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_in clientAddr;
  int clientAddrSize;
  struct hostent* entity;
  struct sigaction sa_dontcare;
  int childPID;
  char *str,*cp,*cp2,*errstr,arg[80],*arg2[3],*inizio,*fine,**ora;
  int error;
  char chatbuf[1024];
  time_t start_tempo;
  FILE *tf;
  int fnbf=0; //flag per bloccare la trasmissione di file troppo grandi (FLAG NO BIG FILES)
  struct utsname machine;
  

  cwPID=-1;
  uname(&machine); //vediamo chi siamo......
  programWIDE_totalReceived=0;
  programWIDE_totalSent=0;
  usr_cnsl=fopen("/dev/tty","a");
  pipe(fd);
  /**************************** AMBIENTE DA FILE RC *********************************/

  sprintf(recvBuffer,"%s/.wsmp3rc",getenv("HOME"));
  tf=fopen(recvBuffer,"r");
  if(tf==NULL){fprintf(usr_cnsl,"no ~/.wsmp3rc found\n"); tf=fopen("/etc/wsmp3rc","r");}
  if(tf==NULL) 
    {
      fprintf(usr_cnsl,"no /etc/wsmp3rc found\n");
      sprintf(recvBuffer,"%s/.wsmp3rc",getenv("PWD"));
      tf=fopen(recvBuffer,"r");
    }
  if(tf!=NULL)
    {
      for(i=0;i<BUFSIZE;i++) buf[i]=recvBuffer[i]='\0';
      while(!feof(tf))
	{
	  i=fread(recvBuffer,BUFSIZE,1,tf);
	}
      strcpy(buf,recvBuffer);
      setenv("wsmp3_rootDIR",gimme_line(recvBuffer,"wsmp3_rootDIR="),1);
      strcpy(recvBuffer,buf);
      setenv("wsmp3_accepted",gimme_line(recvBuffer,"wsmp3_accepted="),1);
      strcpy(recvBuffer,buf);
      if(!strcmp(gimme_line(recvBuffer,"cmdls-follows="),"yes")) ls_follow_outside_links=1;
  
      fclose(tf);
      for(i=0;i<BUFSIZE;i++) buf[i]=recvBuffer[i]='\0'; 
    }
  else fprintf(usr_cnsl,"no $PWD/.wsmp3rc found. setting defaults\n");

  if(getenv("wsmp3_accepted")==NULL)
    {
      setenv("wsmp3_accepted",getenv("PWD"),1);
    }
  
  rootdir=getenv("wsmp3_rootDIR");
  if(rootdir==NULL)
    { 
      rootdir=getenv("PWD");
    }
  setenv("wsmp3_rootDIR",rootdir,1);
   

  //Riempio l'array delle directory accepted.....
  if(getenv("wsmp3_accepted")!=NULL) 
    {
      listadiraccepted=(char *)malloc(BUFSIZE);
      for(i=0;i<BUFSIZE;i++) listadiraccepted[i]='\0';
      strncpy(listadiraccepted,getenv("wsmp3_accepted"),strlen(getenv("wsmp3_accepted")));
      ora=DIRS;
      inizio=listadiraccepted;
      while(*inizio!='\0')
	{
	  fine=inizio;
	  while(*fine!=':'){if(*fine=='\0')break; fine++;}
	  if(fine[0] == ':') *fine='\0';if(abs(fine-inizio)>1 && *(fine-1)=='/') *(fine-1)='\0';
	  *ora=(char *)malloc(1024);
	  strcpy(*ora,inizio);
	  ora++;
	  inizio=fine+1;
	  if(*inizio=='\0')
	    break;
	}
      *ora=NULL;
      free(listadiraccepted);
      inizio=NULL;
      fine=NULL;
    }

  /*********************************************LINEA DI COMANDO**************************/

  error=0;
  for(j=1;j<argc;j++)
    {
      if(!strcmp(argv[j],"-log"))
	{ 
	  logflag=1;
	  sprintf(recvBuffer,"%s",getenv("PWD"));                       //controllo che la dir corrente
	  if(strstr(recvBuffer,getenv("HOME"))!=recvBuffer)             //sia sotto-dir della mia home
	    sprintf(recvBuffer,"%s/wsmp3-logfile.log",getenv("HOME"));  //se si scrivo nella dir
	  else sprintf(recvBuffer,"%s/wsmp3-logfile.log",getenv("PWD"));//se no scrivo nella mia home
	  logfile=fopen(recvBuffer,"a+");
	  if(logfile==NULL) logflag=0;
	}
      else if(!strcmp(argv[j],"-debug"))
	{
	  if(!DAEMON && CHATMODE!=1) DEBUG=1;
	}
      else if(strstr(argv[j],"-port:"))
      {
	str=strstr(argv[j],"-port:");
	str=str+6;
	tmp_porta=0;
	while(*str!='\0')
	  {
	    tmp_porta=tmp_porta*10+(int)(str[0]-'0');
	    str++;
	  }
	if(tmp_porta<=1024) tmp_porta=8000;
	if(DEBUG) fprintf(usr_cnsl,"MYPORT:%d\n\n",tmp_porta);
	MYPORT=tmp_porta;
      }
      else if(strstr(argv[j],"-forkon:"))
      {
	if(CHATMODE!=1)
	  {
	    str=strstr(argv[j],"-forkon:");
	    str=str+8;
	    tmp_forkon=0;
	    while(*str!='\0')
	      {
		tmp_forkon=tmp_forkon*10+(int)(str[0]-'0');
		str++;
	      }
	    if(DEBUG) fprintf(usr_cnsl,"FORKON:%d\n\n",tmp_forkon);
	    FORKON=tmp_forkon;
	  }
      }
      else if(!strcmp(argv[j],"--happymode"))
      {
	HAPPYMODE=1;
      }
      else if(!strcmp(argv[j],"--cmdls-follows"))
      {
	ls_follow_outside_links=1;
      }
      else if(!strcmp(argv[j],"-chat"))
      {
	if(!DAEMON)
	  {
	    CHATMODE=1;
	    FORKON=0;
	    DEBUG=0;
	  }
      }
      else if(!strcmp(argv[j],"-chatgui"))
      {
	if(!DAEMON)
	  {
	    CHATMODE=2;
	    cwPID=fork();
	    if(cwPID==0)
	      {
		sa_dontcare.sa_flags=0;
		sa_dontcare.sa_handler=SIG_IGN;
		sigaction(SIGUSR2,&sa_dontcare,NULL);
		creat_win(argc,argv);
		exit(0);
	      }
	    else
	      {
	      }
	  }
      }
      else if(!strcmp(argv[j],"-D"))
	{
	  DAEMON=1;
	  if(CHATMODE) CHATMODE=0;
	  if(DEBUG) DEBUG=0;
	  fclose(usr_cnsl);
	  usr_cnsl=fopen("/dev/console","a");
	}
      else if(!strcmp(argv[j],"--help")||!strcmp(argv[j],"-h"))
	{
	  printf("%s (%s) %s\n",argv[0],VERSION,AUTHOR);
	  printf("Usage: %s [OPTIONS]\n",argv[0]);
	  printf("OPTIONS are:\n\n");
	  printf("-log\t\t\tEnables log mode\n");
	  printf("-debug\t\t\tEnables debug mode\n");
	  printf("-port:<PORT_NUMBER>\tSet the port users will be connecting to.\n\t\t\tdefault is 8000\n");
	  printf("-forkon:<BYTES>\t\tSet the max dimension for the files on which the program\n\t\t\tdon't need to fork (set 0 to fork always).\n\t\t\tdefault is 8192\n");
	  printf("-chat\t\t\tEnables chat mode textonly\n\t\t\t(will fork always and debugging is disabled)\n");
	  printf("-chatgui\t\tEnables chat mode \n\t\t\t(will fork always and debugging is disabled)\n");
	  printf("--happymode\t\tEnables an \"Happy Mode\"\n");
	  printf("--cmdls-follows\t\tLet cmd:ls to show link pointing outside of\n\t\t\t wsmp3_rootdir or wsmp3_accepted\n");	  
	  printf("-D\t\t\tStart in daemon mode.\n");
	  printf("-h,--help\t\tThis help screen.\n\n");
	  exit(0);
	}
      else
	{
	  fprintf(usr_cnsl,"Unrecognized option!! %s\n",argv[j]);
	  error=1;
	}
    }
  
  if(error!=0)
    { 
      fprintf(usr_cnsl,"Exit()\n");
      exit(0);
    }


  /****************************ROOT MODE********************************/

  if(getuid()==0)
    { 
      fprintf(usr_cnsl,"\n\n\t\t*********************************************************\n\n");
      fprintf(usr_cnsl,"\t\t\tYOU ARE RUNNING THIS PROGRAM AS ROOT!!!!\n");
      fprintf(usr_cnsl,"\t\t\tthat's not a good idea man...\n");
      fprintf(usr_cnsl,"\t\t\t...but, hey - you can do what you want...\n");
      fprintf(usr_cnsl,"\n\t\t*********************************************************\n\n");     
    }

  /*************************** STAMPA AMBIENTE ***********************/

  if(DEBUG) fprintf(usr_cnsl,"wsmp3_accepted=%s\n",getenv("wsmp3_accepted"));
  if(logflag)
    {
      time(&tempo);
      fprintf(logfile,"%s wsmp3_accepted=%s\n",mytime(localtime(&tempo)),getenv("wsmp3_accepted"));
      fflush(logfile);
    }

  if(DEBUG) fprintf(usr_cnsl,"RootDIR=%s\n",rootdir);
  if(logflag)
    {
      time(&tempo);
      fprintf(logfile,"%s RootDIR=%s\n",mytime(localtime(&tempo)),rootdir);
      fflush(logfile);
    }

  /******************************INIZIALIZZAZIONE*********************/

  sockptr=&sock;
  signal(SIGINT,exit_handler);
  sa_dontcare.sa_flags=SA_ONESHOT;
  sa_dontcare.sa_handler=SIG_IGN;
  sigaction(SIGUSR2,&sa_dontcare,NULL);

  serverSocket=socket(AF_INET,SOCK_STREAM,0);
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(MYPORT);
  serverAddr.sin_addr.s_addr=INADDR_ANY;
  memset(&(serverAddr.sin_zero),0,8);

  bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(struct sockaddr));

  time(&tempo);
  fprintf(usr_cnsl,"%s",mytime(localtime(&tempo)));
  fprintf(usr_cnsl,"\n\t\t(l:%d,d:%d) %s (%s): ready to send!! :)\n\n",logflag,DEBUG,argv[0],VERSION);

  /***************************CICLO ATTESA***************************/

  listen(serverSocket,10);
  while(1)
    {
      if(CHATMODE==2)
	{
	  signal(SIGUSR2,cwsentreply);
	}

      clientAddrSize=sizeof(struct sockaddr_in);
      do
	{
	  sock=accept(serverSocket,(struct sockaddr*) &clientAddr,&clientAddrSize);
	}while ((sock==-1) && (errno==EINTR));
      if(sock==-1)
	{
	  fprintf(usr_cnsl,"Bad accept\n");
	  exit(1);
	}
      
      entity=gethostbyaddr((char*) &clientAddr.sin_addr,sizeof(struct in_addr),AF_INET);

      if(DEBUG) fprintf(usr_cnsl,"-------------------------\n");
      if(DEBUG) fprintf(usr_cnsl,"Connection from %s\n",inet_ntoa((struct in_addr) clientAddr.sin_addr));

      if(logflag)
	{
	  time(&tempo);
	  fprintf(logfile,"%s -------------------------\n",mytime(localtime(&tempo)));
	}
      if(logflag) 
	{
	  time(&tempo);
	  fprintf(logfile,"%s Connection from %s\n",
		  mytime(localtime(&tempo)),inet_ntoa((struct in_addr) clientAddr.sin_addr));
	}
      
      /************************SE NON RICEVO RICHIESTE => MENU PER TELNET********************/
      i=-1;
      time(&start_tempo);
      fcntl(sock,F_SETFL,O_NONBLOCK);
	    
      do
	{ 
	  i=recv(sock,recvBuffer,BUFSIZE,0);
	  if(i!=-1) break;
	  time(&tempo);
	  if((( (int)tempo - (int)start_tempo ))>5) break;
       }while(((int)tempo-(int)start_tempo)<5);

      if(i== -1)
	{
	  if(strcmp(inet_ntoa((struct in_addr) clientAddr.sin_addr),"127.0.0.1"))
	    {
	      sprintf(recvBuffer,"No connection from here outside of 127.0.0.1\n");
	      programWIDE_totalSent+=send(sock,recvBuffer,strlen(recvBuffer),0);
	      close(sock);
	      continue; //e' una maniera stupida di impedire accessi non autorizzati....
	    }

	  pidPassed=childPID=fork();
	  if(childPID==0)
	    { 
	      programWIDE_totalSent=0;
	      programWIDE_totalReceived=0;
	      fnbf=1;
	      signal(SIGTERM,exitchild_handler);
	      close(sock);
	      continue;
	    }
	  else
	    {
	      //sa_dontcare.sa_handler=SIG_IGN;      /*per non lasciare troppi processi zombie*/
	      sa_dontcare.sa_flags=SA_NOCLDSTOP; //SA_ONESHOT;
	      sa_dontcare.sa_handler=mychildexithandler;
	      //sa_dontcare.sa_flags=SA_NOCLDSTOP;   /*idem*/
	      sigaction(SIGCHLD,&sa_dontcare,NULL);/*idem*/

	      dupsock=dup(sock);
	      close(sock);
	      menu(&dupsock);
	      close(dupsock);
	      kill(childPID,SIGTERM);
	      fnbf=0;
	      continue;
	    }
	}

      if(PDEBUG) printf("i:%d\n",i);
      if(i==-1 || !i)
	{  
	  continue;
	}

      programWIDE_totalReceived+=i; //incremento il totale dei bytes in rx

      /********************************HO RICEVUTO RICHIESTE********************************/

      /*if(recvBuffer[i-1]!='\n') break;*/  //nostalgic comment... leave it where it is.... :)

      fcntl(sock,F_SETFL,O_APPEND);
      recvBuffer[i]='\0';

      if(DEBUG) fprintf(usr_cnsl,"Received from client: \n*****\n%s\n*****\n",recvBuffer);
      if(logflag) 
	{
	  time(&tempo);
	  fprintf(logfile,"%s Received from client: \n*****\n%s\n*****\n",
		  mytime(localtime(&tempo)),recvBuffer);
	  fflush(logfile);
	}

      conn_req=parse_request(recvBuffer);
      if(PDEBUG)printf("%s\n",print_req_descriptor(conn_req));

      for(i=0;i<BUFSIZE;i++) recvBuffer[i]='\0';
      if(PDEBUG) printf("recvBuffer svuotato: %s\n",recvBuffer);
     /*************************************** HAPPY MODE :) ************************************/      

      if(conn_req->agent!=NULL && (HAPPYMODE && 
				   ((strstr(conn_req->agent,"MSIE")||strstr(conn_req->agent,"indows"))||
				    strstr(conn_req->agent,"inNT"))
				  )
	)
	{
	  if(DEBUG) fprintf(usr_cnsl,"Be Happy! :)\n");
	  if(logflag)
	    {
	      time(&tempo);
	      fprintf(logfile,"%s Be Happy! :)\n",mytime(localtime(&tempo)));
	    } 
	  send(sock,HAPPYHEAD,strlen(HAPPYHEAD),0);	  
	  send(sock,HAPPYMSG,strlen(HAPPYMSG),0);
	  close(sock);
	  rem_req_descriptor(conn_req);
	  continue;
	}

      /***************************************DIRECTORY LIST************************************/

      if((conn_req->action!=NULL && !strcmp(conn_req->action,"GET"))
	 && 
	 (((cp2=strstr((cp=conn_req->what),"cmd:ls"))!=NULL||
	  (cp2=strstr((cp=conn_req->what),"cmd%3Als"))!=NULL)||
	  (cp2=strstr((cp=conn_req->what),"cmd_ls"))!=NULL) 
	 )
	{
	  *cp2='\0';
	  strcpy(arg,cp);
	  if(!strcmp(arg,"")) strcpy(arg,".");
       	  if(DEBUG) fprintf(usr_cnsl,"Request list:%s\n",arg);

	  if(logflag)
	    {
	      time(&tempo);
	      fprintf(logfile,"%s Request list:%s\n",mytime(localtime(&tempo)),arg);
	      fflush(logfile);
	    }

	  strcpy(conn_req->what,arg);
	  send_dir_list(sock,conn_req);
	  close(sock);
	  if(DEBUG) fprintf(usr_cnsl,"Connection closed by client.\n");
	} //inserire comandi????

      /***************************************    VERSION    ************************************/

      else if((conn_req->action!=NULL && !strcmp(conn_req->action,"GET"))
	      && 
	      (strstr(conn_req->what,"cmd:ver")!=NULL||
	       (strstr(conn_req->what,"cmd%3Aver")!=NULL||
		strstr(conn_req->what,"cmd_ver")!=NULL) 
	      )
	     )
	{
	  if(DEBUG) fprintf(usr_cnsl,"Version requested...\n");
	  if(logflag)
	    {
	      time(&tempo);
	      fprintf(logfile,"%s Version requested...\n",mytime(localtime(&tempo)));
	      fflush(logfile);
	    }

	  sprintf(recvBuffer,"HTTP/1.1 200 OK\nConnection: close\nContent/Type: text/html\n\n<HTML><HEAD><TITLE>WSMP3</TITLE><LINK rel=stylesheet href=\"/data/styles.css\" type=\"text/css\"></HEAD><BODY><DIV align=center class=testo>WsMp3 %s<BR>%s<BR><B>Running on:</B> %s %s %s %s %s<BR><B>by:</B> %s</DIV></BODY></HTML>",VERSION,AUTHOR,machine.sysname,machine.nodename,machine.release,machine.version,machine.machine,getenv("USER"));
	  programWIDE_totalSent+=send(sock,recvBuffer,strlen(recvBuffer),0);

	  if(DEBUG) fprintf(usr_cnsl,"Version sent.\n");
	  if(logflag)
	    {
	      time(&tempo);
	      fprintf(logfile,"%s Version sent.\n",mytime(localtime(&tempo)));
	      fflush(logfile);
	    }

	  close(sock);
	}
 
      /*************************************INVIO FLUSSI MP3*************************************/

      else if((conn_req->action!=NULL && !strcmp(conn_req->action,"GET")) && 
	      mp3_player(conn_req))
	{

	  if(fnbf) 
	    {
	      errstr=write_error_msg(conn_req,"please try again later\n");
	      send(sock,errstr,strlen(errstr),0);
	      close(sock);

	      if(DEBUG) fprintf(usr_cnsl,"Sent error:\n%s\n",errstr);
	      if(logflag)
		{ 
		  time(&tempo);
		  fprintf(logfile,"%s Sent error:\n%s\n",mytime(localtime(&tempo)),errstr);
		  fflush(logfile);
		}

	      continue;
	    }

	  pidPassed=childPID=fork();
	  if(childPID==0) 
	    {
	      programWIDE_totalReceived=0; //quando forko , nei processi figli i contatori sono resettati
	      programWIDE_totalSent=0;     //e poi aggiunti a quelli del padre
	      if(DEBUG) fprintf(usr_cnsl,"Parent: PID %d, Child: PID %d\n",getppid(),getpid());
	      if(logflag)
		{ 
		  time(&tempo);
		  fprintf(logfile,"%s Parent: PID %d, Child: PID %d\n",
			   mytime(localtime(&tempo)),getppid(),getpid());
		}
	      if(conn_req->what[strlen(conn_req->what)-1]=='/') read_mp3_dir(conn_req,&sock);
	      else read_mp3_file(conn_req,&sock);
	      close(sock);
	      if(DEBUG) fprintf(usr_cnsl,"Connection closed by client.\n");
	      if(logflag)
		{ 
		  time(&tempo);
		  fprintf(logfile,"%s Connection closed by client.\n",mytime(localtime(&tempo)));
		  fflush(logfile);
		}
	      if(PDEBUG) printf("SENDMP3 scrivo sulla pipe...");
	      sprintf(chatbuf,"%d/%d",programWIDE_totalReceived,programWIDE_totalSent);
	      write(fd[PIPE_WRITE],chatbuf,strlen(chatbuf));
	      if(PDEBUG) printf("ok\n");
	      exit(0);
	    }
	  else
	    { 
	      close(sock);  
	      if(DEBUG) fprintf(usr_cnsl,"Sending music in background....\n");
	      if(logflag)
		{ 
		  time(&tempo);
		  fprintf(logfile,"%s Sending music in background....\n",mytime(localtime(&tempo)));
		  fflush(logfile);
		}
	      sa_dontcare.sa_flags=SA_NOCLDSTOP; /*per non lasciare troppi processi zombie*/
	      sa_dontcare.sa_handler=mychildexithandler;
	      sigaction(SIGCHLD,&sa_dontcare,NULL);/*idem*/
	    }
	}

      /***************************************INVIO FILES*************************************/

      else if(conn_req->action!=NULL &&!strcmp(conn_req->action,"GET"))
	{  
	  if(shallIfork(conn_req->what)) 
	  {
	    if(fnbf) 
	      {
		errstr=write_error_msg(conn_req,"please try again later\n");
		send(sock,errstr,strlen(errstr),0);
		close(sock);
		
		if(DEBUG) fprintf(usr_cnsl,"Sent error:\n%s\n",errstr);
		if(logflag)
		  { 
		    time(&tempo);
		    fprintf(logfile,"%s Sent error:\n%s\n",mytime(localtime(&tempo)),errstr);
		    fflush(logfile);
		  }
		
		continue;
	      }

	  if(DEBUG) fprintf(usr_cnsl,"...must fork, filename:%s\n",conn_req->what);

	  if(logflag)
	    { 
	      time(&tempo);
	      fprintf(logfile,"%s ...must fork, filename:%s\n",
		      mytime(localtime(&tempo)),conn_req->what);
	    }

	  pidPassed=childPID=fork();
	  if(childPID==0) 
	    {
	      programWIDE_totalReceived=0; //quando forko , nei processi figli i contatori sono resettati
	      programWIDE_totalSent=0;     //e poi aggiunti a quelli del padre
	      if(DEBUG) fprintf(usr_cnsl,"Parent: PID %d, Child: PID %d\n",getppid(),getpid());
	      if(logflag)
		{ 
		  time(&tempo);
		  fprintf(logfile,"%s Parent: PID %d, Child: PID %d\n",
			  mytime(localtime(&tempo)),getppid(),getpid());
		}

	      if(!strcmp(conn_req->what,"")||conn_req->what[strlen(conn_req->what)-1]=='/')
		{ 
		  strcat(conn_req->what,"index.htm");
		  if(DEBUG) fprintf(usr_cnsl,"sending default page\n");
		  if(logflag)
		    {
		      time(&tempo);
		      fprintf(logfile,"%s sending default page\n",mytime(localtime(&tempo)));
		    }
		  read_html_file(conn_req,&sock);
		}
	      else if(strstr(conn_req->what,".htm")) read_html_file(conn_req,&sock);
	      else read_any_file(conn_req,&sock);
	      close(sock);
	      if(DEBUG) fprintf(usr_cnsl,"Connection closed by client.\n");
	      if(logflag)
		{ 
		  time(&tempo);
		  fprintf(logfile,"%s Connection closed by client.\n",mytime(localtime(&tempo)));
		  fflush(logfile);
		}
	      if(PDEBUG) printf("SENDFILES scrivo sulla pipe...");
	      sprintf(chatbuf,"%d/%d",programWIDE_totalReceived,programWIDE_totalSent);
	      write(fd[PIPE_WRITE],chatbuf,strlen(chatbuf));
	      if(PDEBUG) printf("ok\n");
	      exit(0);
	    }
	  else
	    { 
	      close(sock);  
	      if(DEBUG) fprintf(usr_cnsl,"Sending in background....\n");
	      if(logflag)
		{ 
		  time(&tempo);
		  fprintf(logfile,"%s Sending in background....\n",mytime(localtime(&tempo)));
		  fflush(logfile);
		}

	      sa_dontcare.sa_flags=SA_NOCLDSTOP;  /*per non lasciare troppi processi zombie*/
	      sa_dontcare.sa_handler=mychildexithandler;
	      sigaction(SIGCHLD,&sa_dontcare,NULL);/*idem*/
	    }
	}
	else 
	  {
	    if(DEBUG) fprintf(usr_cnsl,"...don't need to fork, PID:%d\n",getpid());
	    if(logflag)
	      {
		time(&tempo);
		fprintf(logfile,"%s ...don't need to fork, PID:%d\n",mytime(localtime(&tempo)),getpid());
	      }
	    
	    if(!strcmp(conn_req->what,"")||conn_req->what[strlen(conn_req->what)-1]=='/')
	      { 
		strcat(conn_req->what,"index.htm");

		if(DEBUG) fprintf(usr_cnsl,"sending default page\n");
		if(logflag)
		  {
		    time(&tempo);
		    fprintf(logfile,"%s sending default page\n",mytime(localtime(&tempo)));
		  }
		read_html_file(conn_req,&sock);
	      }
	    else if(strstr(conn_req->what,".htm")) read_html_file(conn_req,&sock);
	    else read_any_file(conn_req,&sock);

	    close(sock);
	    if(DEBUG) fprintf(usr_cnsl,"Connection closed by client.\n");
	    if(logflag)
	      { 
		time(&tempo);
		fprintf(logfile,"%s Connection closed by client.\n",mytime(localtime(&tempo)));
		fflush(logfile);
	      }
	  }
	}

      /*****************************************CGI-BIN************************************/
      
      else if((conn_req->action!=NULL) && !strcmp(conn_req->action,"POST"))
	{
	  if(conn_req->what!=NULL && (tf=fopen(conn_req->what,"r"))==NULL)
	    { 
	      errstr=write_error_msg(conn_req,"Not implemented");
	      send(sock,errstr,strlen(errstr),0);
	      close(sock);
	      if(DEBUG) fprintf(usr_cnsl,"CGI_BIN [%s] not found.\nSent error: %s",
				conn_req->what,errstr);
	      if(logflag)
		{
		  time(&tempo);
		  fprintf(logfile,"%s CGI-BIN [%s] not found.\nSent error: %s",
			  mytime(localtime(&tempo)),conn_req->what,errstr);
		  fflush(logfile);
		}
	    }
	  else //implementato
	    {
	      fclose(tf);
	      pidPassed=childPID=fork();
	      if(childPID==0)
		{
		  arg2[0]=conn_req->what;
		  arg2[1]=conn_req->content;
		  arg2[2]=NULL;
		  fclose(stdout);
		  stdout=tmpfile();
		  if(stdout==NULL)  //non riesco a reindirizzare stdout
		    {
		      stdout=fopen("/dev/tty","a");
		      n_errno=errno;
		      if(DEBUG) fprintf(usr_cnsl,"tmpfile: %s\n",sys_errlist[n_errno]);
		      if(logflag)
			{
			  time(&tempo);
			  fprintf(logfile,"%s tmpfile:%s\n",
				  mytime(localtime(&tempo)),
				  sys_errlist[n_errno]);
			  fflush(logfile);
			  n_errno=-1;
			}
		      close(sock);
		      write(fd[PIPE_WRITE],"0/0",strlen("0/0"));
		      exit(0);
		    }
		  else  //riesco a reindirizzare stdout
		    {
		      pidPassed=childPID=fork();
		      if(childPID==0)  //sono il figlio. handler:mycgichildexithandler del padre
			{
			  if(PDEBUG) printf("PID:%d PadrePID:%d\n",getpid(),getppid());
			  execv(conn_req->what,arg2);
			  exit(0);
			}
		      else   //sono il padre. handler:mychildexithandler del processo base
			{
			  programWIDE_totalSent=0;    //i contatori sono resettati
			  programWIDE_totalReceived=0;
			  sa_dontcare.sa_flags=SA_NOCLDSTOP;/*per non lasciare troppi processi zombie*/
			  sa_dontcare.sa_handler=mycgichildexithandler;
			  sigaction(SIGCHLD,&sa_dontcare,NULL);/*idem*/
			  waitpid(childPID,NULL,0);
			}
		    }
		}
	      else
		{
		  //handler mychildexit
		  close(sock);
		  sa_dontcare.sa_flags=SA_NOCLDSTOP;  /*per non lasciare troppi processi zombie*/
		  sa_dontcare.sa_handler=mychildexithandler;
		  sigaction(SIGCHLD,&sa_dontcare,NULL);/*idem*/      
		}
	    }
	}
      /******************************************CHAT***************************************/

      else if(conn_req->action!=NULL && (!strcmp(conn_req->action,"CHA") && CHATMODE))
	{
	  if(CHATMODE==2)
	    {
	      for(chati=0;chati<1024;chati++) chatbuf[chati]='\0';
	      if(conn_req->what[0]=='[')
		{
		  tmp_porta=0;
		  i=1;
		  while(conn_req->what[i]!=']')
		    {
		      tmp_porta=tmp_porta*10+(int)(conn_req->what[i]-'0');
		      i++;
		    }
		  i++;
		  if(tmp_porta==0) tmp_porta=8000;
		  sprintf(chatbuf,"[%s:%d]\n%s",
			  inet_ntoa((struct in_addr) clientAddr.sin_addr),
			  tmp_porta,
			  &(conn_req->what[i]));
		}
	      else
		{
		  sprintf(chatbuf,"[%s]\n%s",
			  inet_ntoa((struct in_addr) clientAddr.sin_addr),
			  conn_req->what);
		}
	      write(fd[PIPE_WRITE],chatbuf,strlen(chatbuf));
	      if(PDEBUG) printf("pid:%d invio SIGUSR1\n",getpid());
	      kill(cwPID,SIGUSR1);
	      close(sock);
	    }
	  else
	    {
	      for(chati=0;chati<1024;chati++) chatbuf[chati]='\0';
	      fprintf(usr_cnsl,"[%s] %s\n> ",
		      inet_ntoa((struct in_addr) clientAddr.sin_addr),
		      conn_req->what);
	      fgets(chatbuf,1024,stdin);
	      if(chatbuf[strlen(chatbuf)-1]=='\n') chatbuf[strlen(chatbuf)-1]='\0';
	      chatbuf[1023]='\0';
	      send(sock,chatbuf,strlen(chatbuf),0);
	      close(sock);
	      if(logflag)
		{
		  time(&tempo);
		  fprintf(logfile,"%s CHAT MESSAGE SENT: [%s]\n",mytime(localtime(&tempo)),chatbuf);
		  fflush(logfile);
		}
	    }
	}

      /********************************** ping-WsMp3 *****************************************/
      
      else if(conn_req->action!=NULL && !strcmp(conn_req->action,"ping-WsMp3?"))
	{
	  for(chati=0;chati<1024;chati++) chatbuf[chati]='\0';
	  sprintf(chatbuf,"WsMp3:%s\n",VERSION);
	  send(sock,chatbuf,strlen(chatbuf),0);
	  close(sock);
	  if (DEBUG) fprintf(usr_cnsl,"Pong...\n");
	  if(logflag)
	    {
	      time(&tempo);
	      fprintf(logfile,"%s Pong...\n",mytime(localtime(&tempo)));
	      fflush(logfile);
	    }
	}

      /***************************************************************************************/
      else
	{ 
	  if(DEBUG) printf("don't know what to do....\n"); 
	  if(logflag)
	    {
	      time(&tempo);
	      fprintf(logfile,"%s don't know what to do...\n",mytime(localtime(&tempo)));
	      fflush(logfile);
	    }
	}
      rem_req_descriptor(conn_req); //elimina la richiesta
    }
  return 0;
}
