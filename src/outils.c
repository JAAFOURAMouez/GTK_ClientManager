#include "outils.h"
#include "client.h"
#include "comtpe_menu.h"

int date_valide(int jour, int mois, int annee) {
    if (annee < 1900 || annee > 2100) return 0;
    if (mois < 1 || mois > 12) return 0;
    
    int jours_par_mois[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Année bissextile
    if ((annee % 4 == 0 && annee % 100 != 0) || annee % 400 == 0) {
        jours_par_mois[2] = 29;
    }
    
    return (jour >= 1 && jour <= jours_par_mois[mois]);
}

void convertir_majuscule(char *chaine) {
    for (int i = 0; chaine[i] != '\0'; i++) {
        chaine[i] = toupper((unsigned char)chaine[i]);
    }
}
int comparer_comptes(const void *a, const void *b) {
    const SCompte *compteA = (const SCompte *)a;
    const SCompte *compteB = (const SCompte *)b;
    return compteA->code_client - compteB->code_client;
}

int comparer(const void *a, const void *b) {
    const SClient *clientA = (const SClient *)a;
    const SClient *clientB = (const SClient *)b;
    return clientA->code_client - clientB->code_client;
}

int lire_clients(SClient clients[]) {
    FILE *file = fopen(FILE_NAME_CLIENT, "r");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }
    int nbClients = 0;
    while (fread(&clients[nbClients], sizeof(SClient), 1, file)) {
        nbClients++;
        if (nbClients >= MAX_CLIENTS) {
            printf("Nombre maximum de clients atteint (%d).\n", MAX_CLIENTS);
            break;
        }
    }
    fclose(file);
    return nbClients;
}

int lire_comptes(SCompte comptes[]) {
    FILE *file = fopen(FILE_NAME_COMPTE, "r");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }
    int nbComptes = 0;
    while (fread(&comptes[nbComptes], sizeof(SCompte), 1, file)) {
        nbComptes++;
        if (nbComptes >= MAX_COMPTES) {
            printf("Nombre maximum de comptes atteint (%d).\n", MAX_COMPTES);
            break;
        }
    }
    fclose(file);
    return nbComptes;
}

int code_client_existe(int code_client) {
    SClient clients[MAX_CLIENTS];
    int nbClients = lire_clients(clients);
    
    for (int i = 0; i < nbClients; i++) {
        if (clients[i].code_client == code_client) {
            return 1;
        }
    }
    return 0;
}

int code_compte_existe(int code_compte) {
    SCompte comptes[MAX_COMPTES];
    int nbComptes = lire_comptes(comptes);
    for (int i = 0; i < nbComptes; i++) {
        if (comptes[i].code_compte == code_compte) {
            return 1;
        }
    }
    return 0;
}

