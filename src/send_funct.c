#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/socket.h>
#include "utils.h"
#include "send_funct.h"
#include "defines.h"

/*
 * Decide se inviare o no un header http
 */
int needHeader(req_descriptor *reqd)
{
  char *okAgents[]={"ozilla","pera","Wget","Java","Lynx",NULL};
  char **cp;
  
  if(reqd==NULL) return 1;
  else if(reqd!=NULL && reqd->agent==NULL) return 0;
  else
    { 
      cp=okAgents;
      while(*cp!=NULL)
	{
	  if(strstr(reqd->agent,*cp)!=NULL) return 1;
	  cp++;
	}
    }
  return 0;
}


/*
 * controlla se il programma chiamante e' un player mp3
 */
int mp3_player(req_descriptor *desc)
{
  int i;
  char *agents[]={"xmms","Winamp","NSplayer",NULL};
  if(desc->agent!=NULL)
    {
      i=0;
      while(agents[i]!=NULL)
	{
	  if(strstr(desc->agent,agents[i])!=NULL) return 1;
	  i++;
	}
    }
  return 0;
}


/*
 * restituisce un messaggio di errore (in formato html o no)
 */
char* write_error_msg(req_descriptor *reqd,char *msg)
{
  char * ritorno;
  char *errstr[5]={"404 Not Found",
		   "403 Forbidden",
		   "503 Service Unavaible",
		   "501 Not Implemented",
		   "400 Bad Request"};
  int errnum;
  char errhead[1024];
  errnum=0;

  if((ritorno=(char *)malloc(1024))==NULL)
    {
      printf("Not enough memory\n");
      exit(1);
    }
  
  if(strstr(msg,"o such")!=NULL) errnum=0;
  else if(strstr(msg,"denied")!=NULL) errnum=1;
  else if(strstr(msg,"later")!=NULL) errnum=2;
  else if(strstr(msg,"mplemented")!=NULL) errnum=3;
  else errnum=4;
  
  sprintf(errhead,"HTTP/1.1 %s\nConnection: close\nContent/Type: text/html\n\n",
	  errstr[errnum]);


  if(needHeader(reqd))
    sprintf(ritorno,"%s<HTML><HEAD><TITLE>Error</TITLE><BODY><H3>%s%s %s</H3></BODY></HTML>\n",
	    errhead,
	    (reqd!=NULL) ? reqd->what+strlen(rootdir) : "",
	    (reqd!=NULL) ? ":" : "",
	    msg);
  else sprintf(ritorno,"%s%s %s\n",
	       (reqd!=NULL) ? reqd->what+strlen(rootdir) : "",
	       (reqd!=NULL) ? ":" : "",
	       msg);
  n_errno=-1;

  programWIDE_totalSent+=strlen(ritorno);
  return ritorno;
}


/*
 * invia la lista del contenuto di una directory
 */
