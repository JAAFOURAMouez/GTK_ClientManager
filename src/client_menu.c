#include "client_menu.h"
#include "client.h"
#include "widgets.h"
#include "outils.h"

 
void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Gestion Bancaire");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

    // Création du layout principal
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Création des trois colonnes
    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *sous_menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Ajouter les marges
    gtk_widget_set_margin_start(menu_box, 10);
    gtk_widget_set_margin_end(menu_box, 10);
    gtk_widget_set_margin_top(menu_box, 10);
    gtk_widget_set_margin_bottom(menu_box, 10);

    gtk_widget_set_margin_start(sous_menu_box, 10);
    gtk_widget_set_margin_end(sous_menu_box, 10);
    gtk_widget_set_margin_top(sous_menu_box, 10);
    gtk_widget_set_margin_bottom(sous_menu_box, 10);

    gtk_widget_set_margin_start(form_box, 10);
    gtk_widget_set_margin_end(form_box, 10);
    gtk_widget_set_margin_top(form_box, 10);
    gtk_widget_set_margin_bottom(form_box, 10);

    // Création de la structure pour les widgets
    MenuWidgets *widgets = g_new(MenuWidgets, 1);
    widgets->main_box = main_box;
    widgets->menu_box = menu_box;
    widgets->sous_menu_box = sous_menu_box;
    widgets->form_box = form_box;

    // Ajouter les colonnes au conteneur principal
    gtk_box_append(GTK_BOX(main_box), menu_box);
    gtk_box_append(GTK_BOX(main_box), sous_menu_box);
    gtk_box_append(GTK_BOX(main_box), form_box);

    // Créer le menu principal
    create_main_menu(menu_box, widgets);

    gtk_window_present(GTK_WINDOW(window));
}

void show_add_client_form(GtkWidget *form_box) {
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
    GtkWidget *code_label = gtk_label_new("Code Client:");
    GtkWidget *nom_label = gtk_label_new("Nom:");
    GtkWidget *prenom_label = gtk_label_new("Prénom:");

    // Champs de saisie
    GtkWidget *code_entry = gtk_entry_new();
    GtkWidget *nom_entry = gtk_entry_new();
    GtkWidget *prenom_entry = gtk_entry_new();

    // Bouton de validation
    GtkWidget *submit_button = gtk_button_new_with_label("Ajouter");

    // Stocker les widgets dans une structure
    MenuWidgets *widgets = g_new(MenuWidgets, 1);
    widgets->code_entry = code_entry;
    widgets->nom_entry = nom_entry;
    widgets->prenom_entry = prenom_entry;

    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_add_client_clicked), widgets);

    // Placement dans la grille
    gtk_grid_attach(GTK_GRID(grid), code_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), nom_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), nom_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), prenom_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), prenom_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 3, 2, 1);

    gtk_box_append(GTK_BOX(form_box), grid);
}

void on_add_client_clicked(GtkWidget *button, gpointer user_data) {
    MenuWidgets *widgets = (MenuWidgets *)user_data;
    SClient client;
    SClient clients[MAX_CLIENTS];

    const char *code_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)));
    const char *nom_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->nom_entry)));
    const char *prenom_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->prenom_entry)));

    client.code_client = atoi(code_text);
    strncpy(client.nom, nom_text, sizeof(client.nom) - 1);
    strncpy(client.prenom, prenom_text, sizeof(client.prenom) - 1);

    client.nom[sizeof(client.nom) - 1] = '\0';
    client.prenom[sizeof(client.prenom) - 1] = '\0';

    convertir_majuscule(client.nom);
    convertir_majuscule(client.prenom);

    // Vérifier si le code existe déjà
    int nbClients = lire_clients(clients);
    for (int i = 0; i < nbClients; i++) {
        if (clients[i].code_client == client.code_client) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Le code client %d existe déjà",
                client.code_client);
            gtk_window_present(GTK_WINDOW(dialog));
            g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
            return;
        }
    }

    // Ajouter le client
    FILE *file = fopen(FILE_NAME_CLIENT, "a");
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

    if (fwrite(&client, sizeof(SClient), 1, file)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Client ajouté avec succès");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    }
    fclose(file);

    // Vider les champs
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)), "", -1);
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->nom_entry)), "", -1);
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->prenom_entry)), "", -1);
}

