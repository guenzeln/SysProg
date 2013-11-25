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
#include "../common/question.h"

int PORT;



#pragma pack(1)



struct PLAYER{

        char playername[32];
        uint32_t score;
        uint8_t player_id;

};
typedef struct PLAYER PLAYER;

struct client_data{
        int clientID;
        int sock;
        //struct sockaddr_storage addr;
        //socklen_t addrlen;
        char *name;
        int currentQuestion;
        int points;
        struct QUESTION *questionsPTR;
        // 1 - yes, 0 - No
        int clientListChanged;
        int catalogSelected;
        int stat;
};
typedef struct client_data CLIENTDATA;

struct header {
	uint8_t type;
	uint16_t length;
};
typedef struct header HEADER;

struct error {
	uint8_t subtype;
	char message [255];
};
typedef struct error ERROR;

union packetData{
	char playername[32];
	char filename[32];
	uint8_t ID;
	uint8_t rank;
	PLAYER playerlist [4];
	ERROR error;
	QuestionMessage qmessage;
	Question question;
};
typedef union packetData PACKET_DATA;

struct packet{
	HEADER head;
	PACKET_DATA data;

};
typedef struct packet PACKET;


#pragma pack(0)

PACKET createLoginRe(char[]);


#endif
