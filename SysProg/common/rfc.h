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

struct NET_HEADER{

        uint8_t type;
        uint16_t length;
};
typedef struct NET_HEADER NET_HEADER;

struct LOGIN_REQUEST{

		struct NET_HEADER header;
		char* name;
};
typedef struct LOGIN_REQUEST LOGIN_REQUEST;

struct LOGIN_RESPONSE{

        struct NET_HEADER header;
        uint8_t client_id;

};
typedef struct LOGIN_RESPONSE LOGIN_RESPONSE;

struct CATALOG_REQUEST{

        struct NET_HEADER header;
};
typedef struct CATALOG_REQUEST CATALOG_REQUST;

struct CATALOG_RESPONSE{

        struct NET_HEADER header;
        char* filename;
};

struct CATALOG_CHANGE{

        struct NET_HEADER header;
        char* filename;

};
typedef struct CATALOG_CHANGE CATALOG_CHANGE;

struct PLAYER{

        char playername[32];
        uint32_t score;
        uint8_t player_id;

};
typedef struct PLAYER PLAYER;

struct PLAYERLIST{

        struct NET_HEADER header;
        struct PLAYER playerlist [6];
};
typedef struct PLAYERLIST PLAYERLIST;;

struct START_GAME{

        struct NET_HEADER header;
        char* filename;

};
typedef struct START_GAME START_GAME;

struct QUESTION_REQUEST{

        struct NET_HEADER header;
};
typedef struct QUESTION_REQUEST QUESTION_REQUEST;

struct QUESTION{

        struct NET_HEADER header;
        QuestionMessage qmsg;
};
typedef struct QUESTION QUESTION;

struct QUESTION_RESULT{

        struct NET_HEADER header;
        Question question;

};
typedef struct QUESTION_RESULT QUESTEION_RESULT;

struct GAME_OVER{

        struct NET_HEADER header;
        uint8_t rank;
};
typedef struct GAME_OVER GAME_OVER;

struct ERROR_WARNING{

        struct NET_HEADER h;
        uint8_t subtype;
        char* message;

};
typedef struct ERROR_WARNING ERROR_WARNING;

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
