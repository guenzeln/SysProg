/****************************************************************************/
/*Export-Schnittstelle des login-Moduls									    */
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*Makros und Konstanten														*/
/*--------------------------------------------------------------------------*/
#ifndef LOGIN_H
#define LOGIN_H
#define USR_MAX 0
#define USR_EXISTS 1
#define MAX_CLIENTS 4

/*--------------------------------------------------------------------------*/
/*Funktionsprototypen	    												*/
/*--------------------------------------------------------------------------*/
void LoginInit();
int ServerInit ();
void ClientInit(int);
int CheckData();
void SendError(int);

#endif
