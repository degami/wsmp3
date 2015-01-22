#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFSIZE 32768

int main(int argc, char *argv[])
{
  //FILE *fifo;
  int i,fifo;
  char buffer[BUFSIZE];
  fifo=-1;
  while(fifo==-1){
    fifo=open("/home/mirko/Documents/miei/c/WsMp3/MyFifo",O_NONBLOCK);
  }
  i=0;
  while(1)
    {
      
      while(i==0){ i=read(fifo,buffer,BUFSIZE); printf("i:%d ",i);}
      buffer[i]='\0';
      fprintf(stdout,"%s",buffer);
      printf(">");fgets(buffer,BUFSIZE,stdin);
      if(buffer[strlen(buffer)-1]=='\n') buffer[strlen(buffer)-1]='\0';
      write(fifo,buffer,BUFSIZE);
      printf("...");
      for(i=0;i<BUFSIZE;i++) buffer[i]='\0';
    }
  return 0;
}