void show_display_clients_form(GtkWidget *form_box) {
    // Nettoyer le form_box
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    SClient clients[MAX_CLIENTS];
    int nbClients = lire_clients(clients);

    if (nbClients == 0) {
        GtkWidget *label = gtk_label_new("Aucun client trouvé.");
        gtk_box_append(GTK_BOX(form_box), label);
        return;
    }

    // Trier les clients
    qsort(clients, nbClients, sizeof(SClient), comparer);

    // Créer une liste scrollable
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 300);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    // Créer la grille pour afficher les clients
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    // En-têtes
    GtkWidget *header_code = gtk_label_new("Code Client");
    GtkWidget *header_nom = gtk_label_new("Nom");
    GtkWidget *header_prenom = gtk_label_new("Prénom");

    // Centrer les en-têtes et ajouter à la grille
    gtk_widget_set_halign(header_code, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(header_nom, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(header_prenom, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), header_code, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), header_nom, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), header_prenom, 2, 0, 1, 1);

    // Définir une taille minimale pour les colonnes
    gtk_widget_set_size_request(header_code, 100, -1);
    gtk_widget_set_size_request(header_nom, 150, -1);
    gtk_widget_set_size_request(header_prenom, 150, -1);

    // Ajouter chaque client
    for (int i = 0; i < nbClients; i++) {
        char code_str[20];
        sprintf(code_str, "%d", clients[i].code_client);
        
        GtkWidget *code_label = gtk_label_new(code_str);
        GtkWidget *nom_label = gtk_label_new(clients[i].nom);
        GtkWidget *prenom_label = gtk_label_new(clients[i].prenom);

        // Centrer chaque widget dans sa cellule
        gtk_widget_set_halign(code_label, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(nom_label, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(prenom_label, GTK_ALIGN_CENTER);

        gtk_grid_attach(GTK_GRID(grid), code_label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), nom_label, 1, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), prenom_label, 2, i + 1, 1, 1);
    }

    // Mettre la grille dans la fenêtre scrollable
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), grid);
    gtk_box_append(GTK_BOX(form_box), scrolled_window);
}

void show_delete_client_form(GtkWidget *form_box) {
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

    GtkWidget *code_label = gtk_label_new("Code Client à supprimer:");
    GtkWidget *code_entry = gtk_entry_new();
    GtkWidget *delete_button = gtk_button_new_with_label("Supprimer");

    // Stocker les widgets dans une structure
    DeleteClientWidgets *widgets = g_new(DeleteClientWidgets, 1);
    widgets->code_entry = code_entry;
    widgets->form_box = form_box;

    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_client_clicked), widgets);

    // Placement dans la grille
    gtk_grid_attach(GTK_GRID(grid), code_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), code_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), delete_button, 0, 1, 2, 1);

    gtk_box_append(GTK_BOX(form_box), grid);
}
void on_delete_client_clicked(GtkWidget *button, gpointer user_data) {
    DeleteClientWidgets *widgets = (DeleteClientWidgets *)user_data;
    SClient clients[MAX_CLIENTS];
    
    const char *code_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)));
    int code_client = atoi(code_text);
    
    int nbClients = lire_clients(clients);
    int client_supprime = 0;
    
    for (int i = 0; i < nbClients; i++) {
        if (clients[i].code_client == code_client) {
            // Décaler les éléments après le client à supprimer
            for (int j = i; j < nbClients - 1; j++) {
                clients[j] = clients[j + 1];
            }
            nbClients--;
            client_supprime = 1;
            break;
        }
    }
    
    if (client_supprime) {
        FILE *file = fopen(FILE_NAME_CLIENT, "w");
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
        
        fwrite(clients, sizeof(SClient), nbClients, file);
        fclose(file);
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Client supprimé avec succès");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        
        // Réinitialiser le champ de code
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)), "", -1);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Client avec le code %d introuvable",
            code_client);
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    }
}

