#ifndef WIDGETS_H
#define WIDGETS_H

#include <gtk/gtk.h>

// Structure pour les widgets de modification de client
typedef struct {
    GtkWidget *code_entry;
    GtkWidget *nom_entry;
    GtkWidget *prenom_entry;
    GtkWidget *form_box;
} ModifyClientWidgets;

// Structure pour les widgets de suppression de client
typedef struct {
    GtkWidget *code_entry;
    GtkWidget *form_box;
} DeleteClientWidgets;

// Structure principale des widgets pour l'application
typedef struct {
    GtkWidget *main_box;
    GtkWidget *menu_box;
    GtkWidget *sous_menu_box;
    GtkWidget *form_box;
    GtkWidget *code_entry;
    GtkWidget *nom_entry;
    GtkWidget *prenom_entry;
} MenuWidgets;

// Structure pour gérer les choix des sous-menus
typedef struct {
    int main_choice;
    int sub_choice;
    MenuWidgets *widgets;
} SubMenuChoice;

typedef struct {
    GtkWidget *code_compte_entry;
    GtkWidget *code_client_entry;
    GtkWidget *jour_entry;
    GtkWidget *mois_entry;
    GtkWidget *annee_entry;
    GtkWidget *form_box;
} AccountWidgets;

typedef struct {
    GtkWidget *code_entry;
    GtkWidget *result_label;
} SearchAccountData;


typedef struct {
    GtkWidget *code_entry;
    GtkWidget *montant;
} DepotAccount;

typedef struct {
    GtkWidget *code_source_entry;  // Compte qui envoie l'argent
    GtkWidget *code_dest_entry;    // Compte qui reçoit l'argent
    GtkWidget *montant_entry;      // Montant du virement
} VirementAccount;

// Structure pour passer plusieurs paramètres au callback
typedef struct {
    GtkWidget *entry;
    GtkWidget *form_box;
} CallbackData;
#endif // WIDGETS_H
