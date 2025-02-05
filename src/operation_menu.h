#ifndef OPERATION_MENU_H
#define OPERATION_MENU_H
#include "client.h"
#include <gtk/gtk.h>
#include "widgets.h"
#include "outils.h"
#include "operation_pdf.h"
#define FILE_NAME_OPER DATA_DIR "operations.txt"
// Prototypes des fonctions pour gérer les opérations bancaires
void show_depot_form(GtkWidget *form_box);
void show_retrait_form(GtkWidget *form_box);
void show_virement_form(GtkWidget *form_box);
void show_operation_form(GtkWidget *form_box);

#endif // OPERATION_MENU_H
