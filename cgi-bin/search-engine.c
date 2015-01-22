#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFSIZE 32768
char wsmp3_rootDIR[BUFSIZE];
char **DIRS;
char *directory = "<IMG src=\"/data/directory.jpg\" height=16 width=16>";
char *file = "<IMG src=\"/data/file.jpg\" height=16 width=16>";
char *startfrom;
int somethingwritten=0;

int findit(char *where,char *what)
{
  char *cp=where;
  //  printf("findit(%s,%s)\n",where,what);
  while(cp+(strlen(what)-1)<where+(strlen(where)-1))
    { 
      if(!strncmp(cp,what,strlen(what))) return 1;
      cp++;
    }
  return 0;
}

int istext(char *filename)
{
  char *cp;
  cp=filename+(strlen(filename)-1);
  while(*cp!='.' && cp!=filename) cp--;
  if(cp==filename) return 0;
  if((!strcmp(cp,".txt") || !strcmp(cp,".htm"))|| !strcmp(cp,".html")) return 1;
  return 0;
}

int is_inside(char *path)
{
  struct stat st;
  int i;
  char pointed[1024];
  
  for(i=0;i<1024;i++) pointed[i]='\0';

  if(!strcmp(path,startfrom)) return 0;

  lstat(path,&st);

  if( (S_ISDIR(st.st_mode) || S_ISREG(st.st_mode)) 
      && 
      (strstr(path,startfrom)!=path) ) 
    {
      i=0;
      while(DIRS[i]!=NULL)
	{
	  if(strstr(path,DIRS[i])==path) return 1;
	  i++;
	}
      return -1;
    }

  else if(S_ISLNK(st.st_mode))
    {
      readlink(path,pointed,1024);
      if((strstr(pointed,"..")==pointed) && strstr(path,startfrom)==path) return -1;
      if(pointed[0]=='.' && (pointed[1]=='/' || pointed[1]=='\0')) return 0;
      return is_inside(pointed);
    }
  return 1;
}


void search_filename_isin_dir(char *searched,char *startfrom,const int r)
{
  struct dirent *de;
  struct stat st;
  char buf[1024];
  DIR *d;

  d=opendir(startfrom);
  if(d==NULL) return;
  while((de=readdir(d)))
    {
      if(de->d_name[0]=='.') continue;
      strcpy(buf,startfrom);
      if(buf[strlen(buf)-1]!='/') strcat(buf,"/");
      strcat(buf,de->d_name);
      stat(buf,&st);
      
      if((r) 
	 && 
	 ( S_ISDIR(st.st_mode) && (is_inside(buf)==1) )
	)
	search_filename_isin_dir(searched,buf,r);
      
      if(strstr(de->d_name,searched) && (is_inside(buf)==1) )
	{ 
	printf("<TR><TD>%s</TD><TD><a href=\"%s%s\"><DIV class=testo>%s</DIV></a></TD></TR>\n",
	       (S_ISDIR(st.st_mode)) ? directory : file,
	       ((buf+strlen(wsmp3_rootDIR))[0]=='/') ? 
	                  buf+strlen(wsmp3_rootDIR) :
	                  buf+(strlen(wsmp3_rootDIR)-1) ,
	       (S_ISDIR(st.st_mode)) ? "/cmd:ls":"",
	       buf+strlen(wsmp3_rootDIR));
	somethingwritten=1;
	}
    }
  return;
}

void search_filecontent_isin_dir(char *searched,char *startfrom,const int r)
{
  struct dirent *de;
  struct stat st;
  char buf[1024];
  char pointed[1024];
  DIR *d;
  FILE *f;
  int i,writeit;

  d=opendir(startfrom);
  if(d==NULL) return;
  while((de=readdir(d)))
    {
      writeit=0;
      if(de->d_name[0]=='.') continue;
      strcpy(buf,startfrom);
      if(buf[strlen(buf)-1]!='/') strcat(buf,"/");
      strcat(buf,de->d_name);
      stat(buf,&st);
      
      if((r) 
	 && 
	 ( S_ISDIR(st.st_mode) && (is_inside(buf)==1) )
	)
	search_filecontent_isin_dir(searched,buf,r);
      
      if(S_ISDIR(st.st_mode)) continue;
      if(!istext(buf)) continue;
      f=fopen(buf,"r");
      if(f!=NULL)
	{
	  while(!feof(f))
	    {
	      for(i=0;i<1024;i++) pointed[i]='\0';
	      i=fread(pointed,1024,1,f);
	      if(findit(pointed,searched)) {writeit=1; break;}
	    }
	  fclose(f);

	  if(writeit)
	    {
	      printf("<TR><TD>%s</TD><TD><a href=\"%s%s\"><DIV class=testo>%s</DIV></a></TD></TR>\n",
		     (S_ISDIR(st.st_mode)) ? directory : file,
		     ((buf+strlen(wsmp3_rootDIR))[0]=='/') ? 
		     buf+strlen(wsmp3_rootDIR) :
		     buf+(strlen(wsmp3_rootDIR)-1) ,
		     (S_ISDIR(st.st_mode)) ? "/cmd:ls":"",
		     buf+strlen(wsmp3_rootDIR));
	      somethingwritten=1;
	    }
	}
    }
  return;
}