void send_dir_list(int sock, req_descriptor *reqd)
{
  DIR *d;
  struct dirent *de;
  struct stat st;
  char dirslash[1024];
  char path[1024];
  char row[1024]; 
  char weight[80];
  char *dirname;
  char *spacesbuf;
  int spaces,i;
  int somethingwritten;
  int total=0;
  int html=0;
  char *error1 = "Error listing directory\n";
  char *header =  "HTTP/1.1 200 OK\n"
    "Connection: close\n"
    "Content-Type: text/html\n\n";
  char *start_html = "<html><head><title>ls command output"
    "</title><LINK rel=stylesheet href=\"/data/styles.css\" type=\"text/css\"></head><body>"
    "<table border=\"0\"> ";
  char *end_html = "</table></body></html>";
  char *directory = "<IMG src=\"/data/directory.jpg\" height=16 width=16>";
  char *file = "<IMG src=\"/data/file.jpg\" height=16 width=16>";
  char *nothing_html = "<h3>nothing here!</h3>";
  char *nothing_txt = "nothing here!\n";
  char *errstr;
  somethingwritten=0;

  if(needHeader(reqd)) html=1;

  dirname=reqd->what;

  if(!ls_follow_outside_links && !is_inside(dirname))   //se is_inside ritorna 0 non lo mostro....
    {
      errstr=write_error_msg(reqd,"no such directory");
      programWIDE_totalSent+=send(sock,errstr,strlen(errstr),0);
      if (DEBUG) fprintf(usr_cnsl,"Sent Error:%s\n",errstr);
      if(logflag)
	{
	  time(&tempo);
	  fprintf(logfile,"%s Sent Error:%s\n", mytime(localtime(&tempo)),errstr);
	  fflush(logfile);
	}
      return;
    } 
  else d = opendir(dirname);

  if(!d)
    {
      if(html)
	{
	  programWIDE_totalSent+=send(sock,header,strlen(header),0);
	  programWIDE_totalSent+=send(sock,start_html,strlen(start_html),0);
	  programWIDE_totalSent+=send(sock,"<tr><td>",strlen("<tr><td>"),0);	  
	}
      programWIDE_totalSent+=send(sock,error1,strlen(error1),0);
      if(html)
	{
	  programWIDE_totalSent+=send(sock,"</td></tr>",strlen("</td></tr>"),0);  
	  programWIDE_totalSent+=send(sock,end_html,strlen(end_html),0);
	}
      if (DEBUG) fprintf(usr_cnsl,"Error sending directory list\n");
      if(logflag)
	{
	  time(&tempo);
	  fprintf(logfile,"%s Error sending directory list\n", mytime(localtime(&tempo)));
	  fflush(logfile);
	}
      return;
    }
  
  strcpy(dirslash,dirname);			// add final / if needed
  if(dirslash[strlen(dirslash)-1] != '/')
    strcat(dirslash,"/");
  
  if(html)
    {
      programWIDE_totalSent+=send(sock,header,strlen(header),0);
      programWIDE_totalSent+=send(sock,start_html,strlen(start_html),0);
    }

  while((de = readdir(d)))
    {		// read all dirents
      if(de->d_name[0] == '.')	// doesn't show hidden files
	continue;      
      strcpy(path,dirslash);
      strcat(path,de->d_name);
      if(!ls_follow_outside_links && !is_inside(path)) 
	continue;  //se is_inside ritorna -1 non lo mostro....
      stat(path,&st);

      if((int)st.st_size>=GIGA) 
	sprintf(weight,"%.2lf %s",(double)st.st_size/GIGA,"Gbytes");
      else if((int)st.st_size>=MEGA) 
	sprintf(weight,"%.2lf %s",(double)st.st_size/MEGA,"Mbytes");
      else  if((int)st.st_size>=KILO) 
	sprintf(weight,"%.2lf %s",(double)st.st_size/KILO,"Kbytes");
      else sprintf(weight,"%d %s",(int)st.st_size,"bytes");

      if(html)
	{ 
	  sprintf(row,"<tr>
                         <td>%s</td>
                         <td><a href=\"%s%s\">%s</a></td>
                         <td><DIV class=testo>%s</DIV></td>
                       </tr>\n\r",
		  S_ISDIR(st.st_mode)? directory:file,
		  de->d_name,
		  S_ISDIR(st.st_mode)? "/cmd:ls":"",
		  de->d_name,
		  weight); 
	}
      else 
	{
	  spaces=60-strlen(de->d_name);
	  if(spaces<0) spaces=1;
	  if((spacesbuf=(char *)malloc(spaces))==NULL)
	    {
	      fprintf(usr_cnsl,"Not enough memory!\n");
	      exit(1);
	    }
	  for(i=0;i<spaces-1;i++) spacesbuf[i]=' ';
	  spacesbuf[spaces-1]='\0';
	  sprintf(row,"%s %s%s%s\n",S_ISDIR(st.st_mode) ? "<DIR>" : "     ",de->d_name,spacesbuf,weight);
	  free(spacesbuf);
	}
      somethingwritten=1;
      programWIDE_totalSent+=send(sock,row,strlen(row),0); // send a row (a file)
      total+=(int)st.st_size;
    }
  
  if(!somethingwritten && html)  programWIDE_totalSent+=send(sock,nothing_html,strlen(nothing_html),0);
  if(!somethingwritten && !html) programWIDE_totalSent+=send(sock,nothing_txt,strlen(nothing_txt),0);

  if(somethingwritten && html)
    {
      if(total>=GIGA) 
	sprintf(weight,"%.2lf %s",(double)total/GIGA,"Gbytes");
      else if(total>=MEGA) 
	sprintf(weight,"%.2lf %s",(double)total/MEGA,"Mbytes");
      else  if(total>=KILO) 
	sprintf(weight,"%.2lf %s",(double)total/KILO,"Kbytes");
      else sprintf(weight,"%d %s",total,"bytes");

      sprintf(row,"<tr>
                      <td>&nbsp;</td>
                      <td><DIV class=testo><B>Total:</B></DIV></td>
                      <td><DIV class=testo><B>%s</B></DIV></td>
                   </tr>\n\r",
	      weight);
      programWIDE_totalSent+=send(sock,row,strlen(row),0);
    }

    if(somethingwritten && !html)
    {
      if(total>=GIGA) 
	sprintf(weight,"%.2lf %s",(double)total/GIGA,"Gbytes");
      else if(total>=MEGA) 
	sprintf(weight,"%.2lf %s",(double)total/MEGA,"Mbytes");
      else  if(total>=KILO) 
	sprintf(weight,"%.2lf %s",(double)total/KILO,"Kbytes");
      else sprintf(weight,"%d %s",total,"bytes");

      sprintf(row,"Total: %s\n",weight);
      programWIDE_totalSent+=send(sock,row,strlen(row),0);
    }

  if(html) programWIDE_totalSent+=send(sock,end_html,strlen(end_html),0);
  closedir(d);

  if (DEBUG) fprintf(usr_cnsl,"Directory list sent successfully\n");
  
  if(logflag)
    {
      time(&tempo);
      fprintf(logfile,"%s Directory list sent successfully\n",mytime(localtime(&tempo)));
      fflush(logfile);
    }
}


