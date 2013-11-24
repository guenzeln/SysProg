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
int client_id = 0;
packetError errorPacket;
packet loginrcv, loginrsp;
pthread_t c_thread[MAX_CLIENTS-1];
packetData playerlist[MAX_CLIENTS-1];
static struct sockaddr_in server, client;

void LoginInit() {

	int create_socket = ServerInit();
	create_socket = ServerInit();
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
					errorPacket.head.type = 255;
					//errorPacket.err.message = "Maximale Anzahl an Clients erreicht. Pech!";
					errorPacket.head.length = sizeof(errorPacket.err.message+1);
					errorPacket.err.subtype = 0;
					send(client_socket[client_id], &errorPacket, sizeof(errorPacket), 0);
					close(client_socket[client_id]);
			}
			else {
				infoPrint("Warte auf Login-Request vom Client...");
				if (recv(client_socket[client_id], &loginrcv, sizeof(loginrcv),0) < 0)
					perror("Daten konnten nicht gelesen werden!");
				else {
					infoPrint("Login-Daten erhalten\n");
					if(CheckData() == -1)
						infoPrint("Spieler bereits angemeldet. Login abgewiesen.");
					else
						/*if(pthread_create(&c_thread[client_id], NULL, &StartGame, NULL)!=0)
							infoPrint("Konnte keinen Client-Thread erzeugen");*/
						client_id++;
						infoPrint("Warte auf weitere Anfragen...");

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

	int check = 1;
	int true = 1;
	int i;
	int *value;

	while (check) {
		for (i = 0; i < MAX_CLIENTS - 1; i++) {
			if (playerlist[i].playername == loginrcv.data.playername) {
				errorPacket.head.type = 255;
				//errorPacket.err.message = "Spieler mit diesem Benutzernamen bereits angemeldet.";
				errorPacket.head.length = sizeof(errorPacket.err.message + 1);
				errorPacket.err.subtype = 0;
				send(client_socket[client_id], &errorPacket, sizeof(errorPacket), 0);
				i = MAX_CLIENTS - 1;
				true = 0;
				return -1;
			}
		}
		if (true) {
			playerlist[client_id] = loginrcv.data;
			playerlist[client_id].ID = client_id;
			loginrsp.head.type = 2;
			loginrsp.head.length = 1;
			loginrsp.data.ID = client_id;
			send(client_socket[client_id], &loginrsp, sizeof(loginrsp), 0);
			infoPrint("Login response gesendet!");
			check = 0;
			return 1;
		}
	}
}
	/*int loginThread(int client) {

		if (client == 1)
			return 0;
		else
			return -1;
	}*/
