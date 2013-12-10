/****************************************************************************/
/*							listener-MODULHEADER	  							*/
/****************************************************************************/
/* Projekt:				Systemprogrammierung - Multiplayquiz				*/
/* Modulname:			listener.c											*/
/* Modeltyp				funktionell											*/
/* Sprache/Compiler:	C - gcc											    */
/* Kurzbeschreibung:	Das Modul realisiert den Listener des clients	    */
/* Ersteller:			Gruppe 08											*/
/* Letzte Änderung 		27.11.12											*/
/****************************************************************************/
/*--------------------------------------------------------------------------*/
/*Includes des listener Moduls       										*/
/*--------------------------------------------------------------------------*/
#include "../common/util.h"
#include "gui/gui_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "../common/rfc.h"
#include <arpa/inet.h>
#include "fragewechsel.h"


int listenerMain(int* sock){ //EinstiegsFunktion des Listeners

		PACKET answer,empty,init;
		int i=0;
		memset(&empty,0,sizeof(empty)); //empty wird Komplett auf 0 gesetzt
		int CatCount=0;
		int spielphase=0;
		pthread_t fragewechsel;


		infoPrint("Vor WHILE ");
		printf("socket: %d\n",*sock);

/*--------------------------------------------------------------------------*/
/*Auf Datenpakete  warten und auswerten  									*/
/*--------------------------------------------------------------------------*/
		while(spielphase==0){
			while(spielphase==0){
				read(*sock, &answer.head, 3);
				read(*sock, &answer.data, ntohs(answer.head.length));
				switch (answer.head.type) {

					case PLAYERLIST:
							printf("Spielerliste\n");
							int player_count=ntohs(answer.head.length)/37;
							printf("Spieler: %d\n",player_count);
							preparation_clearPlayers();
							for(i=0;i<player_count;i++){
								char name[32];
								strncpy(name,answer.data.playerlist[i].playername,32);
								preparation_addPlayer(name);
							}
							answer=empty;
							break;

					case CATALOGRESPONSE:
							printf("Cataloge\n");
							if(answer.head.length==0){
								printf("Auflistung der Kataloge abgeschlossen. Anzahl: %d \n",CatCount);
							}else{
								preparation_addCatalog(answer.data.filename);
								CatCount++;
							}
							answer=empty;
							break;

					case CATALOGCHANGE:
							printf("Katalogauswahl geändert\n");
							answer.data.filename[answer.head.length]='\0';
							printf("katalog: %s \n",answer.data.filename);
							preparation_selectCatalog(answer.data.filename);
							answer=empty;
							break;

					case STARTGAME:
							printf("Spiel gestartet BRO \n");
							spielphase=1;
							preparation_hideWindow();
							break;

					case ERROR:

								switch(answer.data.error.subtype){

									case ERROR_WARNING:		infoPrint("Warning");
												answer.data.error.message[ntohs(answer.head.length)]='\0';
												printf("fehler: %s \n",answer.data.error.message);
												answer=empty;
												break;

									case ERROR_FATAL:
												answer.data.error.message[ntohs(answer.head.length)]='\0';
												printf("fehler:%s \n",answer.data.error.message);
												return 1;

									default:
												infoPrint("ungültiger Subtype");
												break;
								}
					}

			}
			if(spielphase==1){
				pthread_mutex_lock(&lock);
				if(pthread_create(&fragewechsel,NULL,(void*)&changeQuestion,&sock)!=0){
					perror("Konnte keinen fragewechsel-Thread erzeugen: ");
					return 10;
				}
				pthread_mutex_unlock(&lock);
			}
			while(spielphase==1){
				pthread_mutex_lock(&lock);
				printf("spielphase\n");
				game_showWindow();
				init=createQuestionRequest();
				write(*sock,&init,HEAD_SIZE);

				while((read(*sock, &answer.head, HEAD_SIZE)>=0)){

					read(*sock, &answer.data, ntohs(answer.head.length));

					switch(answer.head.type){

						case QUESTION:
								infoPrint("frage erhalten");

								printf("doof %s \n",answer.data.qmessage.question);
								game_setQuestion(answer.data.qmessage.question);

								for(i=0;i<NUM_ANSWERS;i++){
									game_setAnswer(i,answer.data.qmessage.answers[i]);
								}
								char message[32]={"Du hast "};
								message[8]=(char)answer.data.qmessage.timeout;
								sprintf(message+8,"%i",answer.data.qmessage.timeout);
								char message2[10]={"s Zeit"};
								message2[6]='\0';
								strncpy(message+strlen(message),message2,strlen(message2));
								game_setStatusText(message);
								answer=empty;
								break;

						case PLAYERLIST:
								printf("Spielerliste\n");
								int player_count=ntohs(answer.head.length)/37;
								printf("Spieler: %d\n",player_count);
								for(i=0;i<player_count;i++){
									char name[32];
									strncpy(name,answer.data.playerlist[i].playername,32);
									game_setPlayerName(i+1,name);
									game_setPlayerScore(i+1,ntohl(answer.data.playerlist[i].score));
									printf("score: %d \n",ntohl(answer.data.playerlist[i].score));
								}

								answer=empty;
								break;
						case QUESTIONRESULT :
								infoPrint("QUESTIONRESULT");
								if(answer.data.qResult.timeout!=0){
									game_markAnswerCorrect(answer.data.qResult.correct);
									game_setStatusText("Zeit abgelaufen");
								}
								if(answer.data.qResult.correct==AnswerClicked){
									game_markAnswerCorrect(answer.data.qResult.correct);
									game_setStatusText("Stimmt");
								}else{
									game_markAnswerWrong(AnswerClicked);
									game_markAnswerCorrect(answer.data.qResult.correct);
									game_setStatusText("FALSCH!");
								}

								answer=empty;
								changeQuestion(&sock);
								pthread_mutex_unlock(&lock);
								break;
					}


				}






			}



		}
		return 0;
}