/*
 * invia un header appropriato se tutto ok, altrimenti non invia header.
 * invia un header solo a (m)ozilla, (o)pera o Wget......
 * read file restituisce : 1 (tutto ok) , 2 (e' finita la connessione) , 0 (errore)
 * se errore imposta n_errno a errno, una chiamata a write_error_msg imposta n_errno a -1
 */
int read_file(req_descriptor *reqd,int *sock)
{
  int i,j,totalBytes,bytesSent,totalSent,size,times;
  int stop;
  FILE *f;
  char *header =  "HTTP/1.1 200 OK\n"
    "Connection: close\n"
    "Content-Type: ";
  char head1[1024];
  char file_jpg_location[1024];
  char directory_jpg_location[1024];
  char styles_css_location[1024];
  struct stat st;
  f=NULL;

  sprintf(file_jpg_location,
	  "%s%sdata/file.jpg",rootdir,(rootdir[strlen(rootdir)-1]!='/') ? "/" : "");
  sprintf(directory_jpg_location,
	  "%s%sdata/directory.jpg",rootdir,(rootdir[strlen(rootdir)-1]!='/') ? "/" : "");
  sprintf(styles_css_location,
	  "%s%sdata/styles.css",rootdir,(rootdir[strlen(rootdir)-1]!='/') ? "/" : "");

  if(PDEBUG) printf("Entrato in read_file()\n");
  if(DEBUG) fprintf(usr_cnsl,"file: |%s|\n",reqd->what);
  if(logflag)
    { 
      time(&tempo);
      fprintf(logfile,"%s file: |%s|\n",mytime(localtime(&tempo)),reqd->what);
    }

  /***************file.jpg , directory.jpg , styles.css sono interni al programma******************/

  if(!strcmp(reqd->what,file_jpg_location))           // /data/file.jpg
    {
      time(&tempo);
      sprintf(head1,"%s%s\nDate: %s\nLast-Modified: %s\nContent-Length: %d\n\n",
	      header,
	      get_filedesc(".jpg"),
	      mytime(localtime(&tempo)),
	      mytime(localtime(&tempo)),
	      file_jpg_bufsize);
      
      if(needHeader(reqd)) 
	{
	  programWIDE_totalSent+=send(*sock,head1,strlen(head1),0);
	  if(DEBUG) fprintf(usr_cnsl,"Sent header:\n%s",head1);
	  if(logflag)
	    { 
	      time(&tempo);
	      fprintf(logfile,"%s Sent header:\n%s",mytime(localtime(&tempo)),head1);
	      fflush(logfile);
	    }
	}
      programWIDE_totalSent+=send(*sock,file_jpg,file_jpg_bufsize,0); 
      
      if(DEBUG) fprintf(usr_cnsl,"%d bytes read from file %s\n",file_jpg_bufsize,reqd->what);
      if(logflag) 
	{
	  time(&tempo);
	  fprintf(logfile,"%s %d bytes read from file %s\n",
		  mytime(localtime(&tempo)),
		  file_jpg_bufsize,
		  reqd->what);
	  fflush(logfile);
	}
      return 1;
    }

  else if(!strcmp(reqd->what,directory_jpg_location))    // /data/directory.jpg
    {
      time(&tempo);
      sprintf(head1,"%s%s\nDate: %s\nLast-Modified: %s\nContent-Length: %d\n\n",
	      header,
	      get_filedesc(".jpg"),
	      mytime(localtime(&tempo)),
	      mytime(localtime(&tempo)),
	      directory_jpg_bufsize);

      if(needHeader(reqd))
	{
	  programWIDE_totalSent+=send(*sock,head1,strlen(head1),0);	  
	  if(DEBUG) fprintf(usr_cnsl,"Sent header:\n%s",head1);
	  if(logflag)
	    { 
	      time(&tempo);
	      fprintf(logfile,"%s Sent header:\n%s",mytime(localtime(&tempo)),head1);
	      fflush(logfile);
	    }
	}

      programWIDE_totalSent+=send(*sock,directory_jpg,directory_jpg_bufsize,0);
      
      if(DEBUG) fprintf(usr_cnsl,"%d bytes read from file %s\n",directory_jpg_bufsize,reqd->what);
      if(logflag) 
	{
	  time(&tempo);
	  fprintf(logfile,"%s %d bytes read from file %s\n",
		  mytime(localtime(&tempo)),
		  directory_jpg_bufsize,
		  reqd->what);
	  fflush(logfile);
	}
      return 1;
    }


  else if(!strcmp(reqd->what,styles_css_location))    // /data/styles.css
    {
      time(&tempo);
      sprintf(head1,"%s%s\nDate: %s\nLast-Modified: %s\nContent-Length: %d\n\n",
	      header,
	      get_filedesc(".css"),
	      mytime(localtime(&tempo)),
	      mytime(localtime(&tempo)),
	      styles_css_bufsize);

      if(needHeader(reqd))
	{
	  programWIDE_totalSent+=send(*sock,head1,strlen(head1),0);	  
	  if(DEBUG) fprintf(usr_cnsl,"Sent header:\n%s",head1);
	  if(logflag)
	    { 
	      time(&tempo);
	      fprintf(logfile,"%s Sent header:\n%s",mytime(localtime(&tempo)),head1);
	      fflush(logfile);
	    }
	}

      programWIDE_totalSent+=send(*sock,styles_css,styles_css_bufsize,0);

      if(DEBUG) fprintf(usr_cnsl,"%d bytes read from file %s\n",styles_css_bufsize,reqd->what);
      if(logflag) 
	{
	  time(&tempo);
	  fprintf(logfile,"%s %d bytes read from file %s\n",
		  mytime(localtime(&tempo)),
		  styles_css_bufsize,
		  reqd->what);
	  fflush(logfile);
	}
      return 1;
    }

  /*********************************per tutti gli altri files**********************************/ 

  //controllo esistenza del file....
  f=fopen(reqd->what,"r");
  if(f==NULL) 
    {
      n_errno=errno;
      if(DEBUG) fprintf(usr_cnsl,"%s: %s\n",reqd->what,sys_errlist[n_errno]);
      if(logflag)
	{ 
	  time(&tempo);
	  fprintf(logfile,"%s %s: %s\n",mytime(localtime(&tempo)),reqd->what,sys_errlist[n_errno]);
	  fflush(logfile);
	}
      return 0; 
    }

  //controllo se posso andarlo a prendere
  if(!ls_follow_outside_links && !is_inside(reqd->what)) 
    {
      n_errno=EACCES;
      if(DEBUG) fprintf(usr_cnsl,"%s: %s\n",reqd->what,sys_errlist[n_errno]);
      if(logflag)
	{ 
	  time(&tempo);
	  fprintf(logfile,"%s %s: %s\n",mytime(localtime(&tempo)),reqd->what,sys_errlist[n_errno]);
	  fflush(logfile);
	}
      return 0;
    }

  //controllo se posso eseguire stat
  time(&tempo);
  if(stat(reqd->what,&st)==-1)
     {
       i=errno;
       if(DEBUG) fprintf(usr_cnsl,"%s: %s\n",reqd->what,sys_errlist[i]);
       if(logflag)
	 { 
	   time(&tempo);
	   fprintf(logfile,"%s %s: %s\n",mytime(localtime(&tempo)),reqd->what,sys_errlist[i]);
	   fflush(logfile);
	 }
       sprintf(head1,"%s%s\nDate: %s\n\n",header,get_filedesc(reqd->what),mytime(localtime(&tempo)));
     }  
   else sprintf(head1,"%s%s\nDate: %s\nLast-Modified: %s\nContent-Length: %d\n\n",
		header,
		get_filedesc(reqd->what),
		mytime(localtime(&tempo)),
		mytime(localtime(&(st.st_mtime))),
		(int)st.st_size);
   
  //controllo se devo inviare un header
  if(needHeader(reqd))
    {
      programWIDE_totalSent+=send(*sock,head1,strlen(head1),0);
      if(DEBUG) fprintf(usr_cnsl,"Sent header:\n%s",head1);
      if(logflag)
	{ 
	  time(&tempo);
	  fprintf(logfile,"%s Sent header:\n%s",mytime(localtime(&tempo)),head1);
	  fflush(logfile);
	}
    }
  
  //tutto ok, inizio a trasferire
  for(j=0;j<BUFSIZE;j++)ret_buf[j]='\0';
  totalBytes=0;
  times=0;
  stop=0;
  while((!stop)&&(!feof(f)))
    { 
      times++;
      i=fread(ret_buf,1,BUFSIZE,f);
      if(DEBUG) fprintf(usr_cnsl,"[%d]in read_file(PID:%d),while(!feof(f))=>i:%d\n",times,getpid(),i);
      if(logflag)
	{ 
	  time(&tempo);
	  fprintf(logfile,"%s [%d]in read_file(PID:%d),while(!feof(f))=>i:%d\n",mytime(localtime(&tempo)),times,getpid(),i);	  
	  if(times%10==0) fflush(logfile);
	}
      
      if(i==0) 
	{
	  //c'e' un errore in lettura?
	  if(ferror(f))
	    {
	      fclose(f);  
	      n_errno=errno;
	      if(DEBUG) fprintf(usr_cnsl,"%d bytes read from file %s,in %d times\n",
				totalBytes,reqd->what,times);
	      if(logflag) 
		{
		  time(&tempo);
		  fprintf(logfile,"%s %d bytes read from file %s,in %d times\n",
			  mytime(localtime(&tempo)),
			  totalBytes,
			  reqd->what,
			  times);
		  fflush(logfile);
		}
	      programWIDE_totalSent+=totalBytes;
	      return 0;
	    }
	  //o ho finito il file?
	  stop=1;
	  break;
	}

      //ho letto un blocco, lo invio......
      size=i;
      totalSent=0;
      do
	{
	  //bytesSent=send(*sock,ret_buf+totalSent,i-=totalSent,0);
	  bytesSent=send(*sock,ret_buf+totalSent,(((i-totalSent)/1024)>0)?1024:(i-totalSent),0);
	  if(PDEBUG) fprintf(usr_cnsl,"in read_file(PID:%d),send()=>bytesSent:%d\n",getpid(),bytesSent);
	  if(logflag)
	    { 
	      time(&tempo);
	      fprintf(logfile,"%s in read_file(PID:%d),send()=>bytesSent:%d\n",
		               mytime(localtime(&tempo)),getpid(),bytesSent);	  
	      fflush(logfile);
	    }

	  if(bytesSent==-1)
	    {
	      if(DEBUG) fprintf(usr_cnsl,"%d/%d bytes read from file %s,in %d times\n",
				totalBytes,(int)st.st_size,reqd->what,times);
	      if(logflag) 
		{
		  time(&tempo);
		  fprintf(logfile,"%s %d/%d bytes read from file %s,in %d times\n",
			  mytime(localtime(&tempo)),
			  totalBytes,
			  (int)st.st_size,
			  reqd->what,
			  times);
		  fflush(logfile);
		}
	      programWIDE_totalSent+=totalBytes; 
	      return 2;
	    }
	  totalSent+=bytesSent;
	}while(totalSent<size);
      totalBytes+=totalSent;
    }

  //finito di leggere il file, ricapitoliamo....
  if(DEBUG) fprintf(usr_cnsl,"%d/%d bytes read from file %s,in %d times\n",totalBytes,(int)st.st_size,reqd->what,times);
  if(logflag) 
    {
      time(&tempo);
      fprintf(logfile,"%s %d/%d bytes read from file %s,in %d times\n",
	      mytime(localtime(&tempo)),
	      totalBytes,
	      (int)st.st_size,
	      reqd->what,
	      times);
      fflush(logfile);
    }
  fclose(f);

  programWIDE_totalSent+=totalBytes;

  return 1;
}


