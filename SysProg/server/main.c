/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * main.c: Hauptprogramm des Servers
 */

#include <sys/socket.h>
//#include <login.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#define MAX_CLIENTS 5
#define BLOCKSIZE 1023

char message [64] = "Verbindung hergestellt. Bitte Login-Daten eingeben.";
char buffer [BLOCKSIZE];

size_t MESSAGE = 64;
size_t DATA = 1024;
int size;
struct sockaddr_in server_in;

pthread_t LOGIN, CLIENT [MAX_CLIENTS];
//const unsigned int FLAG = 0;


int main(int argc, char *argv[]) {

    //int size;
    int create_socket;


    if (argc != 2 ) {
        printf("Fehler: Falsche/keine Parameter!\n");
        exit(1);
    }

    create_socket = ServerInit(argv[1]);
    ClientInit(create_socket);


    //fork(loader als kind)


    /*int isTrue = 0;

    while (isTrue == 0) {
            if (pthread_create(&LOGIN, NULL, &(&loginThread)(socketnr), NULL ) != 0)
                perror("Login-Thread konnte nicht erzeugt werden: ");
            else
                isTrue = 1;
        }*/



    /* debugEnable()

     infoPrint("Server Gruppe xy");
     */
    return 0;
}
int loginThread(int client) {


    if (client == 1)
        return 0;
    else
        return -1;
}

void ClientInit(int _create_socket) {

    socklen_t addrlen;
    int client_socket[MAX_CLIENTS];
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

int ServerInit(int port) {

    int s_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (s_socket == -1) {
        perror("Socket kann nicht erstellt werden: ");
        exit(1);
    }

    printf("Socket wurde angelegt.\n");
    server_in.sin_family = AF_INET;
    server_in.sin_addr.s_addr = INADDR_ANY;
    server_in.sin_port = htons(port);

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
