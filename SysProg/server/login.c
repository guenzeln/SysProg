/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 * 
 * login.c: Implementierung des Logins
 */

#include "login.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_CLIENTS 5
#define BLOCKSIZE 1024

char message [64] = "Verbindung hergestellt. Bitte Login-Daten eingeben.";
char buffer [BLOCKSIZE];

size_t MESSAGE = 64;
size_t DATA = 1024;
ssize_t size;
pthread_t LOGIN, CLIENT [MAX_CLIENTS];
static struct sockaddr_in server_in;

Login_Init(int port) {
	int create_socket;
	create_socket = Server_Init(port);
	ClientInit(create_socket);

}



void ClientInit(int _create_socket) {

    socklen_t addrlen;
    int client_socket[MAX_CLIENTS];//asdasdsdasdsad
    int c_init = 0;

    while (1) {
            if (c_init < MAX_CLIENTS) {
                client_socket[c_init] = accept(_create_socket, (struct sockaddr *) &server_in, &addrlen);
                if (client_socket[c_init] < 0)
                    perror("Fehler beim Verbinden mit Socket: ");
                else {
                    if (write(client_socket[c_init], message, MESSAGE-1) < 0)
                        perror("Datenverlust beim Senden");
                    else {
                        printf("Daten vollstaendig gesendet. Warte auf Login-Request vom Client...");
                        if ((size=read(client_socket[c_init], buffer, DATA-1)) < 0)
                            perror("Daten konnten nicht gelesen werden!");
                        else
                            buffer[size] = '\0';
                            printf("Nachricht empfangen: %s\n", buffer);
                    }
                c_init++;
                }
            }
            else
                printf("Maximale Anzahl an Clients erreicht!");
    }
}

int ServerInit(int _port) {

    int s_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (s_socket == -1) {
        perror("Socket kann nicht erstellt werden: ");
        exit(1);
    }

    printf("Socket wurde angelegt.\n");
    server_in.sin_family = AF_INET;
    server_in.sin_addr.s_addr = INADDR_ANY;
    server_in.sin_port = htons(_port);

    if(bind(s_socket, (struct sockaddr *)&server_in, sizeof(server_in)) == -1) {
        perror("Socket konnte nicht an die Adresse gebunden werden: ");
        exit(1);
    }
    if (listen(s_socket, MAX_CLIENTS) == -1){
        perror("Fehler beim Eingehen von Verbindungsanfragen: ");
        exit(1);
    }

return s_socket;
}

int loginThread(int client) {


    if (client == 1)
        return 0;
    else
        return -1;
}
