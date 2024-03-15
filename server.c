#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Structure pour stocker les informations des clients
typedef struct {
    int id;
    int socket;
    struct sockaddr_in address;
} client_info;

client_info clients[MAX_CLIENTS];
pthread_t threads[MAX_CLIENTS];
int num_clients = 0;

// Fonction pour gérer la connexion d'un client
void *handle_client(void *arg) {
    int client_id = *((int *)arg);
    char buffer[BUFFER_SIZE];

    while (1) {
        // Réception du message du client
        int recv_size = recv(clients[client_id].socket, buffer, BUFFER_SIZE, 0);
        if (recv_size <= 0) {
            printf("Client %d disconnected\n", client_id);
            close(clients[client_id].socket);
            pthread_exit(NULL);
        }

        // Envoi du message reçu à tous les clients
        for (int i = 0; i < num_clients; i++) {
            if (i != client_id) {
                send(clients[i].socket, buffer, recv_size, 0);
            }
        }
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Création du socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // Attachement du socket à l'adresse
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Écoute des connexions entrantes
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8888\n");

    // Acceptation des connexions entrantes
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Ajout du client à la liste
        clients[num_clients].id = num_clients;
        clients[num_clients].socket = client_socket;
        clients[num_clients].address = client_addr;

        // Création d'un thread pour gérer ce client
        pthread_create(&threads[num_clients], NULL, handle_client, (void *)&clients[num_clients].id);

        num_clients++;
    }

    close(server_socket);

    return 0;
}
