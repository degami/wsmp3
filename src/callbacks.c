#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>


#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "cw_glob_var.h"
#include "defines.h"
#include "winfunct.h"

void
on_file1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_esci1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  // _exit(0);
}


void
on_entry1_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
  char givenbuf[ncharsinline+2];
  ncharsinline++;
  if(PDEBUG) printf("entry1 change.nchars:%d\n",ncharsinline);
  strncpy(givenbuf,gtk_editable_get_chars(GTK_EDITABLE(entry1),0,ncharsinline),ncharsinline);
  givenbuf[ncharsinline]='\0';
  ncharsinline=strlen(givenbuf);
  if(PDEBUG) printf("strlen(buf):%d\nbuf:[%s]\n",strlen(givenbuf),givenbuf);
  if(PDEBUG) printf("buf[strlen(buf)-1(%d)]=[%c]\n",strlen(givenbuf)-1,givenbuf[strlen(givenbuf)-1]);
  /* if(givenbuf[strlen(givenbuf)]=='\n');
  {
    ncharsinline=0;
    if(PDEBUG) printf("trovato un '\\n' buf:%s\n",givenbuf);
    write(fd[PIPE_WRITE],givenbuf,strlen(givenbuf));
    // kill(getppid(),SIGUSR2);
    add_text(givenbuf);
    gtk_editable_delete_text(GTK_EDITABLE(entry1),0,-1);
    }*/
}


void
on_btn_invia_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  int i;
  GdkColor color;
  GdkColormap *cmap;
  GtkObject *list_item;
  GList   *dlist;
  char buf[ncharsinline+100];
  strncpy(buf,gtk_editable_get_chars(GTK_EDITABLE(entry1),0,ncharsinline),ncharsinline);
  buf[ncharsinline]='\0';
  
  dlist=GTK_LIST(list1)->selection;
  if (!dlist) {
    if(PDEBUG) printf("nessuna selezione");
    return;
  }
  
  /* Get the system color map and allocate the color red */
  cmap = gdk_colormap_get_system();
  color.red = 0xffff;
  color.green = 0;
  color.blue = 0;
  if (!gdk_color_alloc(cmap, &color)) {
    g_error("couldn't allocate color");
  }
  add_text(buf,&color);

  while (dlist) 
    {
      for(i=0;i<ncharsinline+100;i++) buf[i]='\0';
      list_item=GTK_OBJECT(dlist->data);
      strncpy(buf,"[",strlen("["));
      strcat(buf,connlist[gtk_list_child_position (GTK_LIST(list1),GTK_WIDGET(list_item))]);
      strcat(buf,"]");
      strcat(buf,getenv("USER"));
      strcat(buf," - ");   
      strcat(buf,gtk_editable_get_chars(GTK_EDITABLE(entry1),0,ncharsinline));
      if(PDEBUG) printf("scrivo:%s\n",buf);
      write(fd[PIPE_WRITE],buf,strlen(buf));
      if(PDEBUG) printf("invio SIGUSR2 a pid: %d\n",getppid());
      kill(getppid(),SIGUSR2);
      dlist=dlist->next;
    }

  ncharsinline=0;
  gtk_editable_delete_text(GTK_EDITABLE(entry1),0,-1);
}


void
on_WsMp3_ChatWin_destroy               (GtkObject       *object,
                                        gpointer         user_data)
{
  //  gtk_widget_hide(WsMp3_ChatWin);
}


gboolean
on_WsMp3_ChatWin_destroy_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_connectto1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  dialog1=create_dialog1();
  gtk_widget_show (dialog1);
}


void
on_b_rimuovi_ip_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  int i;
  GtkObject *list_item;
  GList   *dlist;

  dlist=GTK_LIST(list1)->selection;
  if (!dlist) {
    if(PDEBUG) printf("nessuna selezione");
    return;
  }
  while (dlist) 
    {
      if(PDEBUG) printf("parte while..\n");
      list_item=GTK_OBJECT(dlist->data);
      i=gtk_list_child_position (GTK_LIST(list1),GTK_WIDGET(list_item));
      if(PDEBUG) printf("posizione:%d\n",i);
      if(i==-1) return;
      gtk_list_clear_items(GTK_LIST(list1),i,i+1);
      while(connlist[i]!=NULL)
	{
	  free(connlist[i]);
	  if(connlist[i+1]!=NULL)
	    {
	      connlist[i]=(char *)malloc(80); 
	      strncpy(connlist[i],connlist[i+1],80);
	      (connlist[i])[79]='\0';
	    }
	  else {connlist[i]=NULL; break;}
	  i++;
	}
      if(PDEBUG) printf("connlist:\n");
      if(PDEBUG){i=0; while(connlist[i]!=NULL){printf("%d:[%s]\n",i,connlist[i]);i++;}}
      if(PDEBUG) printf("---------\n");
      dlist=dlist->next;
      //gtk_widget_show(list1);
    }
}


