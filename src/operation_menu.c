#include "operation_menu.h"
#include <gtk/gtk.h>

void enregistrer_operation(int code_client, int code_compte, const char *type_operation, float montant, int code_client_dest) {
    FILE *file = fopen(FILE_NAME_OPER, "a");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier operations.txt");
        return;
    }
    if (strcmp(type_operation, "Virement") == 0) {
        fprintf(file, "code_client: %d code_compte: %d type_operation: %s montant: %.2f code_compte_destinataire: %d\n",
                code_client, code_compte, type_operation, montant, code_client_dest);
    } else {
        fprintf(file, "code_client: %d code_compte: %d type_operation: %s montant: %.2f\n",
                code_client, code_compte, type_operation, montant);
    }

    fclose(file);
}


void depot_button_clicked(GtkWidget *button, gpointer data) {
    DepotAccount *widgets = (DepotAccount *)data;

    const gchar *code_compte_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)));
    const gchar *montant_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->montant)));

    int code_compte = atoi(code_compte_text);
    float montant = strtof(montant_text, NULL);

    // Vérifier si le code compte existe
    FILE *file = fopen(FILE_NAME_COMPTE, "r+b");
    if (file == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur : Impossible d'ouvrir le fichier des comptes.");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        return;
    }
    SCompte compte;
    int found = 0;

    // Parcourir le fichier pour trouver le compte
    while (fread(&compte, sizeof(SCompte), 1, file)) {
        if (compte.code_compte == code_compte) {
            found = 1;
            compte.solde += montant;  // Ajouter le montant
            fseek(file, -sizeof(SCompte), SEEK_CUR); // Revenir en arrière
            if (fwrite(&compte, sizeof(SCompte), 1, file)) {
                GtkWidget *dialog = gtk_message_dialog_new(NULL,
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Dépôt réussi : %.2f€ ajoutés au compte %d.",
                    montant, code_compte);
                gtk_window_present(GTK_WINDOW(dialog));
                g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
                enregistrer_operation(compte.code_client, code_compte, "Dépôt", montant, -1);
            } else {
                GtkWidget *dialog = gtk_message_dialog_new(NULL,
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Erreur lors de la mise à jour du compte.");
                gtk_window_present(GTK_WINDOW(dialog));
                g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
            }
            break;
        }
    }

    fclose(file);

    if (!found) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur : Le compte %d n'existe pas.",
            code_compte);
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    }
}
// Fonction pour afficher le formulaire de dépôt
void show_depot_form(GtkWidget *form_box) {
    // Nettoyage de la boîte du formulaire
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Créer la grille pour organiser les widgets
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    // Création d'un label pour le titre
    GtkWidget *label = gtk_label_new("Formulaire de Dépôt");
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(form_box), label);

    // Création des labels et champs de saisie
    GtkWidget *code_label = gtk_label_new("Code compte:");
    GtkWidget *montant_label = gtk_label_new("Montant:");

    GtkWidget *code_entry = gtk_entry_new();
    GtkWidget *montant_entry = gtk_entry_new();

    // Bouton de validation
    GtkWidget *submit_button = gtk_button_new_with_label("Valider");

    // Stocker les widgets dans une structure pour récupérer les valeurs plus tard
    DepotAccount *widgets = g_new(DepotAccount, 1);
    widgets->code_entry = code_entry;
    widgets->montant = montant_entry;

    // Ajouter les widgets à la grille
    gtk_grid_attach(GTK_GRID(grid), code_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), montant_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), montant_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 2, 2, 1);

    // Ajouter la grille au form_box
    gtk_box_append(GTK_BOX(form_box), grid);

    // Connecter le bouton de validation à une fonction callback
    g_signal_connect(submit_button, "clicked", G_CALLBACK(depot_button_clicked), widgets);
}

