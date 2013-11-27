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
#define USR_MAX 0
#define USR_EXISTS 1
#define LOGIN_RSP 2
#define PLAYERLIST 3
#define MAX_CLIENTS 4

static PACKET loginrcv, loginrsp, loginerr, players;
static PLAYER user_data[MAX_CLIENTS];
static struct sockaddr_in server, client;
static int client_socket[MAX_CLIENTS];
static int client_id = 0;
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

void LoginInit() {
	int create_socket = ServerInit();
	ClientInit(create_socket);

}

void ClientInit(int _create_socket) {
	socklen_t addrlen;
	pthread_t c_thread[MAX_CLIENTS];
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
				else {	//Login-Request annehmen
					recv(client_socket[client_id], &loginrcv.data,loginrcv.head.length,0);
					infoPrint("Login-Daten erhalten\n");
					if(CheckData() == -1) {
						infoPrint("Spieler bereits angemeldet. Login abgewiesen.");
						SendError(USR_EXISTS);
					}
					else {
						send(client_socket[client_id], &loginrsp,4, 0);
						players.head.type = 6;
						players.head.length = htons((client_id+1)*37);
						for (i = 0; i <= client_id; i++) {
							players.data.playerlist[i].player_id = user_data[i].player_id;
							strncpy(players.data.playerlist[i].playername, user_data[i].playername, 32);
							players.data.playerlist[i].score = user_data[i].score;
						}
						for (i = 0; i <= client_id; i++) {
							send(client_socket[i], &players, sizeof(players.head)+ntohs(players.head.length), 0);
						}
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
	if (listen(s_socket, MAX_CLIENTS+1) == -1) {
		perror("Fehler beim Eingehen von Verbindungsanfragen: ");
		exit(1);
	}
	infoPrint("Socket wurde angelegt.\nWarte auf Anfragen...\n");
	return s_socket;
}

/*Überprüfe Login-Daten: Wenn Name schon vorhanden, return -1. Andernfalls, return 0*/
int CheckData() {
	int i;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (strncmp(user_data[i].playername, loginrcv.data.playername, 32) == 0)
			return -1;
	}
	loginrsp.head.type = 2;
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

void SendError(int type) {
	char max_clients [64] = "Maximale Anzahl an Clients erreicht. Pech!";
	char usr_exists [64] = "Spieler mit diesem Benutzernamen bereits angemeldet.";

	if(type == 0) {
		loginerr.data.error.subtype = 1;
		strncpy(loginerr.data.error.message, max_clients, strlen(max_clients) - 1);
		loginerr.head.type = 255;
		loginerr.head.length = htons(sizeof(loginerr.data.error.message) + 1);
		//close(client_socket[client_id]);
		send(client_socket[client_id], &loginerr, sizeof(loginerr.head) + ntohs(loginerr.head.length), 0);
	}
	if(type == 1) {
		loginerr.data.error.subtype = 0;
		strncpy(loginerr.data.error.message,usr_exists, strlen(usr_exists)-1);
		loginerr.head.type = 255;
		loginerr.head.length = htons(sizeof(loginerr.data.error.message)+1);
		send(client_socket[client_id], &loginerr, sizeof(loginerr.head)+ntohs(loginerr.head.length), 0);
	}

}

