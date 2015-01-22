#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "req_descriptor.h"
#include "defines.h"
#include "utils.h"

/*
 * reinizializza una struct req_descriptor
 */
void reinit_req_descriptor(req_descriptor *desc)
{
  int i;
  if(desc->action!=NULL)     for(i=0;i<10;i++)   desc->action[i]='\0';
  if(desc->what!=NULL)       for(i=0;i<1024;i++) desc->what[i]='\0';
  if(desc->host!=NULL)       for(i=0;i<1024;i++) desc->host[i]='\0';
  if(desc->agent!=NULL)      for(i=0;i<1024;i++) desc->agent[i]='\0';
  if(desc->accept!=NULL)     for(i=0;i<1024;i++) desc->accept[i]='\0';
  if(desc->lang!=NULL)       for(i=0;i<1024;i++) desc->lang[i]='\0';
  if(desc->enc!=NULL)        for(i=0;i<1024;i++) desc->enc[i]='\0';
  if(desc->charset!=NULL)    for(i=0;i<1024;i++) desc->charset[i]='\0';
  if(desc->keep!=NULL)       for(i=0;i<1024;i++) desc->keep[i]='\0';
  if(desc->conn!=NULL)       for(i=0;i<1024;i++) desc->conn[i]='\0';
  if(desc->referer!=NULL)    for(i=0;i<1024;i++) desc->referer[i]='\0';
  if(desc->pragma!=NULL)     for(i=0;i<1024;i++) desc->pragma[i]='\0';
  if(desc->contType!=NULL)   for(i=0;i<1024;i++) desc->contType[i]='\0';
  if(desc->contLength!=NULL) for(i=0;i<1024;i++) desc->contLength[i]='\0';
  if(desc->content!=NULL)    for(i=0;i<1024;i++) desc->content[i]='\0';
}


/*
 * copia una struct req_descriptor in un'altra
 */
