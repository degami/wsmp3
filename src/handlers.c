#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "handlers.h"
#include "defines.h"
#include "send_funct.h"
#include "utils.h"
#include "cw_glob_var.h"
#include "winfunct.h"
#include "connections.h"

/*
 * quando un figlio del programma esce, il padre richiama questa funzione.
 * se il figlio ha scritto il numero di bytes in entrata ed in uscita nella pipe fd
 * i due numeri vengono letti e aggiunti alle variabili programWIDE_totalReceived e programWIDE_totalSent.
 * Il figlio (che intanto e' diventato un processo zombie)
 * viene completamente terminato grazie alla chiamata di waitpid.(nb) exitStatus viene ignorato.
 */
void mychildexithandler(int exitStatus)
{
  char buff[1024];
  char filename[100];
  int i,readnumber;

  if(PDEBUG) printf("In mychildexithandler()\n");
  if(PDEBUG) printf("programWIDE_totalReceived:%d\nprogramWIDE_totalSent:%d\n",
		    programWIDE_totalReceived,programWIDE_totalSent);

  for(i=0;i<100;i++) filename[i]='\0';
  for(i=0;i<1024;i++) buff[i]='\0';
  readnumber=0;
  if(PDEBUG) printf("PID %d leggo dalla pipe...",getpid());
  read(fd[PIPE_READ],buff,1024);
  if(PDEBUG) printf("ok.[%s]\n",buff);
  for(i=0;i<(strstr(buff,"/")-buff);i++)
    { 
      if(buff[i]>='0' && buff[i]<='9') 
	readnumber = readnumber*10 + buff[i]-'0';
    }
  programWIDE_totalReceived+=readnumber;
  
  readnumber=0;
  for(i=i+1;i<strlen(buff);i++)
    {  
      if(buff[i]>='0' && buff[i]<='9') 
	readnumber = readnumber*10 + buff[i]-'0';
    }
  programWIDE_totalSent+=readnumber;
  
  if(PDEBUG) printf("programWIDE_totalReceived:%d\nprogramWIDE_totalSent:%d\n",
		    programWIDE_totalReceived,programWIDE_totalSent);
  waitpid(pidPassed,&i,0); //voglio eliminare il processo zombie!!!
  if(PDEBUG) printf("Uscita da mychildexithandler()\n");
}

/*
 * quando un cgi è eseguito e il processo figlio esce , questa funzione invia
 * il risultato da stdout , scrive i bytes sulla pipe ed esce.
 * verra' intercettato da mychildexithandler (del processo iniziale). spero
 */
void mycgichildexithandler(int status)
{
  int i;
  char buf[1024];
  if(PDEBUG) printf("In mycgichildexithandler()\n");

  if(DEBUG) fprintf(usr_cnsl,"CGI-BIN [%s] Done.\n",conn_req->what);
  if(logflag)
    {
      time(&tempo);
      fprintf(logfile,"%s CGI-BIN [%s] Done.\n",
	      mytime(localtime(&tempo)),
	      conn_req->what);
      fflush(logfile);
    }

  /* invio la computazione e ripristino stdout */
  dread_html_file(stdout,&sock);
  fclose(stdout);
  stdout=fopen("/dev/tty","a");  

  waitpid(pidPassed,&i,0); //voglio eliminare il processo zombie!!!
  close(sock);
  
  if(PDEBUG) printf("PID:%d mycgiexithandler scrivo sulla pipe...",getpid());
  sprintf(buf,"%d/%d",programWIDE_totalReceived,programWIDE_totalSent);
  write(fd[PIPE_WRITE],buf,strlen(buf)); 
  if(PDEBUG) printf("ok.\n");
  if(PDEBUG) printf("Uscita da mycgichildexithandler()\nesco.\n");
  exit(0);
}


/*
 * gestisce il segnale SIGTERM mandato al child dal padre che ritorna dalla funzione menu
 */
void exitchild_handler(int signalcode)
{
  int i;
  char Buffer[1024];
  if(PDEBUG) printf("in exitchild_handler()\n");
  for(i=0;i<1024;i++) Buffer[i]='\0';
  sprintf(Buffer,"%d/%d",programWIDE_totalReceived,programWIDE_totalSent);
  write(fd[PIPE_WRITE],Buffer,strlen(Buffer));
  exit(0);
}


