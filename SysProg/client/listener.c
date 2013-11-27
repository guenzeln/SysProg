/****************************************************************************/
/*							listener-MODULHEADER	  							*/
/****************************************************************************/
/* Projekt:				Systemprogrammierung - Multiplayquiz				*/
/* Modulname:			listener.c											*/
/* Modeltyp				funktionell											*/
/* Sprache/Compiler:	C - gcc											    */
/* Kurzbeschreibung:	Das Modul realisiert den Listener des clients	    */
/* Ersteller:			Gruppe 08											*/
/* Letzte Änderung 		27.11.12											*/
/****************************************************************************/
/*--------------------------------------------------------------------------*/
/*Includes des listener Moduls       										*/
/*--------------------------------------------------------------------------*/
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


void listenerMain(int* sock){ //EinstiegsFunktion des Listeners

		PACKET answer,empty;
		int i=0;
		memset(&empty,0,sizeof(empty)); //empty wird Komplett auf 0 gesetzt

		infoPrint("Vor WHILE ");
		printf("socket: %d\n",*sock);

/*--------------------------------------------------------------------------*/
/*Auf Datenpakete  warten und auswerten  									*/
/*--------------------------------------------------------------------------*/

		while(read(*sock, &answer.head, 3)>=0){			//Auf antwort des servers warten

			read(*sock, &answer.data, ntohs(answer.head.length));
			switch (answer.head.type) {

				case PLAYERLIST:
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

				case ERRORWARNING:

							switch(answer.data.error.subtype){

								case ERROR_WARNING:		infoPrint("Warning");
											answer.data.error.message[ntohs(answer.head.length)]='\0';
											printf("fehler: s%\n",answer.data.error.message);
											answer=empty;
										break;

								case ERROR_FATAL:
											answer.data.error.message[ntohs(answer.head.length)]='\0';
											printf("fehler:%s \n",answer.data.error.message);
											exit(0);

								default:
											infoPrint("ungültiger Subtype");
										break;
							}
				}

		}
}
