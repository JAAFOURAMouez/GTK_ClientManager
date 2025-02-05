#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>

#define MAX_CLIENTS 100
#ifndef DATA_DIR
#define DATA_DIR "data/"
#endif
#define FILE_NAME_CLIENT DATA_DIR "client.txt"

// Structure Client
typedef struct {
    int code_client;
    char nom[255];
    char prenom[255];
} SClient;

#endif // CLIENT_H
