#if !defined(ACCESSO_H)
#define ACCESSO_H

#include "conn.h"
#include "headerclient.h"

#define ACC_OK 0 // Successo
#define ACC_MAF 1 //Allocazione memoria fallita
#define ACC_SOCKF 2 //SC socket fallita
#define ACC_CONF 3 //SC connect fallita
#define ACC_COMF 4 //Comunicazione fallita
#define ACC_NOADD 5 //Utente non aggiunto alla lista dei connessi
#define ACC_EXISTS 6 //Esiste già una connessione con quel nome
#define ACC_DIRACCF 7 //Accesso alla directory: fallito
#define ACC_OPFERR 8 //Apertura file errata
#define ACC_PARFILE 9 //Scrittura parziale (store)
#define ACC_RIMFERR 10 //Errore nella rimozione del file
#define ACC_FDE 11 //Il file non esiste
#define ACC_OPFERR_RET "[KO] Errore nell'apertura del file. Riprovare\n"
#define ACC_SEEKFERR_RET "[KO] Errore nella lettura del file\n"
#define ACC_COMF_RET "[Errore] Comunicazione col server: fallita\n"
#define ACC_MAF_RET "[Errore] Allocazione di memoria fallita\n"
#define ACC_FDE_RET "[KO] Il file non esiste\n"

//MACRO
#define ACC_CHECKNULL(a, b, c) \
    if((a = b) == NULL) { return c; }
#define ACC_CHECKNULL_AND_FREE1(a, b, c, d) \
    if((a = b) == NULL) { free(c); return d; }
#define ACC_CHECKNULL_AND_FREE2(a, b, c, d, e) \
    if((a = b) == NULL) { free(c); free(d); return e; }
#define ACC_MENO1(a, b, c) \
    if((a = b) == -1) { return c; }
#define ACC_MENO1_AND_FREE1(a, b, c, d) \
    if((a = b) == -1) { free(c); return d; }
#define ACC_MENO1_AND_FREE2(a, b, c, d, e) \
    if((a = b) == -1) { free(c); free(d); return e; }
#define ACC_MENO1_AND_FREE3(a, b, c, d, e, f) \
    if((a = b) == -1) { free(c); free(d); free(e); return f; }

typedef struct {
  int fd;
  struct sockaddr_un addr;
} socketnode_t;

/*
   VALORI DI RITORNO DELLE FUNZIONI
   0: True (successo)
   Valori diversi da 0, corrsisponderanno a errori diversi (fallimento)
*/

int os_connect(char *name);
int os_store(char *name, void *block, size_t len);
void *os_retrieve(char *name);
int os_delete(char *name);
int os_disconnect();

#endif /* ACCESSO_H */