void retrait_button_clicked(GtkWidget *button, gpointer data) {
    DepotAccount *widgets = (DepotAccount *)data;

    const gchar *code_compte_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)));
    const gchar *montant_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->montant)));

    int code_compte = atoi(code_compte_text);
    float montant = strtof(montant_text, NULL);

    // Vérifier si le fichier s'ouvre correctement
    FILE *file = fopen(FILE_NAME_COMPTE, "r+b");
    if (file == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur : Impossible d'ouvrir le fichier des comptes.");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        return;
    }

    SCompte compte;
    int found = 0;

    // Recherche du compte dans le fichier
    while (fread(&compte, sizeof(SCompte), 1, file)) {
        if (compte.code_compte == code_compte) {
            found = 1;
            // Vérifier si le solde est suffisant
            if (compte.solde < montant) {
                GtkWidget *dialog = gtk_message_dialog_new(NULL,
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_WARNING,
                    GTK_BUTTONS_OK,
                    "Solde insuffisant pour le retrait de %.2f€ du compte %d.",
                    montant, code_compte);
                gtk_window_present(GTK_WINDOW(dialog));
                g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
                fclose(file);
                return;
            }

            // Effectuer le retrait
            compte.solde -= montant;
            fseek(file, -sizeof(SCompte), SEEK_CUR);
            if (fwrite(&compte, sizeof(SCompte), 1, file)) {
                GtkWidget *dialog = gtk_message_dialog_new(NULL,
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Retrait de %.2f€ effectué avec succès du compte %d.",
                    montant, code_compte);
                gtk_window_present(GTK_WINDOW(dialog));
                g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
                enregistrer_operation(compte.code_client, code_compte, "Retrait", montant, -1);

            } else {
                GtkWidget *dialog = gtk_message_dialog_new(NULL,
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Erreur lors de la mise à jour du compte.");
                gtk_window_present(GTK_WINDOW(dialog));
                g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
            }
            break;
        }
    }

    fclose(file);

    if (!found) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur : Le compte %d n'existe pas.",
            code_compte);
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    }
}
// Fonction pour afficher le formulaire de retrait
void show_retrait_form(GtkWidget *form_box) {
    // Nettoyage du form_box
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Création de la grille pour le formulaire
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    // Titre du formulaire
    GtkWidget *label = gtk_label_new("Formulaire de Retrait");
    gtk_box_append(GTK_BOX(form_box), label);

    // Labels pour les champs
    GtkWidget *code_label = gtk_label_new("Code compte:");
    GtkWidget *montant_label = gtk_label_new("Montant:");

    // Champs de saisie
    GtkWidget *code_entry = gtk_entry_new();
    GtkWidget *montant_entry = gtk_entry_new();
    gtk_entry_set_input_purpose(GTK_ENTRY(montant_entry), GTK_INPUT_PURPOSE_NUMBER);

    // Bouton de validation
    GtkWidget *submit_button = gtk_button_new_with_label("Valider");

    // Stocker les widgets dans une structure
    DepotAccount *widgets = g_new(DepotAccount, 1);
    widgets->code_entry = code_entry;
    widgets->montant = montant_entry;

    // Associer le bouton à la fonction de retrait
    g_signal_connect(submit_button, "clicked", G_CALLBACK(retrait_button_clicked), widgets);

    // Ajouter les widgets à la grille
    gtk_grid_attach(GTK_GRID(grid), code_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), montant_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), montant_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 2, 2, 1);

    // Ajouter la grille au formulaire
    gtk_box_append(GTK_BOX(form_box), grid);
}

