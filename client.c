#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int client_socket;

// Fonction pour gérer la réception des messages du serveur
void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        int recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_size <= 0) {
            printf("Server disconnected\n");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        printf("Message from server: %s", buffer);
    }
}

// Fonction pour gérer l'interruption de l'affichage
void sigint_handler(int signum) {
    printf("\nType your message: ");
    char message[BUFFER_SIZE];
    fgets(message, BUFFER_SIZE, stdin);
    send(client_socket, message, strlen(message), 0);
}

int main() {
    struct sockaddr_in server_addr;
    char server_ip[] = "127.0.0.1"; // Adresse IP du serveur
    pthread_t thread;

    // Création du socket
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Le socket n'a pas pu être créé");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_UNIX;
    server_addr.sin_port = htons(8888);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Connexion au serveur
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("La connexion au serveur n'a pas pu s'établir");
        exit(EXIT_FAILURE);
    }

    // Création d'un thread pour recevoir les messages du serveur
    pthread_create(&thread, NULL, receive_messages, NULL);

    // Gestion de l'interruption de l'affichage pour écrire un message
    signal(SIGINT, sigint_handler);

    // Boucle principale pour envoyer des messages
    while (1) {
        // Attente de l'interruption de l'affichage pour écrire un message
        pause();
    }

    close(client_socket);

    return 0;
}
