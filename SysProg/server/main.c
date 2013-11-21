/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * main.c: Hauptprogramm des Servers
 */

#include <stdio.h>
#include <stdlib.h>
#include "login.h"
//#include "common/util.h"


int main(int argc, char *argv[]) {

    if (argc != 2 ) {
        printf("Fehler: Falsche/keine Parameter!\n");
        exit(1);
    }

    //setProgName();
    //debugEnable();
    //infoPrint("Server Gruppe 10");

    LoginInit(atoi(argv[1]));



    return 0;
}
