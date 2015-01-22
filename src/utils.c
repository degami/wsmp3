#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "defines.h"

/*
 * restituisce una data in formato stringa
 */
char *mytime(struct tm *src)
{
  char *str;
  char *mesi[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  char *giorni[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

                      /*
	              struct tm {
                      int     tm_sec;          seconds 
                      int     tm_min;          minutes
                      int     tm_hour;         hours 
                      int     tm_mday;         day of the month 
                      int     tm_mon;          month 
                      int     tm_year;         year 
                      int     tm_wday;         day of the week 
                      int     tm_yday;         day in the year 
                      int     tm_isdst;        daylight saving time 
		      };*/

  str=malloc(30);
  sprintf(str,"%s %s %02d %02d:%02d:%02d %d%c",giorni[src->tm_wday],mesi[src->tm_mon],src->tm_mday,src->tm_hour,src->tm_min,src->tm_sec,1900+src->tm_year,'\0');
  return str;
}


/*
 * non fa nulla :)
 */
void nop()
{
}


/*
 * se un file ha estensione .mp3 restituisce 1 , 0 altrimenti
 */
int ISMP3(char *nomefile)
{
  if((nomefile[strlen(nomefile)-1]=='3' && 
      (nomefile[strlen(nomefile)-2]=='p' || nomefile[strlen(nomefile)-2]=='P')) && 
     (
      (nomefile[strlen(nomefile)-3]=='m' || nomefile[strlen(nomefile)-3]=='M') && 
       nomefile[strlen(nomefile)-4]=='.'
     )
    ) return 1;
  else return 0;
}


/*
 * se un file ha estensione .htm restituisce 1 , 0 altrimenti
 */
int ISHTM(char *nomefile)
{
  if( 
     (
       (nomefile[strlen(nomefile)-1]=='m' || nomefile[strlen(nomefile)-1]=='M') && 
       (nomefile[strlen(nomefile)-2]=='t' || nomefile[strlen(nomefile)-2]=='T')
     )
      &&
     ( 
      (nomefile[strlen(nomefile)-3]=='h' || nomefile[strlen(nomefile)-3]=='H') && 
      nomefile[strlen(nomefile)-4]=='.'
     )
    ) return 1;
  else return 0;
}


/*
 * se la dimensione del file filename e' 
 * maggiore di FORKON restituisce 1 , 0 altrimenti
 */
int shallIfork(char* filename)
{
  struct stat st;

  if(PDEBUG) printf("Entrato in shallIfork()\n");
  if(stat(filename,&st)==-1) return 0;
  else { if((int)st.st_size>FORKON) return 1; }
  return 0;
}


/*
 * controlla se path è interno o no a wsmp3_rootDIR o wsmp3_accepted
 * ritorna 1 ( e' interno ) 0 ( non è interno )
 */
int is_inside(char *path)
{
  struct stat st;
  int i;
  char *cp;
  char pointed[1024];
  char curp[1024];
  char root_dir[1024];
  char superdir[1024];
  strcpy(root_dir,rootdir); if(root_dir[strlen(root_dir)-1]=='/') root_dir[strlen(root_dir)-1]='\0';
  strcpy(curp,path); if(strcmp(curp,"/") && curp[strlen(curp)-1]=='/') curp[strlen(curp)-1]='\0' ;
  strcpy(superdir,curp);
  cp=superdir+(strlen(superdir)-1);
  while(cp>superdir && *(cp-1)!='/') cp--; *cp='\0';

  if(!strcmp(curp,root_dir)) return 1;

  if(lstat(curp,&st)==-1) return 0;
  if( (S_ISDIR(st.st_mode) || S_ISREG(st.st_mode)) && (strstr(curp,rootdir)!=curp) )
    {
      i=0;
      while(DIRS[i]!=NULL)
	{
	  if(strstr(curp,DIRS[i])==curp) return 1;
	  i++;
	}
      return 0;
    }
  else if(S_ISLNK(st.st_mode))
    {
      for(i=0;i<1024;i++) pointed[i]='\0';
      readlink(curp,pointed,1024);
      if((strstr(pointed,"..")==pointed) && !strcmp(superdir,root_dir)) return 0;
      if(pointed[0]=='.' && (pointed[1]=='/' || pointed[1]=='\0')) return 1;
      return is_inside(pointed);
    }
  return (1 && is_inside(superdir));
}


/** Convert a two-char hex string into the char it represents. **/
char x2c(char *what) {
   register char digit;

   digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
   return(digit);
}

/** Reduce any %xx escape sequences to the characters they represent. **/
void unescape_url(char *url) {
    register int i,j;

    for(i=0,j=0; url[j]; ++i,++j) {
        if((url[i] = url[j]) == '%') {
            url[i] = x2c(&url[j+1]) ;
            j+= 2 ;
        }
    }
    url[i] = '\0' ;
}


/*
 * restituisce il nome del file richiesto (tenendo conto della variabile rootdir)
 * applica eventuali conversioni per caratteri speciali
 */
char* nomefile(char *buf,const int msg)
{
  char *cp, *cp2,*ritorno;
  int i;
  //  struct stat st;

  cp=buf+(strlen(get_op(buf))+2); //buf+5;
  cp2=strstr(cp," HTTP"); if(cp2!=NULL) *cp2='\0';
  cp2=strstr(cp,"\r\n"); if(cp2==cp) return NULL;
  while(cp[0]=='.' || cp[0]=='/') 
    for(i=1;cp[i]!='\0';i++)
      { 
	cp[i-1]=cp[i];
	if(cp[i+1]=='\0') cp[i]='\0';
      }
  
  if((ritorno=(char *)malloc(1024))==NULL)
    {
      fprintf(usr_cnsl,"Not enough memory!\n");
      exit(1);
    }
  else for(i=0;i<1024;i++) ritorno[i]='\0';

  if(rootdir!=NULL && !msg)
    { 
      strcpy(ritorno,rootdir);
      if(ritorno[strlen(ritorno)-1]!='/') strcat(ritorno,"/");
    }

  
  unescape_url(cp);
  strcat(ritorno,cp);
  /*if(ritorno[strlen(ritorno)-1]!='/')  //non so bene se e' un file o una dir
    {
      if(stat(ritorno,&st)==-1)
	{
	  return ritorno;             //non riesco ad aprirlo.. l'errore verra' segnalato dopo..
	}
      else if(S_ISDIR(st.st_mode))    //ora lo so.....
	{
	  strcat(ritorno,"/");
	}
    }*/
  return ritorno;
}


/*
 * restituisce l'operazione richiesta
 */
char* get_op(char *buf)
{
  char* op;
  int i,finded_flag;
  if((op=(char *)malloc(10))==NULL)
    {
      fprintf(usr_cnsl,"Not enough memory!\n");
      exit(1);
    }
  else for(i=0;i<10;i++) op[i]='\0';

  if(buf!=NULL && strlen(buf)>=3)
    { 
      i=0;
      while(buf[i]!=' ' && ((buf[i]!='\0' && buf[i]!='\n') && buf[i]!='\r'))
	{
	  op[i]=buf[i];
	  i++;
	}
      op[i]='\0';
      
      i=0;finded_flag=0;
      while(ACCEPTED_METHODS[i]!=NULL)
	{
	  if(!strcmp(op,ACCEPTED_METHODS[i])) finded_flag=1;
	  i++;
	}
      if(!finded_flag) op=NULL;
    }
  else op=NULL;
  return op;
}


/*
 * restituisce la riga della stringa buf che inizia per *line (senza *line)
 */
char* gimme_line(char *buf,char *line)
{
  char *start,*end,*ritorno;
  int i;
  if(strstr(buf,line)==NULL) return NULL;

  start=(strstr(buf,line)+strlen(line));
  end=start;
  while((*end!='\n' && *end!='\r') &&((end-start)<1024 )) end++;
  *end='\0';

  if((ritorno=(char *)malloc(1024))==NULL)
    {
      fprintf(usr_cnsl,"Not enough memory!\n");
      exit(1);
    }
  for(i=0;i<1024;i++) ritorno[i]='\0';
  strncpy(ritorno,start,1024); ritorno[1023]='\0';
  return ritorno;
}


/*
 * restituisce la riga della stringa buf che contiene informazioni (dopo il \n\n)
 */
char* gimme_content(char *buf)
{
  char *start,*ritorno; //,*end;
  int i;

  start=strstr(buf,"Content-Length:");
  if(start==NULL) return NULL;
  else while(*start!='\n') start++;               //arrivo sopra il primo '\n'
  if(*(start+1)=='\r') start++;                   //niente niente mi hanno mandato '\r\n' ?
  if(*(++start)=='\n') start++;                   //se trovo anche il secondo vado avanti

  if((ritorno=(char *)malloc(1024))==NULL)
    {
      fprintf(usr_cnsl,"Not enough memory!\n");
      exit(1);
    }

  for(i=0;i<1024;i++) ritorno[i]='\0';
  strncpy(ritorno,start,1024);ritorno[1023]='\0'; //ora posso copiare;
  if(ritorno[0]=='\0') 
    { 
      if(PDEBUG) printf("get_content - elimino ritorno..\n");
      free(ritorno);
      return NULL;
    }             //se non ho copiato niente....
  return ritorno;
}


/*
 * restituice la descrizione per un header http di un file
 */
char* get_filedesc(char *filename)
{
  char *ritorno=
  (strstr(filename,".htm")!=NULL) ? "text/html" : 
   (strstr(filename,".txt")!=NULL) ? "text/plain" :
    (strstr(filename,".rtx")!=NULL) ? "text/richtext" :
     (strstr(filename,".css")!=NULL) ? "text/css" :
      (strstr(filename,".jpg")!=NULL) ? "image/jpeg" :
       (strstr(filename,".jpe")!=NULL) ? "image/jpeg" :
        (strstr(filename,".jpeg")!=NULL) ? "image/jpeg" :
         (strstr(filename,".gif")!=NULL) ? "image/gif" :
          (strstr(filename,".png")!=NULL) ? "image/png" :
           (strstr(filename,".tif")!=NULL) ? "image/tiff" :
            (strstr(filename,".tiff")!=NULL) ? "image/tiff" :
             (strstr(filename,".xbm")!=NULL) ? "image/x-xbitmap" :
              (strstr(filename,".pdf")!=NULL) ? "application/pdf" :
               (strstr(filename,".ai")!=NULL) ? "application/postscript" :
                (strstr(filename,".eps")!=NULL) ? "application/postscript" :
                 (strstr(filename,".ps")!=NULL) ? "application/postscript" :
                  (strstr(filename,".rtf")!=NULL) ? "application/rtf" :
                   (strstr(filename,".csh")!=NULL) ? "application/x-csh" :
                    (strstr(filename,".tcl")!=NULL) ? "application/x-tcl" :
                     (strstr(filename,".tex")!=NULL) ? "application/x-tex" :
                      (strstr(filename,".zip")!=NULL) ? "application/zip" :
                       (strstr(filename,".bcpio")!=NULL) ? "application/x-bcpio" :
                        (strstr(filename,".cpio")!=NULL) ? "application/x-cpio" :
                         (strstr(filename,".au")!=NULL) ? "audio/basic" :
                          (strstr(filename,".snd")!=NULL) ? "audio/basic" :
                           (strstr(filename,".aif")!=NULL) ? "audio/x-aiff" :
                            (strstr(filename,".aiff")!=NULL) ? "audio/x-aiff" :
                             (strstr(filename,".aifc")!=NULL) ? "audio/x-aiff" :
                              (strstr(filename,".mpeg")!=NULL) ? "video/mpeg" :
                               (strstr(filename,".mpg")!=NULL) ? "video/mpeg" :
                                (strstr(filename,".mpe")!=NULL) ? "video/mpeg" :
                                 (strstr(filename,".qt")!=NULL) ? "video/quicktime" :
                                  (strstr(filename,".mov")!=NULL) ? "video/quicktime" :
                                   (strstr(filename,".avi")!=NULL) ? "x-video/msvideo" :
                                    (strstr(filename,".movie")!=NULL) ? "video/x-sgi-movie" :
    "*/*";
    return ritorno;
}

