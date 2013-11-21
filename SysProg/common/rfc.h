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


union packetData{
	char playername[32];
	char filename[32];
	uint8_t ID;
};

struct packet{
	struct header head;
	union packetData data;

};
#pragma pack(0)


#endif
