/****************************************************************************/
/*							main-MODULHEADER	  							*/
/****************************************************************************/
/* Projekt:				Systemprogrammierung - Multiplayquiz				*/
/* Modulname:			main.c												*/
/* Modeltyp				funktionell											*/
/* Sprache/Compiler:	C - gcc											    */
/* Kurzbeschreibung:	Das Modul realisiert den Login des clients		    */
/* Ersteller:			Gruppe 08											*/
/* Letzte Änderung 		27.11.12											*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*Includes des main Moduls(client)       									*/
/*--------------------------------------------------------------------------*/

#include "../common/util.h"
#include "gui/gui_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "../common/rfc.h"
#include <arpa/inet.h>
#include "listener.h"

#define MAX_LENGTH 1024


int main(int argc, char **argv) {

	char Name[32]; 		//Name des Clients
	setProgName(argv[0]);
	int i, a; //zähler
	char server_addr[16] = "127.0.0.1"; //IP-adresse
	int port = 54321; // Standard Port
	PACKET init,answer;
	struct sockaddr_in client;
	struct hostent *hostAddress;
	pthread_t listener;

/*--------------------------------------------------------------------------*/
/*Abfrage der Argumente 													*/
/*--------------------------------------------------------------------------*/

	if (argc == 1) {
		printf("-n Spielername zwingend angeben \n");
		return 0;
	}

	for (i = 1; i < argc; i++) {				//Name einlesen
		if (strcmp("-n", argv[i]) == 0) {	    //argumenttyp erkennen
			printf("-n erkannt \n ");
			if ((i + 1) >= argc) {
				printf("Spielername fehlt \n");
				return 0;
			}
			a = strlen(argv[i + 1]);

			if (a > 31) {					// Länge des Namens Prüfen
				printf("%d \n", a);			//Das ist doch Aufgabe des Servers!?
				printf("Name zu lang \n");
				return 0;
			} else {
				strncpy(Name, argv[i + 1], 31);
				Name[a]='\0';
				printf("Name: %s \n",Name);
			}
		}
		if (strcmp("-s", argv[i]) == 0) {// Ip adresse falls angegeben auslesen
										 // anderen falls wird 127.0.0.1 genommen
			printf("-s erkannt \n ");
			if ((i + 1) >= argc) {
				printf("argument fehlt \n");
				return 0;
			}
			a = strlen(argv[i + 1]);
			if (a > 16) {
				printf("%d \n", a);
				printf("IP zu lang \n");
				return 0;
			} else {
				strncpy(server_addr, argv[i + 1], 16);

				printf("IP ist %s \n", server_addr);
			}
		}

			if (strcmp("-p", argv[i]) == 0) { 	//Port einlesen falls angegeben
				printf("-p erkannt \n ");
				if ((i + 1) >= argc) {
					printf("argument fehlt \n");
					return 0;
				}
				a = strlen(argv[i + 1]);
				if (a > 5) {
					printf("%d \n", a);
					printf("Port zu lang \n");
					return 0;
				} else {
					port = atoi(argv[i+1]);
					printf("Port ist %d \n", port);
				}
			}


	}

	guiInit(&argc, &argv);	//GUI initialisieren
	debugEnable();
	infoPrint("Client Gruppe 08");

/*--------------------------------------------------------------------------*/
/*Aufbau der Verbindung  													*/
/*--------------------------------------------------------------------------*/

	int sock = socket(AF_INET, SOCK_STREAM, 0); //socket erstellen Ipv4 TCP


	printf("socket: %d\n",sock);

	client.sin_family = AF_INET;			// Ipv4
	hostAddress = gethostbyname(server_addr); // adresstruktur zur Ip bekommen
	if (hostAddress == NULL ) {
		perror("Host not found:");
		return 1;
	}

	bcopy(hostAddress->h_addr, &(client.sin_addr), hostAddress->h_length); // benötigten teil in client.sin_addr kopieren

	client.sin_port = htons(port);

	if (connect(sock,(struct sockaddr*) &client, sizeof(client)) < 0) { // Verbindung zum Server herstellen
		perror("connect failed: ");
		return 5;
	}
/*--------------------------------------------------------------------------*/
/*Login Anfrage	senden 	 													*/
/*--------------------------------------------------------------------------*/

	init=createLoginRe(Name); // LoginResponse Packet erstellen

	a = write(sock, &init, ntohs(init.head.length) + sizeof(init.head));//Login Request senden

	printf("%d gesendet \n", a);
	if(read(sock,&answer.head,3)<0){
		perror("Lesen fehlgeschlagen:");
	}
/*--------------------------------------------------------------------------*/
/*Login Antwort Auswerten 													*/
/*--------------------------------------------------------------------------*/
	read(sock,&answer.data,ntohs(answer.head.length));

	switch(answer.head.type){

		case LOGIN_RSP:
				printf("loginAnswer\n");
					if (answer.data.ID == 0) {
						infoPrint("Du bist Spielleiter\n");
					}
				infoPrint("Listener wird gestartet");
			break;

		case ERROR:
				answer.data.error.message[ntohs(answer.head.length)]='\0';
				printf("Login fehlgeschlagen, Message: %s \n",answer.data.error.message);
				return 2;
		default:
				infoPrint("Keine Gültige Antwort erhalten");
				return 3;
	}



	if(pthread_create(&listener,NULL,(void*)&listenerMain,&sock)!=0){ //Listener Thread starten mit socket als Parameter
		perror("Konnte keinen Login-Thread erzeugen: ");
		return 4;
	}

	preparation_showWindow(); // Fenster Anzeigen
	guiMain();				  //Gui  Main starten --> Blockiert bis ende der Gui


	guiDestroy();

	return 0;
}

void preparation_onCatalogChanged(const char *newSelection) {
	debugPrint("Katalogauswahl: %s", newSelection);
}

void preparation_onStartClicked(const char *currentSelection) {
	debugPrint("Starte Katalog %s", currentSelection);
}

void preparation_onWindowClosed(void) {
	debugPrint("Vorbereitungsfenster geschlossen");
	guiQuit();
}

void game_onAnswerClicked(int index) {
	debugPrint("Antwort %i ausgewählt", index);
}

void game_onWindowClosed(void) {
	debugPrint("Spielfenster geschlossen");
	guiQuit();
}
