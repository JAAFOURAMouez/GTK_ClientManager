#ifndef OPERATION_PDF_H
#define OPERATION_PDF_H

#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <time.h>

// Structure pour les données PDF
typedef struct {
    GtkWidget *entry;
    GtkWidget *form_box;
    int client_id;
} PdfData;

void sauvegarder_pdf(GtkWidget *button, gpointer user_data);

#endif