req_descriptor * cpy_req_descriptor(req_descriptor *desc)
{
  req_descriptor *ritorno;
  if( (ritorno=((req_descriptor *)malloc(sizeof(req_descriptor))))==NULL )
    {
      fprintf(usr_cnsl,"Not enought memory!!!\n");
      exit(1);
    }
  //else init_req_descriptor(ritorno);

  if((ritorno->action=(char *)malloc(10))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->action!=NULL) strcpy(ritorno->action,desc->action);
  else ritorno->action=NULL;

  if((ritorno->what=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->what!=NULL) strcpy(ritorno->what,desc->what);
  else ritorno->what=NULL;

  if((ritorno->host=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->host!=NULL) strcpy(ritorno->host,desc->host);
  else ritorno->host=NULL;

  if((ritorno->agent=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->agent!=NULL) strcpy(ritorno->agent,desc->agent);
  else ritorno->agent=NULL;

  if((ritorno->accept=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->accept!=NULL) strcpy(ritorno->accept,desc->accept);
  else ritorno->accept=NULL;

  if((ritorno->lang=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->lang!=NULL) strcpy(ritorno->lang,desc->lang);
  else ritorno->lang=NULL;

  if((ritorno->enc=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->enc!=NULL) strcpy(ritorno->enc,desc->enc);
  else ritorno->enc=NULL;

  if((ritorno->charset=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->charset!=NULL) strcpy(ritorno->charset,desc->charset);
  else ritorno->charset=NULL;

  if((ritorno->keep=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->keep!=NULL) strcpy(ritorno->keep,desc->keep);
  else ritorno->keep=NULL;

  if((ritorno->conn=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->conn!=NULL) strcpy(ritorno->conn,desc->conn);
  else ritorno->conn=NULL;

  if((ritorno->referer=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->referer!=NULL) strcpy(ritorno->referer,desc->referer);
  else ritorno->referer=NULL;

  if((ritorno->pragma=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->pragma!=NULL) strcpy(ritorno->pragma,desc->pragma);
  else ritorno->pragma=NULL;

  if((ritorno->contType=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->contType!=NULL) strcpy(ritorno->contType,desc->contType);
  else ritorno->contType=NULL;

  if((ritorno->contLength=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->contLength!=NULL) strcpy(ritorno->contLength,desc->contLength);
  else ritorno->contLength=NULL;

  if((ritorno->content=(char *)malloc(1024))==NULL){fprintf(usr_cnsl,"Not enough memory!\n");exit(1);}
  if(desc->content!=NULL) strcpy(ritorno->content,desc->content);
  else ritorno->content=NULL;

  return ritorno;
}


/*
 * cancella una struct req_descriptor
 */
void rem_req_descriptor(req_descriptor *desc)
{
  int i;
  if(PDEBUG) printf("entro in rem_req_descriptor()\n");
  if((desc->action)!=NULL)     
    {for(i=0;i<10;i++) desc->action[i]='\0';free(desc->action);desc->action=NULL;} 
  if((desc->what)!=NULL)       
    {for(i=0;i<1024;i++) desc->what[i]='\0';free(desc->what);desc->what=NULL;}
  if((desc->host)!=NULL)       
    {for(i=0;i<1024;i++) desc->host[i]='\0';free(desc->host);desc->host=NULL;}
  if((desc->agent)!=NULL)      
    {for(i=0;i<1024;i++) desc->agent[i]='\0';free(desc->agent);desc->agent=NULL;}
  if((desc->accept)!=NULL)     
    {for(i=0;i<1024;i++) desc->accept[i]='\0';free(desc->accept);desc->accept=NULL;}
  if((desc->lang)!=NULL)       
    {for(i=0;i<1024;i++) desc->lang[i]='\0';free(desc->lang);desc->lang=NULL;}
  if((desc->enc)!=NULL)        
    {for(i=0;i<1024;i++) desc->enc[i]='\0';free(desc->enc);desc->enc=NULL;}
  if((desc->charset)!=NULL)    
    {for(i=0;i<1024;i++) desc->charset[i]='\0';free(desc->charset);desc->charset=NULL;}
  if((desc->keep)!=NULL)       
    {for(i=0;i<1024;i++) desc->keep[i]='\0';free(desc->keep);desc->keep=NULL;}
  if((desc->conn)!=NULL)       
    {for(i=0;i<1024;i++) desc->conn[i]='\0';free(desc->conn);desc->conn=NULL;}
  if((desc->referer)!=NULL)    
    {for(i=0;i<1024;i++) desc->referer[i]='\0';free(desc->referer);desc->referer=NULL;}
  if((desc->pragma)!=NULL)     
    {for(i=0;i<1024;i++) desc->pragma[i]='\0';free(desc->pragma);desc->pragma=NULL;}
  if((desc->contType)!=NULL)   
    {for(i=0;i<1024;i++) desc->contType[i]='\0';free(desc->contType);desc->contType=NULL;}
  if((desc->contLength)!=NULL) 
    {for(i=0;i<1024;i++) desc->contLength[i]='\0';free(desc->contLength);desc->contLength=NULL;}
  if((desc->content)!=NULL)    
    {for(i=0;i<1024;i++) desc->content[i]='\0';free(desc->content);desc->content=NULL;}
  if(PDEBUG) printf("campi eliminati....\n");
  if(desc!=NULL) {/*free(desc);*/desc=NULL;}
  if(PDEBUG) printf("esco da rem_req_descriptor()\n");
}


/*
 * stampa una struct req_descriptor in una string
 */
char* print_req_descriptor(req_descriptor *desc)
{
  char *ritorno;
  if((ritorno=(char *)malloc(1024*15))==NULL)
    {
      printf("Not enough memory!\n");
      exit(1);
    }

  sprintf(ritorno,"action: |%s|\nwhat: |%s|\nhost: |%s|\nagent: |%s|\naccept: |%s|\nlang: |%s|\nenc: |%s|\ncharset: |%s|\nkeep: |%s|\nconn: |%s|\nrefer: |%s|\npragma: |%s|\ncontType: |%s|\ncontLength: |%s|\ncontent: |%s|\n",
	  desc->action,
	  desc->what,
	  desc->host,
	  desc->agent,
	  desc->accept,
	  desc->lang,
	  desc->enc,
	  desc->charset,
	  desc->keep,
	  desc->conn,
	  desc->referer,
	  desc->pragma,
	  desc->contType,
	  desc->contLength,
	  desc->content);
  return ritorno;
}


/*
 * crea una nuova struct req_descriptor dalla stringa passata
 * alloca memoria, la inizializza e riempie i campi
 */
req_descriptor* parse_request(char *req)
{
  req_descriptor* ritorno;
  char reqcpy[BUFSIZE];
  int i;
  
  if( (ritorno=((req_descriptor *)malloc(sizeof(req_descriptor))))==NULL )
    {
      fprintf(usr_cnsl,"Not enought memory!!!\n");
      exit(1);
    }
  //else init_req_descriptor(ritorno);

  if(PDEBUG) printf("Entro in parse_request\n");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->action=get_op(reqcpy);

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  if(ritorno->action!=NULL)
    {
      if(!strcmp(ritorno->action,"CHA"))  ritorno->what=nomefile(reqcpy,1) ;
      else ritorno->what=nomefile(reqcpy,0);
    } 
  else ritorno->what=NULL;

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->host=gimme_line(reqcpy,"Host: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->agent=gimme_line(reqcpy,"User-Agent: "); 

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->accept=gimme_line(reqcpy,"Accept: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->lang=gimme_line(reqcpy,"Accept-Language: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->enc=gimme_line(reqcpy,"Accept-Encoding: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->charset=gimme_line(reqcpy,"Accept-Charset: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->keep=gimme_line(reqcpy,"Keep-Alive: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->conn=gimme_line(reqcpy,"Connection: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->referer=gimme_line(reqcpy,"Referer: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->pragma=gimme_line(reqcpy,"Pragma: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->contType=gimme_line(reqcpy,"Content-Type: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->contLength=gimme_line(reqcpy,"Content-Length: ");

  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  strncpy(reqcpy,req,strlen(req));
  ritorno->content=gimme_content(reqcpy);
  for(i=0;i<BUFSIZE;i++) reqcpy[i]='\0';
  return ritorno;
}
