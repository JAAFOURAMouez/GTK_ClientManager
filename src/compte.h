#ifndef COMPTE_H
#define COMPTE_H
#include <stdio.h>
#include"outils.h"
#define MAX_COMPTES 100
#ifndef DATA_DIR
#define DATA_DIR "data/"
#endif
#define FILE_NAME_COMPTE DATA_DIR "compte.txt"
typedef struct {
    int jour;
    int mois;
    int annee;
} Date;
// Structure Compte
typedef struct {
    int code_compte;
    int code_client;
    float solde;
    Date date_compte;
} SCompte;

#endif 
