/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Gemeinsam verwendete Module
 * 
 * rfc.h: Definitionen für das Netzwerkprotokoll gemäß dem RFC
 */

#ifndef RFC_H
#define RFC_H

#include "common/question.h"

#pragma pack(1)
struct header {
	uint8_t type;
	uint16_t length;
};
//typedef struct header header;


union packetData{
	char playername[32];
	char filename[32];
	uint8_t ID;
};
//typedef union packetData packetData;

struct packet{
	struct header head;
	union packetData data;

};
//typedef struct packet packet;

struct error {
	uint8_t subtype;
	char message [];
};
//typedef struct error error;

struct packetError {
	struct header head;
	struct error err;
};
//typedef union packetError packetError;

#pragma pack(0)


#endif
