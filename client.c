#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define TAILLE_BUFFER 1024
#define MAX_MESSAGE_STOCKE 100

int client_socket;
int entrainEcrire = 0;
char **listeMessages;
int numMessage = 0;

// Fonction pour gérer la réception des messages du serveur
void *recevoirMessages(void *arg) {
    char messageBuffer[TAILLE_BUFFER];

    while (1) {
        memset(messageBuffer, 0, TAILLE_BUFFER);
        int read_size = read(client_socket, messageBuffer, TAILLE_BUFFER);
        if (read_size <= 0) {
            printf("Déconnecté\n");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        if (entrainEcrire == 0) {
            printf("%s", messageBuffer);
        }
        else {
            listeMessages[numMessage] = (char *)malloc(TAILLE_BUFFER * sizeof(char));
            if (listeMessages[numMessage] == NULL) {
                fprintf(stderr, "Erreur lors de l'allocation de la mémoire\n");
                exit(EXIT_FAILURE);
            }
            strcpy(listeMessages[numMessage], messageBuffer);
            numMessage++;
        }
    }
}

void creerTableau() {
    listeMessages = (char **)malloc(MAX_MESSAGE_STOCKE * sizeof(char *));
}

// Fonction pour gérer l'interruption de l'affichage
void arreterAffichage(int signum) {
    entrainEcrire = 1;
    printf("\nSaisissez votre message: ");
    char message[TAILLE_BUFFER];
    fgets(message, TAILLE_BUFFER, stdin);
    if (strcmp(message, "exit\n\0") == 0) {
        close(client_socket);
        exit(EXIT_SUCCESS);
    }
    write(client_socket, message, strlen(message));

    if (numMessage != 0) {
        for (int i = 0 ; i < numMessage ; i++) {
            printf("%s", listeMessages[i]);
        }

        for (int i = 0 ; i < numMessage ; i++) {
            free(listeMessages[i]);
        }
        free(listeMessages);
        numMessage = 0;
        creerTableau();
    }
    entrainEcrire = 0;
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

    printf("\t- Votre nom d'utilisateur : ");
    char nomUtilisateur[21];
    fgets(nomUtilisateur, sizeof(nomUtilisateur), stdin);
    nomUtilisateur[strcspn(nomUtilisateur, "\n")] = '\0';


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

    write(client_socket, nomUtilisateur, strlen(nomUtilisateur));
    printf("Connexion au serveur réussi\n");
    creerTableau();

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
