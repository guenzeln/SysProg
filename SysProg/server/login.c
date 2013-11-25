/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 * 
 * login.c: Implementierung des Logins
 */

#include "../common/rfc.h"
#include "login.h"
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
#define MAX_CLIENTS 5



int client_socket[MAX_CLIENTS];
static int client_id = 0;
ERROR errorPacket;
PACKET loginrcv, loginrsp, loginerr, players;
pthread_t c_thread[MAX_CLIENTS-1];
PLAYER playerlist[MAX_CLIENTS-1];
static struct sockaddr_in server, client;

void LoginInit() {

	int create_socket = ServerInit();
	ClientInit(create_socket);

}

void ClientInit(int _create_socket) {
	socklen_t addrlen;



	while (1) {
		do {
			addrlen = sizeof(client);
			client_socket[client_id] = accept(_create_socket, (struct sockaddr *) &client, &addrlen);
			if (client_socket[client_id] < 0)
				perror("Fehler beim Verbinden mit Socket: ");
			if (client_id == MAX_CLIENTS) {
					infoPrint("Maximale Anzahl an Clients erreicht! Client wird informiert...");
					errorPacket.subtype = 0;
					strncpy(errorPacket.message, "Maximale Anzahl an Clients erreicht. Pech!", 255);
					loginerr.head.type = 255;
					loginerr.head.length = htons(sizeof(errorPacket.message+1));
					send(client_socket[client_id], &loginerr, sizeof(loginerr), 0);
					close(client_socket[client_id]);
					client_id--;
			}
			else {
				infoPrint("Warte auf Login-Request vom Client...");
				if (recv(client_socket[client_id], &loginrcv.head, sizeof(loginrcv.head), 0) < 0)
					perror("Daten konnten nicht gelesen werden!");
				else {
					recv(client_socket[client_id], &loginrcv.data,loginrcv.head.length,0);
					infoPrint("Login-Daten erhalten\n");
					if(CheckData() == -1) {
						infoPrint("Spieler bereits angemeldet. Login abgewiesen.");
						send(client_socket[client_id], &loginerr, sizeof(loginerr.head)+sizeof(loginerr.data), 0);
					}
					else {
						send(client_socket[client_id], &loginrsp, sizeof(loginrsp.head)+sizeof(loginrsp.data), 0);
						infoPrint("Login response gesendet!");
						players.head.type = 6;
						players.head.length = htons(37);
						players.data.playerlist[0] = playerlist[client_id];
						send(client_socket[client_id] ,&players, sizeof(players.head)+sizeof(players.data), 0);
						if(pthread_create(&c_thread[client_id], NULL,(void *) &StartGame, NULL)!=0)
							infoPrint("Konnte keinen Client-Thread erzeugen");
						client_id++;
						infoPrint("Warte auf weitere Anfragen...");
					}
				}
			}
		} while (client_id <= MAX_CLIENTS);
		for (client_id = 0; client_id < MAX_CLIENTS; client_id++) {
			close (client_socket[client_id]);
		}
	}
}

int ServerInit() {

	int s_socket = socket(AF_INET, SOCK_STREAM, 0);

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
	if (listen(s_socket, MAX_CLIENTS) == -1) {
		perror("Fehler beim Eingehen von Verbindungsanfragen: ");
		exit(1);
	}
	printf("Socket wurde angelegt.\nWarte auf Anfragen...\n");
	return s_socket;
}


int CheckData() {

	int i;

	for (i = 0; i < MAX_CLIENTS - 1; i++) {
		if (strncmp(playerlist[i].playername, loginrcv.data.playername, 32) == 0) {
			errorPacket.subtype = 0;
			strncpy(errorPacket.message, "Spieler mit diesem Benutzernamen bereits angemeldet.", 255);
			loginerr.head.type = 255;
			loginerr.head.length = htons(sizeof(errorPacket.message + 1));
			return -1;
		}
	}

	strncpy(playerlist[client_id].playername, loginrcv.data.playername, 32);
	playerlist[client_id].player_id = client_id;
	playerlist[client_id].score = htons(0);
	loginrsp.head.type = 2;
	loginrsp.head.length = htons(1);
	loginrsp.data.ID = client_id;
	printf("Client-ID : %d", client_id);
	return 0;
}

	/*int loginThread(int client) {

		if (client == 1)
			return 0;
		else
			return -1;
	}*/
