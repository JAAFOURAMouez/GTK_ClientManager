#ifndef OUTILS_H
#define OUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "client.h"
#include "compte.h"

void convertir_majuscule(char *chaine);
int comparer(const void *a, const void *b);
int comparer_comptes(const void *a, const void *b);
int lire_clients(SClient clients[]);
int code_client_existe(int code_client);
int date_valide(int jour, int mois, int annee);
int lire_comptes(SCompte comptes[]);
int code_compte_existe(int code_compte);
#endif