/*
 * per intercettare il ctrl-c
 */
void exit_handler(int signalcode)
{
  if(PDEBUG) printf("in exit_handler()\n");
  if(*sockptr!=-1) close(*sockptr);
  if(logflag) fclose(logfile);
  fprintf(usr_cnsl,"ctrl-c caught...Exit. Tnx for using this program.\n");
  exit(0);   
}


/*
 * chatwin risponde a SIGUSR1
 */
void serverSentmsg_handler(int signalcode)
{
  GdkColor color;
  GdkColormap *cmap;
  GtkWidget *list_item;
  GList *dlist;
  char buf[1024];
  char ipaddr[80],*ips,*ipe;
  int i,j,ADD=1;
  if(PDEBUG) printf("in serverSentmsg_handler().\n");

  for(i=0;i<1024;i++) buf[i]='\0';
  read(fd[PIPE_READ],buf,1024);
  i=0;
  while(buf[i]!='\0' && i<1022) i++;
  if(i<1021 && buf[++i]=='\0') buf[i]='\n'; 

  ips=strstr(buf,"[")+1;
  ipe=strstr(buf,"]");

  //strncpy(ipaddr,ips,(ipe-ips));
  for(j=0;j<80;j++) ipaddr[j]='\0';
  for(j=0;j<(ipe-ips);j++) 
    if((ips[j]>='0' && ips[j]<='9')
       ||
       (ips[j]=='.'||ips[j]==':')) ipaddr[j]=ips[j];
  ipaddr[ipe-ips]='\0';

  if(PDEBUG) printf("ipaddr:%s\n",ipaddr);
  
  i=0;
  while(connlist[i]!=NULL)
    {
      if(!strcmp(connlist[i],ipaddr)) ADD=0;
      i++;
    }
  if(ADD)
    {
      if(PDEBUG) printf("devo aggiungere.\n");
      i=0;
      if(connlist[i]==NULL)
	{ 
	  connlist[i]=(char *) malloc(80);
	  //	  strncpy(connlist[i],ipaddr,strlen(ipaddr));
	  for(j=0;j<80;j++) (connlist[i])[j]='\0';
	  for(j=0;j<strlen(ipaddr);j++) 
	    if((ipaddr[j]>='0' && ipaddr[j]<='9')
	       ||
	       ((ipaddr[j]=='.'||ipaddr[j]==':')||ipaddr[j]=='\0'))
	      { 
		(connlist[i])[j]=ipaddr[j];
		if((connlist[i])[j]=='\0') break;
	      }
	  i++;
	  connlist[i]=NULL;
	}
      else
	{ 
	  while(connlist[i]!=NULL) i++;
	  connlist[i]=(char *) malloc(80);
	  //	  strncpy(connlist[i],ipaddr,strlen(ipaddr));
	  for(j=0;j<80;j++) (connlist[i])[j]='\0';
	  for(j=0;j<strlen(ipaddr);j++) 
	    if((ipaddr[j]>='0' && ipaddr[j]<='9')
	       ||
	       ((ipaddr[j]=='.'||ipaddr[j]==':')||ipaddr[j]=='\0'))
	      { 
		(connlist[i])[j]=ipaddr[j];
		if((connlist[i])[j]=='\0') break;
	      }
	  i++;
	  connlist[i]=NULL;
	}

      i=0;
      if(PDEBUG) printf("--connlist:--\n");
      if(PDEBUG) while(connlist[i]!=NULL) {printf("%d:[%s]\n",i,connlist[i]);i++;}
      if(PDEBUG) printf("--fine--\n");

      dlist=NULL;
      list_item=gtk_list_item_new_with_label(ipaddr);
      dlist=g_list_prepend(dlist, list_item);
      gtk_widget_show(list_item);
      gtk_list_append_items( GTK_LIST(list1),dlist);
      gtk_object_set_data(GTK_OBJECT(list_item),
                           "list_item_data",
			   "");
    }

  /* Get the system color map and allocate the color blue */
  cmap = gdk_colormap_get_system();
  color.red = 0;
  color.green = 0;
  color.blue = 0xffff;
  if (!gdk_color_alloc(cmap, &color)) {
    g_error("couldn't allocate color");
  }
  add_text(ips-1,&color);
  if(PDEBUG) printf("esco da serverSentmsg_handler.\n");
}


