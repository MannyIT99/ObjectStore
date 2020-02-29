#include "clientconnessi.h"

elemento *deleten(elemento **head, elemento **tail, char *nome) {
    elemento *scorri = *head, *prec = NULL;
    while(scorri != NULL) {
        if(strcmp(scorri -> nomeutente, nome) == 0) {
            if(prec == NULL) {
                (*head) = (*head) -> next;
                free(scorri);
                scorri = *head;
            } else if(scorri == (*tail)) {
                free(scorri);
                prec -> next = NULL;
                (*tail) = prec;
            } else {
                prec -> next = scorri -> next;
                free(scorri);
                scorri = prec -> next;
            }
            return *head;
        }
        prec = scorri;
        scorri = scorri -> next;
    }
    return *head;
}

elemento *inshead(elemento **head, elemento **tail, char *nome) {
    elemento *new = (elemento*) malloc(sizeof(elemento));
    new -> nomeutente = calloc(strlen(nome) + 1, sizeof(char));
    new -> nomeutente = strncpy(new -> nomeutente, nome, strlen(nome));
    new -> next = (*head);
    if((*tail) == NULL) {
        (*tail) = new;
    }
    (*head) = new;
    return *head;
}

void printl(elemento *head) {
    if(head != NULL) {
        write(1, head -> nomeutente, strlen(head -> nomeutente));
        write(1, " -> ", 4);
        printl(head -> next);
    }
}

int cercautente(elemento *head, char *nome) {
    while(head != NULL) {
        if(strcmp(head -> nomeutente, nome) == 0) {
            return 1;
        }
        head = head -> next;
    }
    return 0;
}
