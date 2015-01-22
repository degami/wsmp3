#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
  FILE *f,*output;
  char buffer[1024],buf2[100],arrname[50],*cp;
  unsigned char ch;
  int i,j,firsttime=1;
  int arraysize=0;

  if(argc<=1)
    {
      printf("Usage: %s <imagefile#1> ... <imagefile#N>\n",argv[0]);
      exit(0);
    }
  else
    {
      for(i=1;i<argc;i++)
	{
	  for(j=0;j<1024;j++) buffer[j]='\0';
	  f=fopen(argv[i],"r");
	  if(f!=NULL)
	    {
	      sprintf(buffer,"%s.c",argv[i]);
	      output=fopen(buffer,"w+");
	      if(output==NULL)
		{
		  printf("Error in opening output file\n");
		  exit(0);
		}
	      else
		{
		  strncpy(arrname,argv[i],50);
		  arrname[49]='\0';
		  cp=arrname;
		  while(*cp!='\0')
		    {
		      if(*cp=='.') *cp='_';
		      cp++;
		    }
		  sprintf(buffer,"unsigned char %s[]={ ",arrname);
		  
		  fwrite(buffer,strlen(buffer),1,output);
                  firsttime=1;
		  arraysize=0;
		  while(!feof(f))
		    {
		      if(!firsttime) fwrite(", ",2,1,output);
		      else firsttime=0;

		      fread(&ch,1,1,f);
		      sprintf(buf2,"0x%x",ch);
		      fwrite(buf2,strlen(buf2),1,output);
		      arraysize++;
		    }
		  fwrite(" };\n",strlen(" };\n"),1,output);
		  sprintf(buffer,"\nconst int %s_bufsize=%d;\n",arrname,arraysize);
		  fwrite(buffer,strlen(buffer),1,output);
		  fclose(output);
		}
	      fclose(f);
	    }
	} 
    }
  return 0;
}
