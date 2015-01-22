#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define BUFSIZE 32768
#define VERSION "v.0.0.10"
#define AUTHOR "Written and copyrights by Mirko De Grandis."
#define KILO 1024
#define MEGA (1024*1024)
#define GIGA (1024*1024*1024)

#define PDEBUG 0
#define HAPPYHEAD  "HTTP/1.1 200 OK\nConnection: close\nContent/Type: text/html\n\n"
#define HAPPYMSG  "<HTML><HEAD><TITLE>BE HAPPY</TITLE></HEAD><BODY bgcolor=#deeae4><FONT size=+8 color=#ef280e><U><B>Alert!! Alert!!</B></U></FONT><FONT size=+4><BR>&nbsp;<BR>&nbsp;<BR><B>WINDOZ DETECTED!!!!!!</B><BR>this is not for you kid! :)</FONT></BODY></HTML>\n"

extern unsigned char ret_buf[BUFSIZE];
extern FILE *logfile;
extern int n_errno;
extern int logflag;
extern int DEBUG;
extern int MYPORT;
extern time_t tempo;
extern int FORKON;
extern int CHATMODE;
extern char *rootdir;
extern int HAPPYMODE;
extern int programWIDE_totalReceived,programWIDE_totalSent;
extern pid_t pidPassed;
extern int ls_follow_outside_links;
extern char *DIRS[100];
extern FILE *usr_cnsl;
extern char *ACCEPTED_METHODS[];
extern int *sockptr;
extern int fd[2];
extern int sock;
extern pid_t cwPID;
extern int cwdupsock;
extern char *connlist[];
extern int DAEMON;

extern const int file_jpg_bufsize;
extern const int directory_jpg_bufsize;
extern const int styles_css_bufsize;

extern unsigned char file_jpg[];
extern unsigned char directory_jpg[];
extern unsigned char styles_css[];

