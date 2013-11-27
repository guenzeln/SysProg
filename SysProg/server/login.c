/****************************************************************************/
/*							login-MODULHEADER	  							*/
/****************************************************************************/
/* Projekt:				Systemprogrammierung - Multiplayquiz				*/
/* Modulname:			login.c												*/
/* Modeltyp				funktionell											*/
/* Sprache/Compiler:	C - Eclipse											*/
/* Kurzbeschreibung:	Das Modul realisiert den Login des Multiplayerquiz  */
/* Ersteller:			Gruppe 08											*/
/* Letzte Änderung 		26.11.12											*/
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*Import-Schnittstelle des login-Moduls    									*/
/*--------------------------------------------------------------------------*/
#include "clientthread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------*/
/*Export-Schnittstelle des login-Moduls    	    							*/
/*--------------------------------------------------------------------------*/
#include "../common/rfc.h"
#include "login.h"

/*--------------------------------------------------------------------------*/
/*Modulglobale Datentypen und Variablen    	    							*/
/*--------------------------------------------------------------------------*/
static PACKET loginrcv, loginrsp, loginerr, players;
static PLAYER user_data[MAX_CLIENTS];
static struct sockaddr_in server, client;
static int client_socket[MAX_CLIENTS];
static int client_id = 0;




/****************************************************************************/
/*						FUNKTIONS-HEADER								    */
/****************************************************************************/
/* Funktion: 			void LoginInit()									*/
/* Kurzbeschreibung:	Initialisieren des Logins							*/
/* 						Server-Socket einrichten, danach auf Clients warten	*/
/* Ergebnis:			Der Server wartet in einer Schleife auf Clients, bis*/
/*						das Spiel gestartet wird.							*/
/* Importparameter:		keine												*/
/* Exportparameter:		keine												*/
/****************************************************************************/

void LoginInit() {

	int create_socket;

	create_socket = ServerInit();
	ClientInit(create_socket);
}

/****************************************************************************/
/*						FUNKTIONS-HEADER								    */
/****************************************************************************/
/* Funktion: 			void ClientInit(int)								*/
/* Kurzbeschreibung:	Initialisieren der Clients,	error-messages versenden*/
/* 						Sockets erstellen, accept, in Array 				*/
/* 						schreiben, versenden des players-Arrays				*/
/* Ergebnis:			Der Server speichert Client-Daten in einem Array	*/
/* 						und versendet diese 								*/
/* Importparameter:		int _create_socket 									*/
/* 						//Socketdeskriptor des Server-Sockets				*/
/* Exportparameter:		keine												*/
/****************************************************************************/
void ClientInit(int _create_socket) {

	socklen_t addrlen;
	pthread_t c_thread[MAX_CLIENTS];
	pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
	int i;

		while (1) {
			addrlen = sizeof(client);
			client_socket[client_id] = accept(_create_socket, (struct sockaddr *) &client, &addrlen);
			pthread_mutex_trylock(&data_mutex);
			if (client_socket[client_id] < 0)
				perror("Fehler beim Verbinden mit Socket: ");
			if (client_id == MAX_CLIENTS) {
				SendError(USR_MAX);
				client_id--;
			}
			else {
				printf("Ein neuer Client (%s) ist verbunden\n",inet_ntoa(client.sin_addr));
				if (recv(client_socket[client_id], &loginrcv.head, sizeof(loginrcv.head), 0) < 0)
					perror("Daten konnten nicht gelesen werden!");
				else {
					recv(client_socket[client_id], &loginrcv.data,loginrcv.head.length,0);
					infoPrint("Login-Daten erhalten\n");
					if(CheckData() == USR_EXISTS) {
						infoPrint("Spieler bereits angemeldet. Login abgewiesen.");
						SendError(USR_EXISTS);
					}
					else {
						//send(client_socket[client_id], &loginrsp, sizeof(loginrsp.head)+loginrsp.head.length, 0);
						send(client_socket[client_id], &loginrsp, 4, 0);
						players.head.type = PLAYERLIST;
						players.head.length = htons((client_id+1)*37);
						for (i = 0; i <= client_id; i++) {
							players.data.playerlist[i].player_id = user_data[i].player_id;
							strncpy(players.data.playerlist[i].playername, user_data[i].playername, PLAYER_LENGTH);
							players.data.playerlist[i].score = user_data[i].score;
						}
						for (i = 0; i <= client_id; i++) {
							send(client_socket[i], &players, sizeof(players.head)+ntohs(players.head.length), 0);
						}
						//Erzeugen eines neuen Clientthreads
						/*if(pthread_create(&c_thread[client_id], NULL,(void *) &StartGame, NULL)!=0)
							infoPrint("Konnte keinen Client-Thread erzeugen");*/
						client_id++;
						pthread_mutex_unlock(&data_mutex);
						infoPrint("Warte auf weitere Anfragen...");
					}
				}
			}
		}
}

