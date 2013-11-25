/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Gemeinsam verwendete Module
 * 
 * rfc.c: Implementierung der Funktionen zum Senden und Empfangen von
 * Datenpaketen gemäß dem RFC
 */

#include <sys/types.h>
#include <sys/socket.h>
#include "rfc.h"

PACKET createLoginRe(char *name){
		PACKET init;
		init.head.type = 1;

		init.head.length = htons(strlen(name));
		strncpy(init.data.playername, name, strlen(name));
		printf("%d \n", ntohs(init.head.length));
		printf("%d  %d  %s \n", init.head.type, ntohs(init.head.length),
				init.data.playername);
		return init;
}

/*void SendMessage(int type, int socket) {


	if (type==1) {
		packetData LoginRequest;

void SendPacket (int type, int socket) {

	switch(type) {
	case '2':

	}
}
*/
