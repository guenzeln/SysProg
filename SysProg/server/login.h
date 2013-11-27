/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 * 
 * login.h: Header für das Login
 */

#ifndef LOGIN_H
#define LOGIN_H

void ClientInit(int);
int ServerInit ();
void LoginInit();
int CheckData();
void SendError(int);


/* ... */

#endif
