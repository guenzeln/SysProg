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

void ClientInit(int client_socket);
int ServerInit (int _port);
void LoginInit(int port);
int CheckData();

/* ... */

#endif
