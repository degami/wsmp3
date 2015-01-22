#include <gtk/gtk.h>


void
on_file1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_esci1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_entry1_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_btn_invia_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_WsMp3_ChatWin_destroy               (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_WsMp3_ChatWin_destroy_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_connectto1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_b_rimuovi_ip_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_aggiungi_ip_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_entry2_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_b_diag_ok_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_sel_all_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_unsel_all_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_aggiungi_ip_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_b_rimuovi_ip_clicked                (GtkButton       *button,
                                        gpointer         user_data);
