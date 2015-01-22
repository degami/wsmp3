#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "defines.h"

unsigned char ret_buf[BUFSIZE];
FILE *logfile;
int n_errno=-1;
int logflag=0;
int DEBUG=0;
int MYPORT=8000;    /* the port users will be connecting to */
time_t tempo;
int FORKON=8192;
int CHATMODE=0;
char *rootdir=NULL;
int HAPPYMODE=0;
int programWIDE_totalReceived,programWIDE_totalSent; //per implementare status
pid_t pidPassed;
int ls_follow_outside_links=0;
char *DIRS[100]; //directory in cui posso leggere
FILE *usr_cnsl;  //dove vanno stampati i messaggi di DEBUG
char *ACCEPTED_METHODS[]={"GET","CHA","POST","ping-WsMp3?",NULL};
int *sockptr=NULL;
int fd[2];
int sock;
int cwPID;
int cwdupsock;
char *connlist[100]={NULL}; //lista host collegati
int  DAEMON=0; //flag modalità demone
