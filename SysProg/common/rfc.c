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

void SendMessage(int type, int socket) {

	if (type==1) {
		packetData LoginRequest;
	}
}