/*
 * invia un file leggendo da uno stream gia' aperto !USATO SOLO PER CGI-BIN!
 */
int dread_file(FILE *filedes,char *filetype,int *sock)
{
  int i,j,totalBytes,bytesSent,totalSent,size,times;
  int stop;
  char *header =  "HTTP/1.1 200 OK\n"
    "Connection: close\n"
    "Content-Type: ";
  char head1[1024];
  struct stat st;

  time(&tempo);
  fstat(fileno(filedes),&st);
  sprintf(head1,"%s%s\nDate: %s\nLast-Modified: %s\nContent-Length: %d\n\n",
	  header,
	  get_filedesc(filetype),
	  mytime(localtime(&tempo)),
	  mytime(localtime(&(st.st_mtime))),
	  (int)st.st_size);
  programWIDE_totalSent+=send(*sock,head1,strlen(head1),0);
  if(DEBUG) fprintf(usr_cnsl,"%s Sent header:\n%s",mytime(localtime(&tempo)),head1);
  if(logflag)
    { 
      time(&tempo);
      fprintf(logfile,"%s Sent header:\n%s",mytime(localtime(&tempo)),head1);
      fflush(logfile);
    }   
  for(j=0;j<BUFSIZE;j++)ret_buf[j]='\0';
  totalBytes=0;
  times=0;
  stop=0;
  lseek(fileno(filedes),0,SEEK_SET);
  while((!stop)&&(!feof(filedes)))
    { 
      times++;
      i=fread(ret_buf,1,BUFSIZE,filedes);
      if(DEBUG) fprintf(usr_cnsl,"[%d]in dread_file(PID:%d),while(!feof(filedes))=>i:%d\n",
			times,getpid(),i);	  
      if(logflag)
	{ 
	  time(&tempo);
	  fprintf(logfile,"%s [%d]in dread_file(PID:%d),while(!feof(filedes))=>i:%d\n",
		  mytime(localtime(&tempo)),times,getpid(),i);	  
	  if(times%10==0) fflush(logfile);
	}

      if(i==0)
	{
	  if(ferror(filedes))
	    {
	      n_errno=errno;
	      if(DEBUG) fprintf(usr_cnsl,"%d bytes read from filedes,in %d times\n",totalBytes,times);
	      if(logflag) 
		{
		  time(&tempo);
		  fprintf(logfile,"%s %d bytes read from filedes,in %d times\n",
			  mytime(localtime(&tempo)),
			  totalBytes,
			  times);
		  fflush(logfile);
		}
	      programWIDE_totalSent+=totalBytes;
	      return 0;
	    }
	  stop=1;
	  break;
	}
      size=i;
      totalSent=0;
      do
	{
	  bytesSent=send(*sock,ret_buf+totalSent,i-=totalSent,0);
	  if(bytesSent==-1)
	    {
	      if(DEBUG) fprintf(usr_cnsl,"%d bytes read from filedes,in %d times\n",totalBytes,times); 
	      if(logflag) 
		{
		  time(&tempo);
		  fprintf(logfile,"%s %d bytes read from filedes,in %d times\n",
			  mytime(localtime(&tempo)),
			  totalBytes,
			  times);
		  fflush(logfile);
		}
	      programWIDE_totalSent+=totalBytes; 
	      return 2;
	    }
	  totalSent+=bytesSent;
	}while(totalSent<size);
      totalBytes+=totalSent;
    }
  if(DEBUG) fprintf(usr_cnsl,"%d bytes read from filedes,in %d times\n",totalBytes,times);
  if(logflag) 
    {
      time(&tempo);
      fprintf(logfile,"%s %d bytes read from filedes,in %d times\n",
	      mytime(localtime(&tempo)),
	      totalBytes,
	      times);
      fflush(logfile);
    }
  programWIDE_totalSent+=totalBytes;
  return 1;
}


