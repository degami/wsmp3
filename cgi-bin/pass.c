#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main(int argc,char *argv[])
{
  char *nome;
  char *pass;
  nome=strstr(argv[1],"nome=")+strlen("nome=");
  pass=strstr(argv[1],"&");
  *pass='\0';
  pass+=8;
  fprintf(stdout,"<HTML><HEAD><TITLE>pass</TITLE></HEAD>");
  fprintf(stdout,"<BODY>nome: %s<BR>your pass has been seen: <B>%s</B></BODY></HTML>\n",nome,pass);
}
