#include "req_descriptor.h"

void read_mp3_dir(req_descriptor *reqd,int *sock);
void read_mp3_file(req_descriptor *reqd,int *sock);
void read_html_file(req_descriptor *reqd,int *sock);
void dread_html_file(FILE *filedes,int *sock);
void read_any_file(req_descriptor *reqd,int *sock);
int read_file(req_descriptor *reqd,int *sock);
int dread_file(FILE *filedes,char *filetype,int *sock);
void send_dir_list(int sock, req_descriptor *reqd);
int needHeader(req_descriptor *reqd);
int mp3_player(req_descriptor *desc);
char* write_error_msg(req_descriptor *reqd,char *msg);