void virement_button_clicked(GtkWidget *button, gpointer data) {
    VirementAccount *widgets = (VirementAccount *)data;

    const gchar *code_source_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->code_source_entry)));
    const gchar *code_dest_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->code_dest_entry)));
    const gchar *montant_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->montant_entry)));

    int code_source = atoi(code_source_text);
    int code_dest = atoi(code_dest_text);
    float montant = strtof(montant_text, NULL);

    if (code_source == code_dest) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Le compte source et le compte destinataire doivent être différents.");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        return;
    }

    FILE *file = fopen(FILE_NAME_COMPTE, "r+b");
    if (file == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur : Impossible d'ouvrir le fichier des comptes.");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        return;
    }

    SCompte compte_source, compte_dest;
    int source_found = 0, dest_found = 0;
    long source_pos = 0, dest_pos = 0;

    SCompte compte_temp;
    while (fread(&compte_temp, sizeof(SCompte), 1, file)) {
        if (compte_temp.code_compte == code_source) {
            source_found = 1;
            source_pos = ftell(file) - sizeof(SCompte);
            compte_source = compte_temp; // Copier les bonnes données
        } else if (compte_temp.code_compte == code_dest) {
            dest_found = 1;
            dest_pos = ftell(file) - sizeof(SCompte);
            compte_dest = compte_temp; // Copier les bonnes données
        }

        if (source_found && dest_found) break;
    }
    if (!source_found || !dest_found) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "L'un des comptes n'existe pas.");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        fclose(file);
        return;
    }

    // Vérifier le solde du compte source
    if (compte_source.solde < montant) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Solde insuffisant pour effectuer le virement.");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        fclose(file);
        return;
    }

    // Effectuer le virement
    compte_source.solde -= montant;
    compte_dest.solde += montant;

    // Mettre à jour les comptes dans le fichier
    fseek(file, source_pos, SEEK_SET);
    fwrite(&compte_source, sizeof(SCompte), 1, file);

    fseek(file, dest_pos, SEEK_SET);
    fwrite(&compte_dest, sizeof(SCompte), 1, file);

    fclose(file);

    // Confirmation de l'opération
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Virement de %.2f€ du compte %d au compte %d effectué avec succès.",
        montant, code_source, code_dest);
    gtk_window_present(GTK_WINDOW(dialog));
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    enregistrer_operation(compte_source.code_client, code_source, "Virement", montant, code_dest);
}
// Fonction pour afficher le formulaire de virement
void show_virement_form(GtkWidget *form_box) {
    // Nettoyage du formulaire
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Création d'une grille pour organiser les champs
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    // Titre du formulaire
    GtkWidget *label = gtk_label_new("Formulaire de Virement");
    gtk_box_append(GTK_BOX(form_box), label);

    // Labels pour les champs
    GtkWidget *source_label = gtk_label_new("Code compte source:");
    GtkWidget *dest_label = gtk_label_new("Code compte destinataire:");
    GtkWidget *montant_label = gtk_label_new("Montant:");

    // Champs de saisie
    GtkWidget *code_source_entry = gtk_entry_new();
    GtkWidget *code_dest_entry = gtk_entry_new();
    GtkWidget *montant_entry = gtk_entry_new();
    gtk_entry_set_input_purpose(GTK_ENTRY(montant_entry), GTK_INPUT_PURPOSE_NUMBER);

    // Bouton de validation
    GtkWidget *submit_button = gtk_button_new_with_label("Effectuer le Virement");

    // Stocker les widgets dans une structure
    VirementAccount *widgets = g_new(VirementAccount, 1);
    widgets->code_source_entry = code_source_entry;
    widgets->code_dest_entry = code_dest_entry;
    widgets->montant_entry = montant_entry;

    // Associer le bouton à la fonction de virement
    g_signal_connect(submit_button, "clicked", G_CALLBACK(virement_button_clicked), widgets);

    // Ajouter les widgets à la grille
    gtk_grid_attach(GTK_GRID(grid), source_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_source_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), dest_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_dest_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), montant_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), montant_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 3, 2, 1);

    // Ajouter la grille au formulaire
    gtk_box_append(GTK_BOX(form_box), grid);
}
void afficher_message_erreur(const gchar *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        NULL,                       // Aucune fenêtre parente
        GTK_DIALOG_MODAL,            // Modal : l'utilisateur doit fermer le dialogue pour continuer
        GTK_MESSAGE_ERROR,           // Type d'icône (ici, une icône d'erreur)
        GTK_BUTTONS_OK,              // Le bouton OK pour fermer la boîte
        "%s",                        // Le message à afficher
        message                      // Message passé en argument
    );

    // Afficher la boîte de dialogue
    gtk_window_present(GTK_WINDOW(dialog));

    // Connecter le signal pour fermer la boîte de dialogue lorsqu'on clique sur le bouton OK
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
}



