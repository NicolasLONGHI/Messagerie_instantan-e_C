#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define NB_MAX_CLIENTS 10
#define TAILLE_BUFFER 1024
#define PORT 8888

// Structure pour stocker les informations des clients
typedef struct {
    int id;
    int socket;
    struct sockaddr_in address;
} client_info;

client_info clients[NB_MAX_CLIENTS];
pthread_t threads[NB_MAX_CLIENTS];
int num_clients = 0;

// Fonction thread pour gérer la connexion d'un client
void *nouvelleConnexion(void *arg) {
    int client_id = *((int *)arg);
    char messageBuffer[TAILLE_BUFFER];

    while (1) {
        // Réception du message du client
        int read_size = read(clients[client_id].socket, messageBuffer, TAILLE_BUFFER);
        if (read_size <= 0) {
            printf("Client %d déconnecté\n", client_id);
            close(clients[client_id].socket);
            pthread_exit(NULL);
        }

        // Envoi du message reçu à tous les clients
        for (int i = 0; i < num_clients; i++) {
            if (i != client_id) {
                write(clients[i].socket, messageBuffer, read_size);
            }
        }
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Création du socket
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "Erreur lors de la création du socket\n");
        return 1;
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_UNIX;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Attachement du socket à l'adresse
    int checkBind = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (checkBind == -1) {
        fprintf(stderr, "Erreur lors de l'attachement (bind)\n");
        return 1;
    }

    // Écoute des connexions entrantes
    int checkListen = listen(server_socket, 5);
    if (checkListen == -1) {
        fprintf(stderr, "Erreur lors de l'écoute\n");
        return 1;
    }

    printf("Attente de connexion sur le port %d ...\n", PORT);

    // Acceptation des connexions entrantes
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            fprintf(stderr, "Erreur lors de l'acceptation de la connexion\n");
            continue;
        }

        // Ajout du client à la liste
        clients[num_clients].id = num_clients;
        clients[num_clients].socket = client_socket;
        clients[num_clients].address = client_addr;

        // Création d'un thread pour gérer ce client
        pthread_create(&threads[num_clients], NULL, nouvelleConnexion, (void *)&clients[num_clients].id);

        num_clients++;
    }

    close(server_socket);

    return 0;
}
