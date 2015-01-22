#include <time.h>

char *mytime(struct tm *src);
void nop();
int ISMP3(char *nomefile);
int ISHTM(char *nomefile);
int shallIfork(char* filename);
int is_inside(char *path);

char x2c(char *what);
void unescape_url(char *url);

char* nomefile(char *buf,const int msg);
char* get_op(char *buf);
char* gimme_line(char* buf,char* line);
char* gimme_content(char *buf);
char* get_filedesc(char *filename);