void rechercher_operations_par_client(GtkWidget *button, gpointer user_data) {
    CallbackData *data = (CallbackData *)user_data;
    const gchar *code_client_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(data->entry)));
    int code_client = atoi(code_client_text);
    GtkWidget *form_box = data->form_box;

    // Si le code client est invalide
    if (code_client <= 0) {
        afficher_message_erreur("Code client invalide.");
        return;
    }

    // Nettoyer l'affichage précédent (garder seulement la zone de recherche)
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    GtkWidget *search_grid = child;  // Sauvegarder la grille de recherche
    while ((child = gtk_widget_get_last_child(form_box))) {
        if (child != search_grid) {
            gtk_widget_unparent(child);
        } else {
            break;
        }
    }


    // Ouvrir le fichier operations.txt
    FILE *file = fopen(FILE_NAME_OPER, "r");
    if (file == NULL) {
        afficher_message_erreur("Erreur d'ouverture du fichier des opérations.");
        return;
    }

    // Créer le widget pour afficher les résultats
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 300);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    // En-têtes
    const char *headers[] = {"Compte", "Opération", "Montant", "Destinataire"};
    for (int i = 0; i < 4; i++) {
        GtkWidget *header = gtk_label_new(headers[i]);
        gtk_widget_add_css_class(header, "heading");
        gtk_grid_attach(GTK_GRID(grid), header, i, 0, 1, 1);
    }

    // Lire et afficher les opérations
    char line[256];
    int row = 1;
    while (fgets(line, sizeof(line), file)) {
        int client_id, compte_id, dest_id;
        char type_operation[50];
        float montant;
        // Essayer de lire avec un destinataire (pour les virements)
        int matches = sscanf(line, "code_client: %d code_compte: %d type_operation: %s montant: %f code_client_destinataire: %d",
                           &client_id, &compte_id, type_operation, &montant, &dest_id);
        
        if (client_id == code_client) {
            char montant_str[50];
            sprintf(montant_str, "%.2f", montant);
            
            // Créer les labels
            GtkWidget *compte_label = gtk_label_new(g_strdup_printf("%d", compte_id));
            GtkWidget *operation_label = gtk_label_new(type_operation);
            GtkWidget *montant_label = gtk_label_new(montant_str);
            
            const char *destinataire_text = (matches == 5) ? g_strdup_printf("%d", dest_id) : "-";
            GtkWidget *destinataire_label = gtk_label_new(destinataire_text);

            // Ajouter à la grille
            gtk_grid_attach(GTK_GRID(grid), compte_label, 0, row, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), operation_label, 1, row, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), montant_label, 2, row, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), destinataire_label, 3, row, 1, 1);
            
            row++;
        }
    }
    
    fclose(file);

    // Ajouter la grille à la fenêtre scrollable
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), grid);
    gtk_box_append(GTK_BOX(form_box), scrolled_window);
}

void show_operation_form(GtkWidget *form_box) {
    // Nettoyage du formulaire
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Créer la grille pour le formulaire de recherche
    GtkWidget *search_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(search_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(search_grid), 10);

    // Champ pour entrer le code client
    GtkWidget *label_code = gtk_label_new("Code client:");
    gtk_grid_attach(GTK_GRID(search_grid), label_code, 0, 0, 1, 1);
    
    // Créer et configurer l'entrée
    GtkWidget *entry_code = gtk_entry_new();
    // Définir une taille minimale
    gtk_widget_set_size_request(entry_code, 200, -1);  // Largeur de 200 pixels, hauteur inchangée
    // Rendre l'entrée extensible horizontalement
    gtk_widget_set_hexpand(entry_code, TRUE);
    
    gtk_grid_attach(GTK_GRID(search_grid), entry_code, 1, 0, 2, 1);
    
    GtkWidget *valider_button = gtk_button_new_with_label("Valider");
    gtk_grid_attach(GTK_GRID(search_grid), valider_button, 3, 0, 1, 1);

    // Ajouter le bouton PDF
    GtkWidget *pdf_button = gtk_button_new_with_label("Exporter PDF");
    gtk_grid_attach(GTK_GRID(search_grid), pdf_button, 4, 0, 1, 1);

    // Créer la structure pour le PDF
    PdfData *pdf_data = g_new(PdfData, 1);
    pdf_data->entry = entry_code;
    pdf_data->form_box = form_box;
    pdf_data->client_id = 0;

    // Créer et configurer les données du callback principal
    CallbackData *data = g_new(CallbackData, 1);
    data->entry = entry_code;
    data->form_box = form_box;

    // Connecter les signaux
    g_signal_connect_data(valider_button, "clicked", 
                         G_CALLBACK(rechercher_operations_par_client), 
                         data, NULL, 0);

    g_signal_connect_data(pdf_button, "clicked",
                         G_CALLBACK(sauvegarder_pdf),
                         pdf_data, (GClosureNotify)g_free, 0);

    // Rendre la grille extensible horizontalement
    gtk_widget_set_hexpand(search_grid, TRUE);
    
    gtk_box_append(GTK_BOX(form_box), search_grid);
}