void
on_b_aggiungi_ip_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
  dialog1=create_dialog1();
  gtk_widget_show (dialog1);
}


void
on_entry2_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
  char givenbuf[ncharsindialogline+2];
  ncharsindialogline++;
  if(PDEBUG) printf("entry2 change.nchars:%d\n",ncharsindialogline);
  strncpy(givenbuf,gtk_editable_get_chars(GTK_EDITABLE(entry2),0,ncharsindialogline),ncharsindialogline);
  givenbuf[ncharsindialogline]='\0';
  ncharsindialogline=strlen(givenbuf);
  if(PDEBUG) printf("strlen(buf):%d\nbuf:[%s]\n",strlen(givenbuf),givenbuf);
  if(PDEBUG) printf("buf[strlen(buf)-1(%d)]=[%c]\n",strlen(givenbuf)-1,givenbuf[strlen(givenbuf)-1]);
}


void
on_b_diag_ok_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  int i,j;
  char ipaddr[80];
  GtkWidget *list_item;
  GList *dlist;
  
  //aggiungi ip alla lista
  if(PDEBUG) printf("devo aggiungere.\n");
  if(ncharsindialogline>79) ncharsindialogline=79;
  if(ncharsindialogline!=0)
    {
      for(i=0;i<80;i++) ipaddr[i]='\0';
      strncpy(ipaddr,gtk_editable_get_chars(GTK_EDITABLE(entry2),0,ncharsindialogline),ncharsindialogline);
      
      i=0;
      while(connlist[i]!=NULL)
	{
	  if(!strcmp(connlist[i],ipaddr)) return;
	  i++;
	}
      
      i=0;
      if(connlist[i]==NULL)
	{ 
	  connlist[i]=(char *) malloc(80);
	  for(j=0;j<80;j++) (connlist[i])[j]='\0';
	  for(j=0;j<strlen(ipaddr);j++) 
	    if((ipaddr[j]>='0' && ipaddr[j]<='9')
	       ||
	       ((ipaddr[j]=='.'||ipaddr[j]==':')||ipaddr[j]=='\0'))
	      { 
		(connlist[i])[j]=ipaddr[j];
		if((connlist[i])[j]=='\0') break;
	      }
	  i++;
	  connlist[i]=NULL;
	}
      else
	{ 
	  while(connlist[i]!=NULL) i++;
	  connlist[i]=(char *) malloc(80);
	  for(j=0;j<80;j++) (connlist[i])[j]='\0';
	  for(j=0;j<strlen(ipaddr);j++) 
	    if((ipaddr[j]>='0' && ipaddr[j]<='9')
	       ||
	       ((ipaddr[j]=='.'||ipaddr[j]==':')||ipaddr[j]=='\0'))
	      { 
		(connlist[i])[j]=ipaddr[j];
		if((connlist[i])[j]=='\0') break;
	      }
	  i++;
	  connlist[i]=NULL;
	}
      
      dlist=NULL;
      list_item=gtk_list_item_new_with_label(ipaddr);
      dlist=g_list_prepend(dlist, list_item);
      gtk_widget_show(list_item);
      gtk_list_append_items( GTK_LIST(list1),dlist);
      gtk_object_set_data(GTK_OBJECT(list_item),
			  "list_item_data",
			  "");
      
      ncharsindialogline=0;
      gtk_editable_delete_text(GTK_EDITABLE(entry2),0,-1);
    }
  gtk_widget_hide (dialog1);
  dialog1=NULL;
}


void
on_b_sel_all_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_list_select_all(GTK_LIST(list1));
}


void
on_b_unsel_all_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_list_unselect_all(GTK_LIST(list1));
}