/****************************************************************************/
/*						FUNKTIONS-HEADER								    */
/****************************************************************************/
/* Funktion: 			int ServerInit()									*/
/* Kurzbeschreibung:	Server-Socket erstellen, IP-Adresse/Port festlegen	*/
/* Ergebnis:			Funktion gibt Socketdeskriptor zurück und ist für	*/
/*						Anfragen von Clients bereit							*/
/* Importparameter:		keine												*/
/* Exportparameter:		int s_socket 		//Socketdeskriptor				*/
/****************************************************************************/
int ServerInit() {

	int s_socket;

	s_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (s_socket == -1) {
		perror("Socket kann nicht erstellt werden: ");
		exit(1);
	}
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY );
	server.sin_port = htons(PORT);

	if (bind(s_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Socket konnte nicht an die Adresse gebunden werden: ");
		exit(1);
	}
	if (listen(s_socket, MAX_CLIENTS+1) == -1) {
		perror("Fehler beim Eingehen von Verbindungsanfragen: ");
		exit(1);
	}
	infoPrint("Socket wurde angelegt.\nWarte auf Anfragen...\n");
	return s_socket;
}

/*						FUNKTIONS-HEADER								    */
/****************************************************************************/
/* Funktion: 			int CheckData()										*/
/* Kurzbeschreibung:	Die Funktion überprüft das Userdatenarray auf schon */
/*						vorhandene Namen, speichert im Erfolgsfall die Daten*/
/* 						dann im Array und bereitet Response-Packet vor		*/
/* Ergebnis:			Doppelter Name abgewiesen bzw. Daten hinzugefügt 	*/
/* Importparameter:		keine												*/
/* Exportparameter:		return-Wert int 0/USR_EXISTS						*/
/****************************************************************************/
int CheckData() {

	int i;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (strncmp(user_data[i].playername, loginrcv.data.playername, PLAYER_LENGTH) == 0)
			return USR_EXISTS;
	}
	loginrsp.head.type = LOGIN_RSP;
	loginrsp.head.length = htons(1);
	loginrsp.data.ID = (uint8_t) client_id;
	strncpy(user_data[client_id].playername, loginrcv.data.playername, strlen(loginrcv.data.playername));
	bzero(loginrcv.data.playername, sizeof(loginrcv.data.playername));
	printf("Player %s ist jetzt verbunden\n", user_data[client_id].playername);
	user_data[client_id].player_id = (uint8_t)client_id;
	user_data[client_id].score = htonl(0);
	printf("Client-ID : %d\t Score: %i\n", client_id, user_data[client_id].score);
	return 0;
}

/*						FUNKTIONS-HEADER								    */
/****************************************************************************/
/* Funktion: 			void SendError(int)									*/
/* Kurzbeschreibung:	erstellen und senden der Error-Packete				*/
/* Ergebnis:			Error-Packete werden dem Typ entsprechend versendet	*/
/* Importparameter:		int type 		//Typ der Error-Nachricht			*/
/* Exportparameter:		keine												*/
/****************************************************************************/
void SendError(int type) {

	char max_clients [64] = "Maximale Anzahl an Clients erreicht. Pech!";
	char usr_exists [64] = "Spieler mit diesem Benutzernamen bereits angemeldet.";

	if(type == USR_MAX) {
		loginerr.data.error.subtype = ERROR_FATAL;
		strncpy(loginerr.data.error.message, max_clients, strlen(max_clients) - 1);
		loginerr.head.type = ERRORWARNING;
		loginerr.head.length = htons(sizeof(loginerr.data.error.message) + 1);
		//close(client_socket[client_id]);
		send(client_socket[client_id], &loginerr, sizeof(loginerr.head) + ntohs(loginerr.head.length), 0);
	}
	if(type == USR_EXISTS) {
		loginerr.data.error.subtype = ERROR_WARNING;
		strncpy(loginerr.data.error.message,usr_exists, strlen(usr_exists)-1);
		loginerr.head.type = ERRORWARNING;
		loginerr.head.length = htons(sizeof(loginerr.data.error.message)+1);
		send(client_socket[client_id], &loginerr, sizeof(loginerr.head)+ntohs(loginerr.head.length), 0);
	}
}

