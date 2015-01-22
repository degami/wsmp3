/*typedef struct{
  char* action;          //what the client wants
  char* what;            //what the action interest
  char* host;            //Host line
  char* agent;           //the client user-agent
  char* accept;          //accept line
  char* lang;            //accept-language line
  char* enc;             //accept-encoding line
  char* charset;         //accept-charset line
  char* keep;            //keep-alive line;
  char* conn;            //connection line;
  char* referer;         //referer page
  char* pragma;          //pragma line
  char* contType;        //content-type line   (se il browser invia qualche info)
  char* contLength;      //content-lenght line (idem)
  char* content;         //quello che viene inviato
}req_descriptor;
now it's in req_descriptor_struct.h */

#include "req_descriptor_struct.h"

req_descriptor* parse_request(char *req); 
req_descriptor* cpy_req_descriptor(req_descriptor *desc);
void rem_req_descriptor(req_descriptor *desc);
void reinit_req_descriptor(req_descriptor *desc);
char* print_req_descriptor(req_descriptor *desc);
