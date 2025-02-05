#ifndef COMPTE_MENU_H
#define COMPTE_MENU_H
#include <gtk/gtk.h>
#include "outils.h"
#include "widgets.h"
#include "compte.h"

void on_delete_account_clicked(GtkWidget *button, gpointer user_data);
void show_delete_account_form(GtkWidget *form_box);
void show_add_account_form(GtkWidget *form_box);
void show_view_account_form(GtkWidget *form_box);
void on_add_account_clicked(GtkWidget *button, gpointer user_data);
void on_search_account_clicked(GtkWidget *button, gpointer user_data);
void show_display_comptes_form(GtkWidget *form_box);
#endif
