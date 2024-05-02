# Messagerie_instantanee_C

## Description
Le but du projet consiste en la création d'un fichier server.c qui gère les connexions entrantes avec les sockets et les threads. Le fichier client.c permet la connexion vers le serveur que l'on désire (en entrant l'adresse IP et le port du serveur).


## Informations complémentaires
Nous avons lancé sur un VPS personnel le programme server.c ce qui permet de s'y connecter pour effectuer des tests dans des conditions réelles. Voici l'IP et le port du serveur : 37.187.37.19:8888


## Ajouts
Nous avons ajoutés quelques fonctionnalités, les voici : 
- Commandes pour le serveur : 
        - message <votreMessage> : Envoie un message depuis le serveur à tous les clients connecté avec le nom du serveur en rouge
        - show : Affiche ou n'affiche plus les messages envoyés par les clients dans la console du serveur
        - help : Affiche la description de toutes les commandes disponibles sur le serveur
        - exit : ferme toutes les connexions en cours et ferme le programme
- Pseudo pour chaque client
- Possiblité de choisir l'adresse IP et le port du serveur auquel nous voulons nous connecter
- Possiblité de modifier le port du serveur dans le fichier port.txt

## Composition du projet

Le fichier server.c possède les fonctions suivantes : 
    - nouvelleConnexion : gére la réception des messages de chaque client pour les renvoyer à tous les autres clients
    - entrerCommande : gére les entrées des commandes quand le programme est interrompu par ctrl+c
    - lirePort : créer le fichier port.txt s'il n'existe pas et lit son contenu. Si le port est valable alors il utilise celui-ci sinon il écrit dans le fichier et utilise le port par défaut 8888.
    - main : lance la fonction lirePort, configure le socket et attend des connexions entrantes pour les créer des threads qui utiliseront la fonction nouvelleConnexion


Le fichier client.c possède les fonctions suivantes : 
    - recevoirMessage : gére la réception des messages venant du serveur et le stockage de ceux-ci si l'utilisateur saisit son messsage
    - creerTableau : créer un tableau de chaine de caractère pour stocker les messages pendant que l'utilisateur saisit son message
    - arreterAffichage : quand l'utilisateur fait ctrl+c cette fonction est appelée pour entrer le message à envoyer
    - main : demande à l'utilisateur les informations de connexion, configure la socket pour se connecter au serveur et créer un thread avec la fonction recevoirMessage



## Compiler

Pour compiler les 2 programmes, il faut tapper la commande suivante dans le même dossier où se trouve les fichiers : gcc server.c -o server && gcc client.c -o client


## Exécuter

Pour exécuter le programme serveur, il vous suffit d'entrer la commande suivante : ./server
Pour exécuter le programme client, il vous suffit d'entrer la commande suivante : ./client


## Utilisation

Lorsque l'on démarrer le serveur, celui-ci créer un fichier port.txt qui stocke le port utilisé pour la connexion au serveur. Le fichier peut être modifié pour changer de port, il faudra simplement redémarrer le serveur pour prendre en compte les modifications. Par défaut, les messages des clients s'affiche dans la console du serveur. Le serveur dispose de plusieurs commandes telles que message, show, help ou exit (leur utilité est décrite dans la séction "Ajouts"). Ces commandes sont utilisables une fois avoir fait ctrl+c.
Lorsque l'on lance le client, celui-ci nous demande les informations de connexion au serveur telles que l'adresse IP, le port et le pseudo utilisé. Pour envoyer un message, il suffit de faire ctrl+c, d'écrire son message et d'appuyer sur entrer. A la place d'écrire un message, nous pouvons écrire la commande exit qui aura pour effet de fermer la connexion au serveur et de fermer l'application.
