#include "operation_pdf.h"
#include "operation_menu.h"

void sauvegarder_pdf(GtkWidget *button, gpointer user_data) {
    PdfData *pdf_data = (PdfData *)user_data;
    const gchar *code_client_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(pdf_data->entry)));
    int code_client = atoi(code_client_text);
    if (code_client <= 0) {
        afficher_message_erreur("Code client invalide.");
        return;
    }
    
    pdf_data->client_id = code_client;
    // Créer le nom du fichier avec la date
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char filename[256];
    sprintf(filename, "operations_client_%d_%04d%02d%02d.pdf",
            code_client, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    // Initialiser le contexte Cairo pour PDF
    cairo_surface_t *surface = cairo_pdf_surface_create(filename, 595.0, 842.0); // Format A4
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        afficher_message_erreur("Erreur lors de la création de la surface PDF.");
        return;
    }

    cairo_t *cr = cairo_create(surface);
    if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        afficher_message_erreur("Erreur lors de la création du contexte Cairo.");
        cairo_surface_destroy(surface);
        return;
    }

    // Configurer la police et la taille
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    
    // Titre
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, 50, 50);
    char title[100];
    sprintf(title, "Relevé des opérations - Client %d", code_client);
    cairo_show_text(cr, title);

    // En-têtes du tableau
    cairo_set_font_size(cr, 12);
    const char *headers[] = {"Compte", "Opération", "Montant", "Destinataire"};
    double x_positions[] = {50, 150, 300, 400};
    double y = 100;

    for (int i = 0; i < 4; i++) {
        cairo_move_to(cr, x_positions[i], y);
        cairo_show_text(cr, headers[i]);
    }

    // Ligne sous les en-têtes
    cairo_move_to(cr, 50, y + 5);
    cairo_line_to(cr, 545, y + 5);
    cairo_stroke(cr);

    // Lire et écrire les opérations
    y += 30;
    FILE *file = fopen(FILE_NAME_OPER, "r");
    if (file == NULL) {
        printf("Tentative d'ouverture du fichier : %s\n", FILE_NAME_OPER);
        perror("Erreur d'ouverture");  // Ceci affichera la raison précise de l'échec
    }
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            int client_id, compte_id, dest_id;
            char type_operation[50];
            float montant;
            
            int matches = sscanf(line, "code_client: %d code_compte: %d type_operation: %s montant: %f code_client_destinataire: %d",
                               &client_id, &compte_id, type_operation, &montant, &dest_id);
            
            if (client_id == code_client) {
                char montant_str[50];
                sprintf(montant_str, "%.2f", montant);
                
                // Écrire chaque colonne
                char buffer[50];
                
                sprintf(buffer, "%d", compte_id);
                cairo_move_to(cr, x_positions[0], y);
                cairo_show_text(cr, buffer);
                
                cairo_move_to(cr, x_positions[1], y);
                cairo_show_text(cr, type_operation);
                
                cairo_move_to(cr, x_positions[2], y);
                cairo_show_text(cr, montant_str);
                
                cairo_move_to(cr, x_positions[3], y);
                if (matches == 5) {
                    sprintf(buffer, "%d", dest_id);
                    cairo_show_text(cr, buffer);
                } else {
                    cairo_show_text(cr, "-");
                }
                
                y += 20;
                
                // Nouvelle page si nécessaire
                if (y > 800) {
                    cairo_show_page(cr);
                    y = 100;
                }
            }
        }
        fclose(file);
    } else {
        afficher_message_erreur("Erreur d'ouverture du fichier des opérations.");
    }

    // Finaliser le PDF
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    // Afficher un message de confirmation
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                             GTK_DIALOG_MODAL,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "Le PDF a été généré avec succès:\n%s",
                                             filename);
    gtk_window_set_title(GTK_WINDOW(dialog), "Succès");
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
    gtk_widget_show(dialog);
}