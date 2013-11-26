#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "login.h"
#include "../common/util.h"
#include "../common/rfc.h"



int main(int argc, char *argv[]) {
        pthread_t login_thread;

    if (argc != 2 ) {
        printf("Fehler: Bitte (nur) Port angeben!\n");
        exit(1);
    }
    setProgName(argv[0]);
    debugEnable();
    infoPrint("Server Gruppe 10");

    //fork

    PORT = atoi(argv[1]);

    if(pthread_create(&login_thread, NULL, (void*)&LoginInit, NULL) != 0) {

            infoPrint("Konnte keinen Login-Thread erzeugen");
            exit(1);
    }

        pthread_join(login_thread,NULL);


    return 0;
}


