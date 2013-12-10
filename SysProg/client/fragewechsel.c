/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Client
 * 
 * fragewechsel.c: Implementierung des Fragewechsel-Threads
 */

#include "fragewechsel.h"
#include "common/rfc.h"

void changeQuestion(int* sock){

		pthread_mutex_lock(&lock);
		sleep(3);
		PACKET init=createQuestionRequest();
		write(*sock,&init,HEAD_SIZE);
		printf("YOLO\n");

		pthread_mutex_unlock(&lock);

}
