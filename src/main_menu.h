#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <gtk/gtk.h>
#include "widgets.h"

// Déclarez la fonction on_activate
void on_activate(GtkApplication *app, gpointer user_data);

void create_main_menu(GtkWidget *menu_box, MenuWidgets *widgets);
void on_sous_menu_clicked(GtkWidget *button, MenuWidgets *widgets);

#endif
