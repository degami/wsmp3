#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "interface.h"
#include "support.h"
#include "defines.h"
#include "cw_glob_var.h"
#include "handlers.h"


void daicheesso(int signal)
{
  kill(getppid(),SIGUSR1);
  exit(0);
}

int creat_win (int argc, char*argv[])
{
  //  GtkWidget *WsMp3_ChatWin;
  int i;
  GtkWidget *list_item;
  GList *dlist;
  gtk_set_locale ();
  gtk_init (&argc, &argv);

  //  add_pixmap_directory (PACKAGE_DATA_DIR "/pixmaps");
  //  add_pixmap_directory (PACKAGE_SOURCE_DIR "/pixmaps");

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  WsMp3_ChatWin = create_WsMp3_ChatWin ();
  signal(SIGUSR1,serverSentmsg_handler);

  i=0;
  dlist=NULL;
  while(connlist[i]!=NULL)
    {
      list_item=gtk_list_item_new_with_label(connlist[i]);
      dlist=g_list_prepend(dlist, list_item);
      gtk_widget_show(list_item);
      gtk_list_append_items( GTK_LIST(list1),dlist);
       gtk_object_set_data(GTK_OBJECT(list_item),
                           "list_item_data",
			   "");
      i++;
    }

  gtk_widget_show (WsMp3_ChatWin);
  gtk_main ();
  return 1;
}

void add_text(const char *testo,GdkColor *color)
{
  char buf[1025];
  //GdkFont *font;
  // font= gdk_font_load ("-misc-fixed-medium-r-*-*-*-140-*-*-*-*-*-*");
  sprintf(buf,"%s\n\n",testo);
  if(PDEBUG) printf("testo: %s\n",testo);
  gtk_text_insert(GTK_TEXT(text1),NULL,color,NULL,buf,-1);
  if(PDEBUG) printf("ok....\n");
}

void updatelist()
{
  int i;
  GtkWidget *list_item;
  GList *dlist;

  i=0;
  dlist=NULL;
  while(connlist[i]!=NULL)
    {
      list_item=gtk_list_item_new_with_label(connlist[i]);
      dlist=g_list_prepend(dlist, list_item);
      gtk_widget_show(list_item);
      gtk_list_remove_items( GTK_LIST(list1),dlist);
      i++;
    }  

  i=0;
  dlist=NULL;
  while(connlist[i]!=NULL)
    {
      list_item=gtk_list_item_new_with_label(connlist[i]);
      dlist=g_list_prepend(dlist, list_item);
      gtk_widget_show(list_item);
      gtk_list_append_items( GTK_LIST(list1),dlist);
      gtk_object_set_data(GTK_OBJECT(list_item),
			  "list_item_data",
			  "");
      i++;
    }
}
