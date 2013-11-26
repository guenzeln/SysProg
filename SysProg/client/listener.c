/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Client
 * 
 * listener.c: Implementierung des Listener-Threads
 */

#include "../common/util.h"
#include "gui/gui_interface.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "../common/rfc.h"
#include <arpa/inet.h>


void listenerMain(int* s){//test
		PACKET answer;
		int i=0;
		PACKET empty;
		memset(&empty,0,sizeof(empty));

		printf("Vor WHILE \n");
		printf("socket: %d\n",*s);




		while(read(*s, &answer.head, 3)>=0){			//Auf antwort des servers ?warten
		read(*s, &answer.data, ntohs(answer.head.length));
		switch (answer.head.type) {

		case 6:
			printf("Spielerliste\n");
			int player_count=ntohs(answer.head.length)/37;
			printf("Spieler: %d\n",player_count);
			preparation_clearPlayers();
			for(i=0;i<player_count;i++){
				char name[32];
				strncpy(name,answer.data.playerlist[i].playername,32);
				preparation_addPlayer(name);
			}

			printf("inhalt vorher: %s",answer.data.playerlist[0].playername);
			answer=empty;
			printf("jetzt inhalt: %s ",answer.data.playerlist[0].playername);
			break;

		case 255:

			switch(answer.data.error.subtype){

			case 0:	infoPrint("Warning");
					answer.data.error.message[ntohs(answer.head.length)]='\0';
					printf("fehler: s%\n",answer.data.error.message);
					//guiShowErrorDialog(answer.data.error.message,0);
					answer=empty;
					break;

			case 1:
				    answer.data.error.message[ntohs(answer.head.length)]='\0';
					printf("fehler:%s \n",answer.data.error.message);
					exit(4);

			default:
					infoPrint("ungültiger Subtype");
					break;
		}
		}

}
}
