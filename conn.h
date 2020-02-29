#if !defined(CONN_H)
#define CONN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

//Lunghezza massima dell'header:
#define LENHEADER 276
//255 - Massima lunghezza dei file unix
#define LENMAXUNIX 255
//8 - Massima lunghezza di un comando della libreria prefedinita
#define LENMAXCOMM 8
//6 - Massima lunghezza che indica la dimensione del file (100000)
#define LENMAXDIM 6
//Spazi e \n
//Lunghezza dei messaggi che indicano l'esito delle operazioni
#define LENGENMSG 512
//Massima lunghezza dei dati
#define MAXLENDATA 100000

#define SOCKNAME "objstore.sock"
#define SEPST " \n" //Secondo paramtero strtok

/* COMANDI HEADER */
#define REGISTER "REGISTER"
#define STORE "STORE"
#define RETRIEVE "RETRIEVE"
#define DELETE "DELETE"
#define LEAVE "LEAVE"

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { write(1, e, strlen(e));exit(errno); }
#define CHECKNULL(r,c,e) \
    if ((r=c)==NULL) { write(1, e, strlen(e));exit(errno); }
#define CHECKNULL_NOBLOCK(r,c,e) \
if ((r=c)==NULL) { write(1, e, strlen(e));continue; }
#define MORE_CHECKNULL(r,c,e) \
if ((r=c)!=NULL) { write(1, e, strlen(e));continue; }

#if !defined(BUFSIZE)
#define BUFSIZE 256
#endif

/**
 * tipo del messaggio
 */
typedef struct msg {
    int len;
    char *str;
} msg_t;


static inline int readn(long fd, void *buf, size_t size) {
    int r;
    char *bufptr = (char*)buf;
	if ((r = read((int)fd, bufptr, size)) == -1) {
	    if (errno != EINTR)
            return -1;
	}
	if(r == 0)
        return 0; //Gestione chiusura socket
    return size;
}

static inline int writen(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
	if ((r = write((int)fd, bufptr, size)) == -1) {
	    if (errno != EINTR)
            return -1;
	}
	if(r == 0)
        return 0; //Gestione chiusura socket
    return 1;
}


#endif /* CONN_H */