/*
 * invia in continuazione i files mp3 in una directory
 */
void read_mp3_dir(req_descriptor *reqd,int *sock)
{
  DIR *d;
  struct dirent *de;
  req_descriptor *reqd_cpy;

  programWIDE_totalSent+=send(*sock,"ICY 200 OK",strlen("ICY 200 OK"),0);
  programWIDE_totalSent+=send(*sock,"icy-metaint:?????",strlen("icy-metaint:?????"),0);
  while(1)
    {
      d = opendir(reqd->what);
      while((de = readdir(d)))
	{		// read all dirents
	  if(de->d_name[0] == '.')	// doesn't show hidden files
	    continue;
	  if(!ISMP3(de->d_name)) continue;
	  
	  reqd_cpy=cpy_req_descriptor(reqd);

	  strcat(reqd_cpy->what,de->d_name);
	  
	  if(DEBUG) fprintf(usr_cnsl,"Sending: %s\n",reqd_cpy->what);
	  if(logflag)
	    { 
	      time(&tempo);
	      fprintf(logfile,"%s Sending: %s\n", mytime(localtime(&tempo)),reqd_cpy->what);	  
	      fflush(logfile);
	    }
	  if(read_file(reqd_cpy,sock)==2) return;
	  
	}
    }
}


/*
 * invia un file mp3
 */
