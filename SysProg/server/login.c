/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 * 
 * login.c: Implementierung des Logins
 */

#include "login.h"
#include "common/rfc.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_CLIENTS 5
#define BLOCKSIZE 1024


char buffer[BLOCKSIZE];


size_t MESSAGE = 64;
size_t DATA = 1024;
pthread_t CLIENT[MAX_CLIENTS-1];
packetData playerlist[MAX_CLIENTS-1];
static struct sockaddr_in server_in;

void LoginInit(int port) {
	int create_socket;
	create_socket = ServerInit(port);
	ClientInit(create_socket);

}

void ClientInit(int _create_socket) {

	socklen_t addrlen;
	char message[64] = "Verbindung hergestellt. Bitte Login-Daten eingeben.";
	packetError errorPacket;
	packet loginrcv, loginrsp;
	int client_socket[MAX_CLIENTS]; //asdasdsdasdsad
	int client_id;

	while (1) {
		do {
			client_socket[client_id] = accept(_create_socket, (struct sockaddr *) &server_in, &addrlen);
			if (client_socket[client_id] < 0)
				perror("Fehler beim Verbinden mit Socket: ");
			if (client_id == MAX_CLIENTS) {
				printf("Maximale Anzahl an Clients erreicht! Client wird informiert...");
				errorPacket.err.message = "Maximale Anzahl an Clients erreicht. Pech!";
				errorPacket.head.length = sizeof(errorPacket.error.message+1);
				errorPacket.err.subtype = 0;
				send(client_socket[client_id], errorPacket, sizeof(errorPacket), 0);
				close(client_socket[client_id]);
			}
			else {
				if (send(client_socket[client_id], message, sizeof(message),0) < 0)
					perror("Datenverlust beim Senden");
				else {
					printf("Warte auf Login-Request vom Client...");
					if (recv(client_socket[client_id], (packet) &loginrcv, sizeof(loginrcv),0) < 0)
						perror("Daten konnten nicht gelesen werden!");
					else {
						int check = 1;
						int true = 1;
						int i;
						while(check){
							for(i = 0; i < MAX_CLIENTS-1; i++) {
								if( playerlist[i].playername == loginrcv.data.playername){
									errorPacket.err.message = "Player bereits angemeldet.";
									errorPacket.head.length = sizeof(errorPacket.error.message+1);
									errorPacket.err.subtype = 0;
									send(client_socket[client_id], errorPacket, sizeof(errorPacket), 0);
									i = MAX_CLIENTS-1;
									true = 0;
								}
							}
							if(true) {
								playerlist[client_id] = packet.packetData;
								playlist[client_id].ID = client_id;
								loginrsp.head.type = 2;
								loginrsp.head.length = 1;
								loginrsp.data.ID = client_id;

								send(client_socket[client_id], loginrsp, sizeof(loginrsp), 0);
								client_id++;
								check = 0;
							}
						}
					}
				}
			}
		} while (client_id <= MAX_CLIENTS);
	}

	int ServerInit(int _port) {

		int s_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (s_socket == -1) {
			perror("Socket kann nicht erstellt werden: ");
			exit(1);
		}

		printf("Socket wurde angelegt.\nWarte auf Anfragen...\n");
		server_in.sin_family = AF_INET;
		server_in.sin_addr.s_addr = INADDR_ANY;
		server_in.sin_port = htons(_port);

		if (bind(s_socket, (struct sockaddr *) &server_in, sizeof(server_in))
				== -1) {
			perror("Socket konnte nicht an die Adresse gebunden werden: ");
			exit(1);
		}
		if (listen(s_socket, MAX_CLIENTS) == -1) {
			perror("Fehler beim Eingehen von Verbindungsanfragen: ");
			exit(1);
		}

		return s_socket;
	}

	int loginThread(int client) {

		if (client == 1)
			return 0;
		else
			return -1;
	}
