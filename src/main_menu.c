#include "main_menu.h"
#include "widgets.h"
#include "comtpe_menu.h"
#include "client_menu.h"
#include "operation_menu.h"

void on_form_action(GtkWidget *button, gpointer user_data) {
    SubMenuChoice *choice = (SubMenuChoice *)user_data;

    // Nettoyer le formulaire existant
    GtkWidget *child = gtk_widget_get_first_child(choice->widgets->form_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Gérer les différentes actions en fonction des choix
    switch (choice->main_choice) {
        case 1: // Gestion des clients
            switch (choice->sub_choice) {
                case 1:
                    show_add_client_form(choice->widgets->form_box);
                    break;
                case 2:
                    show_modify_client_form(choice->widgets->form_box);
                    break;
                case 3:
                    show_delete_client_form(choice->widgets->form_box);
                    break;
                case 4:
                    show_display_clients_form(choice->widgets->form_box);
                    break;
            }
            break;

        case 2:
            switch (choice->sub_choice){
            case 1:
                show_add_account_form(choice->widgets->form_box);
                break;
            
            case 2:
                show_view_account_form(choice->widgets->form_box);
                break;
            case 3:
                show_delete_account_form(choice->widgets->form_box);
                break;
            case 4:
                show_display_comptes_form(choice->widgets->form_box);
                break;
            }
            break;
        case 3:
            switch (choice->sub_choice){
                case 1:
                    show_depot_form(choice->widgets->form_box);
                    break;
                case 2:
                    show_retrait_form(choice->widgets->form_box);
                    break;
                case 3:
                    show_virement_form(choice->widgets->form_box);
                    break;
                case 4:
                    show_operation_form(choice->widgets->form_box);
                    break;
            }
            break;
    }
}

void create_main_menu(GtkWidget *menu_box, MenuWidgets *widgets) {
    const char *main_labels[] = {
        "1- Gestion des clients",
        "2- Gestion des comptes",
        "3- Gestion des opérations",
        "4- Quitter le programme"
    };

    for (int i = 0; i < 4; i++) {
        GtkWidget *button = gtk_button_new_with_label(main_labels[i]);
        g_object_set_data(G_OBJECT(button), "menu_choice", GINT_TO_POINTER(i + 1));
        g_signal_connect(button, "clicked", G_CALLBACK(on_sous_menu_clicked), widgets);
        gtk_box_append(GTK_BOX(menu_box), button);
    }
}

void on_sous_menu_clicked(GtkWidget *button, MenuWidgets *widgets){
    int choice = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "menu_choice"));
    
    // Nettoyer le sous-menu existant
    GtkWidget *child = gtk_widget_get_first_child(widgets->sous_menu_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    const char *sous_menu_labels[4][4] = {
        { // Gestion des clients
            "1- Ajouter un client",
            "2- Modifier un client",
            "3- Supprimer un client",
            "4- Afficher les clients"
        },
        { // Gestion des comptes
            "1- Créer un compte",
            "2- Modifier un compte",
            "3- Supprimer un compte",
            "4- Afficher les comptes"
        },
        { // Gestion des opérations
            "1- Effectuer un dépôt",
            "2- Effectuer un retrait",
            "3- Effectuer un virement",
            "4- Afficher les opérations"
        },
        { // Option Quitter - pas de sous-menu
            NULL
        }
    };

    // Si c'est l'option Quitter
    if (choice == 4) {
        GtkRoot *root = gtk_widget_get_root(button);
        if (GTK_IS_WINDOW(root)) {
            gtk_window_destroy(GTK_WINDOW(root));
        }
        return;
    }

    // Créer les boutons du sous-menu
    for (int i = 0; sous_menu_labels[choice-1][i] != NULL && i < 4; i++) {
        GtkWidget *sous_button = gtk_button_new_with_label(sous_menu_labels[choice-1][i]);
        
        // Stocker le choix du menu principal et du sous-menu
        SubMenuChoice *sub_choice = g_new(SubMenuChoice, 1);
        sub_choice->main_choice = choice;
        sub_choice->sub_choice = i + 1;
        sub_choice->widgets = widgets;
        
        g_signal_connect(sous_button, "clicked", G_CALLBACK(on_form_action), sub_choice);
        gtk_box_append(GTK_BOX(widgets->sous_menu_box), sous_button);
    }
}