/*
 * il padre elimina il processo di chatwin
 */
void mycwchildexithandler(int signalcode)
{
  int i;
  struct sigaction sa_dontcare;

  if(PDEBUG) printf("in mycwchildexithandler()\n");
  WsMp3_ChatWin=NULL;
  waitpid(cwPID,&i,0); //voglio eliminare il processo zombie!!!
  cwPID=-1;
  sa_dontcare.sa_flags=SA_ONESHOT;
  sa_dontcare.sa_handler=SIG_IGN;
  sigaction(SIGUSR1,&sa_dontcare,NULL);
  if(PDEBUG) printf("esco da mycwchildexithandler\n");
}


/*
 * il padre risponde a SIGUSR2
 */
void cwsentreply(int signal)
{
  char chatbuf[1024],sendbuf[1100];
  char addr[16],*ps,*pe,*pdoubledot;int port;
  int i;
  //  struct sigaction sa_dontcare;

  pe=NULL;
  ps=NULL;

  if(PDEBUG) printf("in cwsentreply()\n");
  for(i=0;i<1024;i++) chatbuf[i]='\0';
  for(i=0;i<1100;i++) sendbuf[i]='\0';
  read(fd[PIPE_READ],chatbuf,1024);
  //send(cwdupsock,chatbuf,strlen(chatbuf),0);
  if(PDEBUG) printf("letto:%s\n",chatbuf);

  ps=chatbuf;
  ps=strstr(ps,"[");if(ps==NULL) return;
  while(ps!=NULL)
    {
      port=0;
      if(PDEBUG) printf("entro nel while");
      pe=strstr(ps,"]");if(pe==NULL) return;      
      if((pdoubledot=strstr(ps,":"))==NULL) 
	{
	  port=8000;
	  strncpy(addr,ps+1,(pe-(ps+1)));
	  addr[pe-(ps+1)]='\0';
	}
      else if(!(pdoubledot<=pe && pdoubledot>=ps))
	{
	  port=8000;
	  strncpy(addr,ps+1,(pe-(ps+1)));
	  addr[pe-(ps+1)]='\0';
	}
      else
	{
	  strncpy(addr,ps+1,(pdoubledot-(ps+1)));
	  addr[pdoubledot-(ps+1)]='\0';
	  while(pdoubledot<pe)
	    {
	      if(pdoubledot[0]>='0' && pdoubledot[0]<='9') port= port*10 + (int)(pdoubledot[0]-'0');
	      pdoubledot++;
	    }
	}
      
      if(PDEBUG) printf("apro la connessione\n");
      if(!openConnection(addr,port,&cwdupsock))
	{ 
	  if(PDEBUG) printf("errore connessione\n");
	  //elimina ip
	  return;
	}
      else
	{
	  ps=pe+1;
	  if((ps=strstr(ps,"["))!=NULL){ *ps='\0'; if(PDEBUG) printf("prossimo ps:%s\n",ps); }
	  sprintf(sendbuf,"CHA /[%d]%s",MYPORT,pe+1);
	  if(PDEBUG) printf("invio: %s\n",sendbuf);
	  send(cwdupsock,sendbuf,strlen(sendbuf),0);
	  close(cwdupsock);
	  if(logflag)
	    {
	      time(&tempo);
	      fprintf(logfile,"%s CHAT MESSAGE SENT: [%s]\n",mytime(localtime(&tempo)),pe+1);
	      fflush(logfile);
	    }
	  if(ps!=NULL) *ps='[';
	}
    }
  /*sa_dontcare.sa_flags=SA_ONESHOT;
  sa_dontcare.sa_handler=SIG_IGN;
  sigaction(SIGUSR2,&sa_dontcare,NULL);*/
  if(PDEBUG) printf("esco da cwsentreply.");
}

