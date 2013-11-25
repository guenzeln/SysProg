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


void listenerMain(int* s){
		PACKET answer;
		int i=0;

		printf("Vor WHILE \n");

		printf("socket: %d\n",s);


		while(read(*s, &answer.head, 3)>=0){			//Auf antwort des servers ?warten
		infoPrint("vor read");
		read(*s, &answer.data, ntohs(answer.head.length));
		infoPrint("nach read");
		switch (answer.head.type) {

		case 2:
			printf("loginAnswer\n");
			if (answer.data.ID == 0) {
				printf("Du bist Spielleiter\n");
			}
			break;

		case 6:
			printf("Spielerliste\n");
			int player_count=ntohs(answer.head.length)/37;
			printf("Spieler: %d\n",player_count);
			preparation_clearPlayers();
			for(i=0;i<player_count;i++){
				char name[32];
				strncpy(name,answer.data.playerlist[i].playername,32);
				preparation_addPlayer(name);

				//game_setPlayerScore(answer.data.playerlist[i].player_id+1,answer.data.playerlist[i].score);
			}
			break;
		}
		}

}
