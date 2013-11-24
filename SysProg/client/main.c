/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Client
 * 
 * main.c: Hauptprogramm des Clients
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
#include "../common/rfc.h"
#include <arpa/inet.h>

#define MAX_LENGTH 1024

int main(int argc, char **argv) {

	char Name[32];
	setProgName(argv[0]);
	int i, a;
	char server_addr[16] = "127.0.0.1";
	int port = 54321; // Standard Port
	char buf[MAX_LENGTH];
	struct packet init;

	if (argc == 1) {
		printf("-n Spielername zwingend angeben \n");
		exit(0);
	}

	for (i = 1; i < argc; i++) {				//Name einlesen
		if (strcmp("-n", argv[i]) == 0) {	//argumenttyp erkennen
			printf("-n erkannt \n ");
			if ((i + 1) >= argc) {
				printf("Spielername fehlt \n");
				exit(0);
			}
			a = strlen(argv[i + 1]);

			if (a > 32) {					// Länge des Namens Prüfen
				printf("%d \n", a);			//Das ist doch Aufgabe des Servers!?
				printf("Name zu lang \n");
				exit(0);
			} else {
				strncpy(Name, argv[i + 1], 32);

				printf("Name ist %s \n", Name);
			}
		}
		if (strcmp("-s", argv[i]) == 0) {// Ip adresse falls angegeben auslesen
			// anderen falls wird 127.0.0.1 genommen
			printf("-s erkannt \n ");
			if ((i + 1) >= argc) {
				printf("argument fehlt \n");
				exit(0);
			}
			a = strlen(argv[i + 1]);
			if (a > 16) {
				printf("%d \n", a);
				printf("IP zu lang \n");
				exit(0);
			} else {
				strncpy(server_addr, argv[i + 1], 16);

				printf("IP ist %s \n", server_addr);
			}
		}

			if (strcmp("-p", argv[i]) == 0) { 	//Port einlesen falls angegeben
				printf("-p erkannt \n ");
				if ((i + 1) >= argc) {
					printf("argument fehlt \n");
					exit(0);
				}
				a = strlen(argv[i + 1]);
				if (a > 5) {
					printf("%d \n", a);
					printf("Port zu lang \n");
					exit(0);
				} else {
					port = argv[i];

					printf("Port ist %d \n", port);
				}
			}


	}

	guiInit(&argc, &argv);
	debugEnable();
	infoPrint("Client Gruppe 08");

	/* Initialisierung: Verbindungsaufbau, Threads starten usw... */

	int s = socket(AF_INET, SOCK_STREAM, 0); //socket erstellen Ipv4 TCP
	struct sockaddr_in client;
	struct hostent *hostAddress;

	client.sin_family = AF_INET;		// Ipv4
	hostAddress = gethostbyname(server_addr); // adresstruktur zur Ip bekommen
	if (hostAddress == NULL ) {
		printf("Host not found \n");
		exit(0);
	}

	bcopy(hostAddress->h_addr, &(client.sin_addr), hostAddress->h_length); // benötigten teil in client.sin_addr kopieren

	client.sin_port = htons(port);

	if (connect(s, &client, sizeof(client)) < 0) { // Verbindung zum Server herstellen
		printf("connect failed \n");
	}

	init.head.type = 1;
	init.head.length = htons(strlen(Name));
	strncpy(init.data.playername, Name, strlen(Name));

	printf("%d  %d  %s \n", init.head.type, init.head.length,
			init.data.playername);

	a = write(s, &init, strlen(Name) + sizeof(init.head));//Login Request senden

	//if (a != init.head.length + 2) {
	//	printf("stimmt nicht überein \n");
	//}

	printf("%d gesendet \n", a);
	struct packet answer;

	while(read(s, &answer.head, 3)>0){			//Auf antwort des servers ?warten
	read(s, &answer.data, answer.head.length);



	switch (answer.head.type) {

	case 2:
		printf("loginAnswer\n");
		if (answer.data.ID == 0) {
			printf("Du bist Spielleiter\n");
		}
		break;

	case 3:
		printf("Spielerliste\n");
	}
	}
	preparation_showWindow();

	preparation_addPlayer(Name);
	guiMain();
	/* Resourcen freigeben usw... */
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