void read_mp3_file(req_descriptor *reqd,int *sock)
{
  struct stat st;
  char metaint[1024];
  char *err="";
 
  stat(reqd->what,&st);
  sprintf(metaint,"icy-metaint:%d",(int)st.st_size);

  if(ISMP3(reqd->what))
    {
      programWIDE_totalSent+=send(*sock,"ICY 200 OK",strlen("ICY 200 OK"),0);
      programWIDE_totalSent+=send(*sock,metaint,strlen(metaint),0);
      if(!read_file(reqd,sock)) 
	send(*sock,err=write_error_msg(reqd,(char*)sys_errlist[n_errno]),strlen(err),0); 
    }
}

/*
 * invia un file html leggendo da un file descriptor già aperto !USATO SOLO PER CGI-BIN!
 */
void dread_html_file(FILE *filedes,int *sock)
{
    char *err;
    if(!dread_file(filedes,".htm",sock))
      {
	err=write_error_msg(NULL,(char *)sys_errlist[n_errno]);
	send(*sock,err,strlen(err),0); 
	
	if(DEBUG) fprintf(usr_cnsl,"Sent error:\n%s\n",err);
	if(logflag)
	  { 
	       time(&tempo);
	       fprintf(logfile,"%s Sent error:\n%s\n",mytime(localtime(&tempo)),err);
	       fflush(logfile);
	  }
      }
}

