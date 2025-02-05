#include "comtpe_menu.h"

void show_add_account_form(GtkWidget *form_box) {
    // Nettoyer le form_box
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Créer la grille pour le formulaire
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    // Labels
    GtkWidget *code_compte_label = gtk_label_new("Code Compte:");
    GtkWidget *code_client_label = gtk_label_new("Code Client:");
    GtkWidget *date_label = gtk_label_new("Date de création:");
    
    // Champs de saisie
    GtkWidget *code_compte_entry = gtk_entry_new();
    GtkWidget *code_client_entry = gtk_entry_new();
    GtkWidget *jour_entry = gtk_entry_new();
    GtkWidget *mois_entry = gtk_entry_new();
    GtkWidget *annee_entry = gtk_entry_new();

    // Définir la largeur maximale des champs de date
    gtk_widget_set_size_request(jour_entry, 30, -1);
    gtk_widget_set_size_request(mois_entry, 30, -1);
    gtk_widget_set_size_request(annee_entry, 60, -1);


    // Box pour regrouper les champs de date
    GtkWidget *date_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(date_box), jour_entry);
    gtk_box_append(GTK_BOX(date_box), gtk_label_new("/"));
    gtk_box_append(GTK_BOX(date_box), mois_entry);
    gtk_box_append(GTK_BOX(date_box), gtk_label_new("/"));
    gtk_box_append(GTK_BOX(date_box), annee_entry);

    // Bouton de validation
    GtkWidget *submit_button = gtk_button_new_with_label("Ajouter");

    // Stocker les widgets dans une structure
    AccountWidgets *widgets = g_new(AccountWidgets, 1);
    widgets->code_compte_entry = code_compte_entry;
    widgets->code_client_entry = code_client_entry;
    widgets->jour_entry = jour_entry;
    widgets->mois_entry = mois_entry;
    widgets->annee_entry = annee_entry;
    widgets->form_box = form_box;

    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_add_account_clicked), widgets);

    // Placement dans la grille
    gtk_grid_attach(GTK_GRID(grid), code_compte_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_compte_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_client_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_client_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_box, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 3, 2, 1);

    gtk_box_append(GTK_BOX(form_box), grid);
}

void on_add_account_clicked(GtkWidget *button, gpointer user_data) {
    AccountWidgets *widgets = (AccountWidgets *)user_data;
    SCompte compte;
    
    // Récupérer les valeurs
    const char *code_compte_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->code_compte_entry)));
    const char *code_client_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->code_client_entry)));
    const char *jour_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->jour_entry)));
    const char *mois_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->mois_entry)));
    const char *annee_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(widgets->annee_entry)));
    
    compte.code_compte = atoi(code_compte_text);
    compte.code_client = atoi(code_client_text);
    compte.date_compte.jour = atoi(jour_text);
    compte.date_compte.mois = atoi(mois_text);
    compte.date_compte.annee = atoi(annee_text);
    
    // Vérifier si le code client existe
    if (!code_client_existe(compte.code_client)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Le code client %d n'existe pas",
            compte.code_client);
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        return;
    }
    
    // Vérifier la validité de la date
    if (!date_valide(compte.date_compte.jour, compte.date_compte.mois, compte.date_compte.annee)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Date invalide");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        return;
    }
    compte.solde=0;
    // Ajouter le compte dans le fichier
    FILE *file = fopen(FILE_NAME_COMPTE, "a");
    if (file == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur lors de l'ouverture du fichier compte");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        return;
    }

    if (fwrite(&compte, sizeof(SCompte), 1, file)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Compte ajouté avec succès");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        
        // Vider les champs
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_compte_entry)), "", -1);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_client_entry)), "", -1);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->jour_entry)), "", -1);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->mois_entry)), "", -1);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->annee_entry)), "", -1);
    }
    fclose(file);
}

void show_view_account_form(GtkWidget *form_box) {
    // Nettoyer le form_box
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

    // Éléments de recherche
    GtkWidget *code_label = gtk_label_new("Code Compte:");
    GtkWidget *code_entry = gtk_entry_new();
    GtkWidget *search_button = gtk_button_new_with_label("Rechercher");

    // Zone de résultat
    GtkWidget *result_label = gtk_label_new("");
    gtk_label_set_wrap(GTK_LABEL(result_label), TRUE);
    gtk_widget_set_hexpand(result_label, TRUE);

    // Connecter le signal de recherche
    SearchAccountData *search_data = g_new(SearchAccountData, 1);
    search_data->code_entry = code_entry;
    search_data->result_label = result_label;
    
    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_account_clicked), search_data);

    // Placement dans la grille
    gtk_grid_attach(GTK_GRID(search_grid), code_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), code_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), search_button, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), result_label, 0, 1, 3, 1);

    gtk_box_append(GTK_BOX(form_box), search_grid);
}

void on_search_account_clicked(GtkWidget *button, gpointer user_data) {
    SearchAccountData *data = (SearchAccountData *)user_data;
    
    const char *code_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(data->code_entry)));
    int code_compte = atoi(code_text);
    
    FILE *file = fopen(FILE_NAME_COMPTE, "r");
    if (file == NULL) {
        gtk_label_set_text(GTK_LABEL(data->result_label), 
            "Erreur lors de l'ouverture du fichier compte");
        return;
    }

    SCompte compte;
    int trouve = 0;
    
    while (fread(&compte, sizeof(SCompte), 1, file)) {
        if (compte.code_compte == code_compte) {
            char result_text[512];
            snprintf(result_text, sizeof(result_text),
                "Code compte: %d\nCode client: %d\nDate création: %d/%d/%d",
                compte.code_compte, compte.code_client,
                compte.date_compte.jour, compte.date_compte.mois, compte.date_compte.annee);
            
            gtk_label_set_text(GTK_LABEL(data->result_label), result_text);
            trouve = 1;
            break;
        }
    }
    
    fclose(file);
    
    if (!trouve) {
        gtk_label_set_text(GTK_LABEL(data->result_label), 
            "Aucun compte trouvé avec ce code");
    }
}

