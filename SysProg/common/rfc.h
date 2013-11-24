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
#define LOGINREQUEST     1
#define LOGINRESPONSE    2
#define CATALOGREQUEST   3
#define CATALOGRESPONSE  4
#define CATALOGCHANGE    5
#define PLAYERLIST_RFC   6
#define STARTGAME        7
#define QUESTIONREQUEST  8
#define QUESTION_RFC     9
#define QUESTIONANSWERED 10
#define QUESTIONRESULT   11
#define GAMEOVER         12
#define ERRORWARNING     255
#include <stdint.h>

//#include "common/question.h"

#pragma pack(1)
struct header {
	uint8_t type;
	uint16_t length;
};
typedef struct header header;


union packetData{
	char playername[32];
	char filename[32];
	//unsigned int score;
	uint8_t ID;
};
typedef union packetData packetData;

struct packet{
	struct header head;
	union packetData data;

};
typedef struct packet packet;

struct error {
	uint8_t subtype;
	char message [];
};
typedef struct error error;

struct packetError {
	header head;
	error err;
};
typedef struct packetError packetError;

#pragma pack(0)


#endif