int main(int argc,char *argv[],char *environ[])
{
  char *searched,*buf,*buf2;
  char *inizio,*fine,**ora;
  char what;
  int r,i;
  FILE *console;

  printf("<HTML><HEAD><TITLE>Search Finished</TITLE><LINK rel=stylesheet href=\"/data/styles.css\" type=\"text/css\"></HEAD><BODY><TABLE border=0>\n");
  if((searched=(char *)malloc(strlen(argv[1])))==NULL)
    {
      printf("<TR><TD>Error During execution!!</TR></TD>");
      printf("</TABLE></BODY></HTML>\n");
      exit(0);
    }
  if((buf=(char *)malloc(strlen(argv[1])))==NULL)
    {
      printf("<TR><TD>Error During execution!!</TR></TD>");
      printf("</TABLE></BODY></HTML>\n");
      exit(0);
    }
  if((startfrom=(char *)malloc(1024))==NULL)
    {
      printf("<TR><TD>Error During execution!!</TR></TD>");
      printf("</TABLE></BODY></HTML>\n");
      exit(0);
    }
  if((DIRS=(char **)malloc(1024*sizeof(char *)))==NULL)
    {
      printf("<TR><TD>Error During execution!!</TR></TD>");
      printf("</TABLE></BODY></HTML>\n");
      exit(0);
    }
  /************************ AMBIENTE ******************************/

  console=fopen("/dev/console","a");
  strcpy(wsmp3_rootDIR,getenv("wsmp3_rootDIR"));
  fprintf(console,"wsmp3_rootDIR:%s\n",wsmp3_rootDIR);
  
  if(getenv("wsmp3_accepted")!=NULL) 
    {
      buf2=(char *)malloc(BUFSIZE);
      for(i=0;i<BUFSIZE;i++) buf2[i]='\0';
      strncpy(buf2,getenv("wsmp3_accepted"),strlen(getenv("wsmp3_accepted")));
      ora=DIRS;
      inizio=buf2;
      while(*inizio!='\0')
	{
	  fine=inizio;
	  while(*fine!=':'){if(*fine=='\0')break; fine++;}
	  if(fine[0] == ':') *fine='\0';
	  *ora=(char *)malloc(1024);
	  strcpy(*ora,inizio); ora++;
	  inizio=fine+1;
	  if(*inizio=='\0')
	    break;
	}
      *ora=NULL;
    }
  
  /*****************************ACQUISIZIONE DATI***********************/
  strcpy(buf,argv[1]);
  inizio=strstr(buf,"stringa=")+strlen("stringa=");
  fine=strstr(buf,"&cosa=");
  *fine='\0';
  strcpy(searched,inizio);
  inizio=searched;
  while(*inizio!='\0')      //   '+' in ' ' ; 
    {
      if(*inizio=='+') *inizio=' ';
      inizio++;
    }
  
  strcpy(buf,argv[1]);
  inizio=strstr(buf,"cosa=")+strlen("cosa=");
  fine=strstr(buf,"&metodo=");
  *fine='\0';
  if(!strcmp(inizio,"nome_file")) what='n';
  else what='c';

  strcpy(buf,argv[1]);
  inizio=strstr(buf,"metodo=")+strlen("metodo=");
  fine=strstr(buf,"&startfrom=");
  *fine='\0';
  if(!strcmp(inizio,"ricorsivo")) r=1;
  else r=0;

  strcpy(buf,argv[1]);
  inizio=strstr(buf,"startfrom=")+strlen("startfrom=");
  if(!strcmp(inizio,"%2F")) strcpy(startfrom,wsmp3_rootDIR);
  else 
    {
      strcpy(startfrom,wsmp3_rootDIR);
      while(*inizio!='\0') 
	{
	  if((*inizio=='%' && *(inizio+1)=='2') && *(inizio+2)=='F') 
	    {
	      strcat(startfrom,"/");
	      inizio+=3; 
	    }
	  else
	    {
	      buf[0]=inizio[0];
	      buf[1]='\0';
	      strcat(startfrom,buf);
	      inizio++;
	    }
	}
    }
  
  /*****************************ELABORAZIONE DATI***********************/
  //printf("<HTML><HEAD><TITLE>Search Finished</TITLE></HEAD><BODY>\n");
  if(r)     //ricorsivamente
    {
      if(what=='n')  //per nomefile
	{
	  search_filename_isin_dir(searched,startfrom,1);
	  if(!somethingwritten)
	    printf("<TR><TD><DIV class=testo><H3>Search Failed!</H3></DIV></TD></TR>\n");
	}
      else          //per contenuto
	{
	  search_filecontent_isin_dir(searched,startfrom,1);
	  if(!somethingwritten)
	    printf("<TR><TD><DIV class=testo><H3>Search Failed!</H3></DIV></TD></TR>\n");
	}
    }
  else      //non ricorsivamente
    {
      if(what=='n')  //per nomefile
	{
	  search_filename_isin_dir(searched,startfrom,0);
	  if(!somethingwritten)
	    printf("<TR><TD><DIV class=testo><H3>Search Failed!</H3></DIV></TD></TR>\n");
	}
      else          //per contenuto
	{
	  search_filecontent_isin_dir(searched,startfrom,0);
	  if(!somethingwritten)
	    printf("<TR><TD><DIV class=testo><H3>Search Failed!</H3></DIV></TD></TR>\n");
	}
    }
  printf("</TABLE></BODY></HTML>\n");
  return 0;
}

