#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h> // Pour strerror
#include <errno.h>  // Pour errno

#define NB_MAX_CLIENTS 10
#define TAILLE_BUFFER 1024
#define PORT 8888

typedef struct {
    int id;
    int socket;
    struct sockaddr_in address;
    char nomUtilisateur[21];
    int nbMessage;
} client_info;

client_info clients[NB_MAX_CLIENTS];
pthread_t threads[NB_MAX_CLIENTS];
int num_clients = 0;
int server_socket;

void *nouvelleConnexion(void *arg) {
    int client_id = *((int *)arg);
    char messageBuffer[TAILLE_BUFFER];

    while (1) {
        memset(messageBuffer, 0, TAILLE_BUFFER);
        int read_size = read(clients[client_id].socket, messageBuffer, TAILLE_BUFFER);
        if (read_size <= 0) {
            printf("Client %d déconnecté\n", client_id);
            close(clients[client_id].socket);
            pthread_exit(NULL);
        }
        if (clients[client_id].nbMessage == 0) {
            sprintf(clients[client_id].nomUtilisateur, "%s", messageBuffer);
        }
        else {
            char messageBufferModifie[TAILLE_BUFFER + 27];
            memset(messageBufferModifie, 0, TAILLE_BUFFER + 27);
            sprintf(messageBufferModifie, "[%s] : %s", clients[client_id].nomUtilisateur, messageBuffer);
        
            printf("%s", messageBufferModifie);
            //Envoi du message à tous les clients
            for (int i = 0; i < num_clients; i++) {
                if (i != client_id) {
                    write(clients[i].socket, messageBufferModifie, strlen(messageBufferModifie));
                }
            }
        }
        clients[client_id].nbMessage++;
    }
}


void entrerCommande(int signum) {
    printf("\nEntrez votre commande: ");
    char commande[TAILLE_BUFFER];
    fgets(commande, TAILLE_BUFFER, stdin);
    if (strcmp(commande, "message\n\0") == 0) {
        printf("\nEntrez le message à envoyer à tous les clients : ");
        char messageBuffer[TAILLE_BUFFER];
        fgets(messageBuffer, TAILLE_BUFFER, stdin);
        char messageBufferModifie[TAILLE_BUFFER];
        sprintf(messageBufferModifie, "\033[31m[Serveur] : \033[0m%s", messageBuffer);
        for (int i = 0; i < num_clients; i++) {
            write(clients[i].socket, messageBufferModifie, strlen(messageBufferModifie));
        }
    }
    else if (strcmp(commande, "port\n\0") == 0) {
        //changement de port dans le fichier de configuration
    }
    else if (strcmp(commande, "help\n\0") == 0) {
        printf("Les commandes disponibles :\n\tmessage : Envoyer un message à tous les clients connectés\n\tport : Modifier le port du serveur\n");
        printf("\thelp : Connaître toutes les commandes\n\texit : Fermer le serveur\n");
    }
    else if (strcmp(commande, "exit\n\0") == 0) {
        close(server_socket);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);


    // Fichier de configuration
    

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "Erreur lors de la création du socket\n");
        return 1;
    }

    //Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int checkBind = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (checkBind == -1) {
        fprintf(stderr, "Erreur lors de l'attachement (bind)\n");
        fprintf(stderr, "%s\n", strerror(errno));
        return 1;
    }

    int checkListen = listen(server_socket, NB_MAX_CLIENTS); //Attend les connexions entrantes
    if (checkListen == -1) {
        fprintf(stderr, "Erreur lors de l'écoute\n");
        return 1;
    }

    printf("Attente de connexion sur le port %d ...\n", PORT);

    signal(SIGINT, entrerCommande);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            fprintf(stderr, "Erreur lors de l'acceptation de la connexion\n");
            continue;
        }

        clients[num_clients].id = num_clients;
        clients[num_clients].socket = client_socket;
        clients[num_clients].address = client_addr;

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Connexion du client %d : %s\n", num_clients, client_ip);

        pthread_create(&threads[num_clients], NULL, nouvelleConnexion, (void *)&clients[num_clients].id);

        num_clients++;
    }

    close(server_socket);

    return 0;
}