void show_modify_client_form(GtkWidget *form_box) {
    // Nettoyer le form_box
    GtkWidget *child = gtk_widget_get_first_child(form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Création du formulaire de recherche
    GtkWidget *search_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(search_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(search_grid), 10);

    GtkWidget *code_label = gtk_label_new("Code Client à modifier:");
    GtkWidget *code_entry = gtk_entry_new();
    GtkWidget *search_button = gtk_button_new_with_label("Rechercher");

    // Création des champs pour la modification (initialement désactivés)
    GtkWidget *nom_label = gtk_label_new("Nouveau Nom:");
    GtkWidget *prenom_label = gtk_label_new("Nouveau Prénom:");
    GtkWidget *nom_entry = gtk_entry_new();
    GtkWidget *prenom_entry = gtk_entry_new();
    GtkWidget *modify_button = gtk_button_new_with_label("Modifier");

    // Désactiver les champs de modification initialement
    gtk_widget_set_sensitive(nom_entry, FALSE);
    gtk_widget_set_sensitive(prenom_entry, FALSE);
    gtk_widget_set_sensitive(modify_button, FALSE);

    // Stocker les widgets dans une structure
    ModifyClientWidgets *widgets = g_new(ModifyClientWidgets, 1);
    widgets->code_entry = code_entry;
    widgets->nom_entry = nom_entry;
    widgets->prenom_entry = prenom_entry;
    widgets->form_box = form_box;

    // Connecter les signaux
    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_client_clicked), widgets);
    g_signal_connect(modify_button, "clicked", G_CALLBACK(on_modify_client_clicked), widgets);

    // Placement dans la grille
    gtk_grid_attach(GTK_GRID(search_grid), code_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), code_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), search_button, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), nom_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), nom_entry, 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(search_grid), prenom_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(search_grid), prenom_entry, 1, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(search_grid), modify_button, 0, 3, 3, 1);

    gtk_box_append(GTK_BOX(form_box), search_grid);
}
void on_modify_client_clicked(GtkWidget *button, gpointer user_data) {
    ModifyClientWidgets *widgets = (ModifyClientWidgets *)user_data;
    SClient clients[MAX_CLIENTS];
    SClient nvClient;
    
    const char *code_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)));
    const char *nom_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->nom_entry)));
    const char *prenom_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->prenom_entry)));
    
    nvClient.code_client = atoi(code_text);
    strncpy(nvClient.nom, nom_text, sizeof(nvClient.nom) - 1);
    strncpy(nvClient.prenom, prenom_text, sizeof(nvClient.prenom) - 1);
    
    nvClient.nom[sizeof(nvClient.nom) - 1] = '\0';
    nvClient.prenom[sizeof(nvClient.prenom) - 1] = '\0';
    
    convertir_majuscule(nvClient.nom);
    convertir_majuscule(nvClient.prenom);
    
    int nbClients = lire_clients(clients);
    int client_modifie = 0;
    
    for (int i = 0; i < nbClients; i++) {
        if (clients[i].code_client == nvClient.code_client) {
            clients[i] = nvClient;
            client_modifie = 1;
            break;
        }
    }
    
    if (client_modifie) {
        FILE *file = fopen(FILE_NAME_CLIENT, "w");
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
        
        fwrite(clients, sizeof(SClient), nbClients, file);
        fclose(file);
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Client modifié avec succès");
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        
        // Réinitialiser le formulaire
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)), "", -1);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->nom_entry)), "", -1);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->prenom_entry)), "", -1);
        
        // Désactiver les champs de modification
        gtk_widget_set_sensitive(widgets->nom_entry, FALSE);
        gtk_widget_set_sensitive(widgets->prenom_entry, FALSE);
        gtk_widget_set_sensitive(gtk_widget_get_next_sibling(widgets->prenom_entry), FALSE);
    }
}

void on_search_client_clicked(GtkWidget *button, gpointer user_data) {
    ModifyClientWidgets *widgets = (ModifyClientWidgets *)user_data;
    SClient clients[MAX_CLIENTS];
    
    const char *code_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->code_entry)));
    int code_client = atoi(code_text);
    
    int nbClients = lire_clients(clients);
    int client_trouve = 0;
    
    for (int i = 0; i < nbClients; i++) {
        if (clients[i].code_client == code_client) {
            // Activer les champs de modification
            gtk_widget_set_sensitive(widgets->nom_entry, TRUE);
            gtk_widget_set_sensitive(widgets->prenom_entry, TRUE);
            gtk_widget_set_sensitive(gtk_widget_get_next_sibling(widgets->prenom_entry), TRUE); // modify_button
            
            // Remplir les champs avec les données actuelles
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->nom_entry)), clients[i].nom, -1);
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widgets->prenom_entry)), clients[i].prenom, -1);
            
            client_trouve = 1;
            break;
        }
    }   
    if (!client_trouve) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Client avec le code %d introuvable",
            code_client);
        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    }
}