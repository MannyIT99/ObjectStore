#if !defined(HEADERSERVER_H)
#define HEADERSERVER_H

#define SER_MENO1_BLOCK(a, b, c) \
    if((a = b) == -1)  { write(2, c, strlen(c)); }
#define SER_NULL_BLOCK(a, b) \
    if((a = b) == NULL) { write(2, SER_MAF, strlen(SER_MAF)); exit(EXIT_FAILURE); }
#define SER_MENO1_BREAK(a, b, c) \
    if((a = b) == -1)  { write(2, c, strlen(c)); }
#define SER_CHECK_NULL(a, b, c) \
    if((a = b) == NULL) { write(2, c, strlen(c)); break; }
#define SER_CHECK_NULL_AND_FREE1(a, b, c, d) \
    if((a = b) == NULL) { write(2, c, strlen(c)); free(d); break; }
#define SER_CHECK_NULL_AND_FREE2(a, b, c, d, e) \
    if((a = b) == NULL) { write(2, c, strlen(c)); free(d); free(e); break; }
#define SER_MENO1_BREAK_AND_FREE1(a, b, c, d) \
    if((a = b) == -1) { write(2, c, strlen(c)); free(d); break; }
#define SER_MENO1_BREAK_AND_FREE2(a, b, c, d, e) \
    if((a = b) == -1) { write(2, c, strlen(c)); free(d); free(e); break; }
#define SER_MENO1_BREAK_AND_FREE3(a, b, c, d, e, f) \
    if((a = b) == -1) { write(2, c, strlen(c)); free(d); free(e); free(f); break; }
#define SER_NEZERO_BLOCK(a, b, c, d) \
if((a = b) != 0) { write(2, c, strlen(c)); d; }

//Allocazione memoria: fallita
#define SER_MAF "[Errore] Allocazione memoria: fallita\n"

//SC socket fallita
#define SER_SC_SOCKF "[Errore] SC socket: fallita\n"

//SC bind fallita
#define SER_SC_BINDF "[Errore] SC bind: fallita\n"

//SC listen fallita
#define SER_SC_LISF "[Errore] SC listen: fallita\n"

//SC accept fallita
#define SER_SC_ACCPF "[Errore] SC accept: fallita\n"

//SC mkdir fallita
#define SER_SC_MKDIRF "[Errore] SC mkdir: fallita\n"

//data è la directory madre delle directory dei singoli utenti
#define DIRDATA "data/"

//Errore della funzione addutente
#define SER_NOADD "[KO] Errore del server nell'inserimento dell'utente\n"

//Esiste un altro utente connesso con lo stesso nome
#define SER_EXISTS "[KO] Utente già connesso\n"

//Errore di comunicazione
#define SER_COMF "[Errore] Impossibile continuare a comunicare col client\n"

//Accesso alla directory: fallito
#define SER_DIRACCF "[KO] Accesso alla directory: fallito\n"

//Accesso alla directory fallit
#define SER_DIRACCF_LOC "[Errore] Accesso alla directory: fallito\n"

//Creazione o accesso alla directory: riuscito
#define SER_REGOK "[OK] Registrazione riuscita\n"

//Disconnessione riuscita
#define SER_DISCOK "[OK] Disconnessione riuscita\n"

//Errore nella chiusura della directory
#define SER_SC_CLODIRF "[Errore] Chiusura della directory fallita\n"

//Nome oggetto = NULL
#define SER_OBJNN "[Errore] L'oggetto non può avere nome \"nullo\"\n"

//Lunghezza oggetto (inizialmente stringa) = NULL
#define SER_DLNN "[Errore] La dimensione dell'oggetto non può essere nulla\n"

//Se la scrittura dei dati è parziale, il file viene rimosso
#define SER_PARFILE "[KO] Scrittura file: parziale. Il file è stato cancellato\n"

//Se la fopen fallice, errore
#define SER_OPFERR "[KO] Errore nell'apertura del file. Riprovare\n"

//Rimozione del file fallita
#define SER_RIMFERRLOC "[Errore] Rimozione del file non completata con successo\n"

//La memoizzazione è stata completata
#define SER_MEMOBJOK "[OK] Memorizzazione dati nel file: effettuata con successo\n"

//Trovato file con stesso nome
#define SER_ADVCLI_ITFILE "Esiste già un file con questo nome\n"

//Non trovato file con stesso nome
#define SER_ADVCLI_NOITFILE "Non esiste un file con questo nome\n"

//Rimozione file non completata
#define SER_RIMFERR "[KO] Rimozione del file: non completata\n"

//Rimozione file riuscita
#define SER_RIMFOK "[OK] Il file è stato rimosso correttamente\n"

//File non esistente
#define SER_FDE "[KO] Il file non esiste\n"

//Errore nella operazioni di seek
#define SER_SEEKFERR "[KO] Errore nella lettura del file\n"

//Non ammessi valori nulli
#define SER_NN "[KO] Non ammessi valori nulli\n"

//Errore della ftw
#define SER_ERRFTW "[Errore] Impossibile calcolare la size totale dello store\n"

//Errore nel calcolo del numero di oggetti presenti nello store
#define SER_ERRNOBJ "[Errore] Impossibile calcolare il numero di oggetti presenti\n"

//Messaggi dopo SIGUSR1
#define SER_RESO "*** RESOCONTO STATO DELLO STORE ***"
#define SER_NCLICON "\nNumero di client connessi: "
#define SER_NOBJIN "\nNumero di oggetti presenti nello store: "
#define SER_SZSTO "\nSize totale dello store: "
#define SER_RESOTERM "\n\n-------------------------------------------------------------------------\n\n"

//Errore delle funzioni per la gestione dei segnali
#define SER_ERRSIG "[Errore] Gestione dei segnali: non settata correttamente\n"

//Errore nel settare l'ambiente per thread detached
#define SER_ERRENVTH "[Errore] Impossibile settare ambiente per thread detached\n"

#endif /* HEADERSERVER_H */
