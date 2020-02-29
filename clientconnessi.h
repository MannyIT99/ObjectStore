#if !defined(CLIENTCONNESSI_H)
#define CLIENTCONNESSI_H

#include "conn.h"
#include <pthread.h>

typedef struct elemento_ {
    char *nomeutente;
    struct elemento_ *next;
} elemento;

int cercautente(elemento *head, char *nome) ;
elemento *deleten(elemento **head, elemento **tail, char *nome);
elemento *inshead(elemento **head, elemento **tail, char *nome);
void printl(elemento *head);

#endif /* CLIENTCONNESSI_H */
