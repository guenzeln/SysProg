/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * main.c: Hauptprogramm des Servers
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>


int main(int argc, char *argv[]) {

    if (argc != 2 ) {
        printf("Fehler: Falsche/keine Parameter!\n");
        exit(1);
    }

    setProgName();
    debugEnable();
    infoPrint("Server Gruppe 10");

    Login_Init(atoi(argv[1]));



    return 0;
}
