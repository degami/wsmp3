typedef struct{
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

req_descriptor *conn_req;