/*
 * invia un file HTML , 
 * se il file <nomefile>.htm non viene trovato prova con il file <nomefile>.html 
 * prima di inviare un errore e viceversa
 */
void read_html_file(req_descriptor *reqd,int *sock)
{
   char *err;

   if(ISHTM(reqd->what) && !read_file(reqd,sock))
     { 
       strcat(reqd->what,"l"); 
       if(!read_file(reqd,sock))
	 {
	   err=write_error_msg(reqd,(char *)sys_errlist[n_errno]);
	   send(*sock,err,strlen(err),0); 
	   
	   if(DEBUG) fprintf(usr_cnsl,"Sent error:\n%s\n",err);
	   if(logflag)
	     { 
	       time(&tempo);
		fprintf(logfile,"%s Sent error:\n%s\n",mytime(localtime(&tempo)),err);
		fflush(logfile);
	     }
	 }
       return;
     }
   else if(strstr(reqd->what,".html") && !read_file(reqd,sock))
     {
       reqd->what[strlen(reqd->what)-1]='\0';
	if(!read_file(reqd,sock))
	  {
	    err=write_error_msg(reqd,(char *)sys_errlist[n_errno]);
	    send(*sock,err,strlen(err),0);

	    if(DEBUG) fprintf(usr_cnsl,"Sent error:\n%s\n",err);
	    if(logflag)
	      { 
		time(&tempo);
		fprintf(logfile,"%s Sent error:\n%s\n",mytime(localtime(&tempo)),err);
		fflush(logfile);
	      }
	  } 
     }
}


/*
 * invia un qualsiasi file
 */
void read_any_file(req_descriptor *reqd,int *sock)
{
  char *err;
  
  if(!read_file(reqd,sock))
    {
      err=write_error_msg(reqd,(char *)sys_errlist[n_errno]);
      send(*sock,err,strlen(err),0);
      
      if(DEBUG) fprintf(usr_cnsl,"Sent error:\n%s\n",err);
      if(logflag)
	   { 
	     time(&tempo);
	     fprintf(logfile,"%s Sent error:\n%s\n",mytime(localtime(&tempo)),err);
	     fflush(logfile);
	   }
    }
}
