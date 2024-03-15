#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define TAILLE_BUFFER 1024

int client_socket;

// Fonction pour gérer la réception des messages du serveur
void *recevoirMessages(void *arg) {
    char messageBuffer[TAILLE_BUFFER];

    while (1) {
        int read_size = read(client_socket, messageBuffer, TAILLE_BUFFER);
        if (read_size <= 0) {
            printf("Déconnecté\n");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        printf("%s", messageBuffer);
    }
}

// Fonction pour gérer l'interruption de l'affichage
void arreterAffichage(int signum) {
    printf("\nSaisissez votre message: ");
    char message[TAILLE_BUFFER];
    fgets(message, TAILLE_BUFFER, stdin);
    if (strcmp(message, "exit\n\0") == 0) {
        close(client_socket);
        exit(EXIT_SUCCESS);
    }
    write(client_socket, message, strlen(message));
}

int main() {
    printf("Veuillez entrer les informations de connexion :\n\t- IP (127.0.0.1 par défaut): ");
    char ipServeur[16];
    fgets(ipServeur, 16, stdin);
    if (ipServeur[0] == '\n') {
        strcpy(ipServeur, "127.0.0.1"); 
    }
    printf("\t- Port (8888 par défaut) : ");
    int portServeur;
    char entreePort[20];
    fgets(entreePort, sizeof(entreePort), stdin);
    if (sscanf(entreePort, "%d", &portServeur) != 1) {
        portServeur = 8888;
    }


    struct sockaddr_in server_addr;
    pthread_t thread;

    // Création du socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        fprintf(stderr, "Erreur lors de la création du socket\n");
        return 1;
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portServeur);
    inet_pton(AF_INET, ipServeur, &server_addr.sin_addr);

    // Connexion au serveur
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Erreur lors de la connexion au serveur\n");
        close(client_socket);
        return 1;
    }

    printf("Connexion au serveur réussi\n");

    // Création d'un thread pour recevoir les messages du serveur
    pthread_create(&thread, NULL, recevoirMessages, NULL);

    // Gestion de l'interruption de l'affichage pour écrire un message
    signal(SIGINT, arreterAffichage);

    // Boucle principale pour envoyer des messages
    while (1) {
        // Attente de l'interruption de l'affichage pour écrire un message
        pause();
    }

    close(client_socket);

    return 0;
}
