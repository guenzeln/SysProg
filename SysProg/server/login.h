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

void Client_Init(int client_socket){};
int Server_Init (int _port);
void Login_Init(int port);

/* ... */

#endif