void show_delete_account_form(GtkWidget *form_box) {
    // Nettoyer le form_box
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Création du formulaire de suppression
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    GtkWidget *code_label = gtk_label_new("Code à supprimer:");
    GtkWidget *code_entry = gtk_entry_new();
    GtkWidget *delete_button = gtk_button_new_with_label("Supprimer");

    // Stocker les widgets dans une structure
    DeleteClientWidgets *widgets = g_new(DeleteClientWidgets, 1);
    widgets->code_entry = code_entry;
    widgets->form_box = form_box;

    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_account_clicked), widgets);

    // Placement dans la grille
    gtk_grid_attach(GTK_GRID(grid), code_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), delete_button, 0, 1, 2, 1);

    gtk_box_append(GTK_BOX(form_box), grid);
}

void on_delete_account_clicked(GtkWidget *button, gpointer user_data) {
    DeleteClientWidgets *widgets = (DeleteClientWidgets *)user_data;
    SCompte comptes[MAX_COMPTES];
    
    const char *code_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)));
    int code_compte = atoi(code_text);
    
    int nbComptes = lire_comptes(comptes);
    int compte_supprime = 0;
    
    for (int i = 0; i < nbComptes; i++) {
        if (comptes[i].code_compte == code_compte) {
            // Décaler les éléments après le client à supprimer
            for (int j = i; j < nbComptes - 1; j++) {
                comptes[j] = comptes[j + 1];
            }
            nbComptes--;
            compte_supprime = 1;
            break;
        }
    }
    
    if (compte_supprime) {
        FILE *file = fopen(FILE_NAME_COMPTE, "w");
        if (file == NULL) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Erreur lors de l'ouverture du fichier");
            gtk_window_present(GTK_WINDOW(dialog));
            g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
            return;
        }
        
        fwrite(comptes, sizeof(SCompte), nbComptes, file);
        fclose(file);
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Compte supprimé avec succès");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        
        // Réinitialiser le champ de code
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)), "", -1);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Compte avec le code %d introuvable",
            code_compte);
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    }
}

void show_display_comptes_form(GtkWidget *form_box) {
    // Nettoyer le form_box
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    SCompte comptes[MAX_COMPTES];
    int nbComptes = lire_comptes(comptes);

    if (nbComptes == 0) {
        GtkWidget *label = gtk_label_new("Aucun compte trouvé.");
        gtk_box_append(GTK_BOX(form_box), label);
        return;
    }

    // Trier les comptes (à adapter avec une fonction comparer_comptes si nécessaire)
    qsort(comptes, nbComptes, sizeof(SCompte), comparer_comptes);

    // Créer une liste scrollable
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 300);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    // Créer la grille pour afficher les comptes
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    // En-têtes
    GtkWidget *header_code_compte = gtk_label_new("Code Compte");
    GtkWidget *header_code_client = gtk_label_new("Code Client");
    GtkWidget *header_date = gtk_label_new("Date");
    GtkWidget *header_solde = gtk_label_new("Solde");
    // Centrer les en-têtes et ajouter à la grille
    gtk_widget_set_halign(header_code_compte, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(header_code_client, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(header_date, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(header_solde, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), header_code_compte, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), header_code_client, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), header_date, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), header_solde, 3, 0, 1, 1);

    // Définir une taille minimale pour les colonnes
    gtk_widget_set_size_request(header_code_compte, 100, -1);
    gtk_widget_set_size_request(header_code_client, 150, -1);
    gtk_widget_set_size_request(header_date, 150, -1);
    gtk_widget_set_size_request(header_solde, 150, -1);

    // Ajouter chaque compte
    for (int i = 0; i < nbComptes; i++) {
        char code_compte_str[20], code_client_str[20], date_str[50],solde_str[20];

        sprintf(code_compte_str, "%d", comptes[i].code_compte);
        sprintf(code_client_str, "%d", comptes[i].code_client);
        snprintf(date_str, sizeof(date_str), "%02d/%02d/%04d", 
                 comptes[i].date_compte.jour, comptes[i].date_compte.mois, comptes[i].date_compte.annee);
        sprintf(solde_str, "%f", comptes[i].solde);

        GtkWidget *code_compte_label = gtk_label_new(code_compte_str);
        GtkWidget *code_client_label = gtk_label_new(code_client_str);
        GtkWidget *date_label = gtk_label_new(date_str);
        GtkWidget *solde_label = gtk_label_new(solde_str);

        // Centrer chaque widget dans sa cellule
        gtk_widget_set_halign(code_compte_label, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(code_client_label, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(date_label, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(solde_label, GTK_ALIGN_CENTER);

        gtk_grid_attach(GTK_GRID(grid), code_compte_label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), code_client_label, 1, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), date_label, 2, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), solde_label, 3, i + 1, 1, 1);
    }

    // Mettre la grille dans la fenêtre scrollable
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), grid);
    gtk_box_append(GTK_BOX(form_box), scrolled_window);
}
