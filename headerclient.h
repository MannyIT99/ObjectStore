#if !defined(HEADERCLIENT_H)
#define HEADERCLIENT_H

#define CLI_CHECKNULL_AND_FREE1(a, b, c, d, e) \
    if((a = b) == NULL) { free(c); fprintf(stderr, "%s", d); e++; break; }
#define CLI_CHECKNULL_AND_FREE2(a, b, c, d, e, f) \
    if((a = b) == NULL) { free(c); free(d); fprintf(stderr, "%s", e); f++; break; }

//Numero di iterazioni del test 1
#define IT_TEST1 20

//Numero di argomenti da passare al client (Compreso il nome)
#define NPARAM 3

//Test non inserito
#define TESTND "Errore - Non definita"

//Tipo di test #1
#define TEST1 1
#define TEST1_STR "TEST1"

//Tipo di test #2
#define TEST2 2
#define TEST2_STR "TEST2"

//Tipo di test #2
#define TEST3 3
#define TEST3_STR "TEST3"

//Stringa di "benvenuto"
#define CLI_TITLE "CLIENT AVVIATO\n"

//Numero di parametri diverso da 3
#define CLI_NE3 "[Errore] \n\
Richiesti i seguenti parametri: \n\
  - Nome da utilizzare per la connessione con l'object_store\n\
  - Numero compreso tra 1 e 3 (Estremi inclusi)\n"

//Il numero di test non è compreso tra 1 e 3
#define CLI_TNE "[Errore] \n\
Il secondo parametro deve essere un intero \n\
compreso tra 1 e 3 (Estremi inclusi)\n"

//Connessione al server effettuata correttamente
#define CLI_REGOK "[OK] Registrazione riuscita\n"

//Allocazione di memoria fallita
#define CLI_MAF "[Errore] Allocazione di memoria fallita\n"

//Fallimento della chiamata di sistema socket
#define CLI_SC_SOCKF "[Errore] SC socket: fallita\n"

//Fallimento della chiamata di sistema connect
#define CLI_SC_CONF "[Errore] SC connect: fallita\n"

//Fallimento della comunicazione col server (conn.h)
#define CLI_COMF "[Errore] Comunicazione col server: fallita\n"

//Lettura/Scrittura fallita
#define CLI_LOCOMF "[Errore] Comunicazione fallita\n"

//Disconnessione riuscita
#define CLI_DISCOK "[OK] Disconnessione riuscita\n"

//Errore durante la disconnessione
#define CLI_DISCNOK "[Errore] Disconnessione forzata\n"

//Errore durante l'accesso alla directory
#define CLI_DIRACCF "[KO] Accesso alla directory: fallito\n"

//Esiste un altro utente connesso con lo stesso nome
#define CLI_EXISTS "[KO] Utente già connesso\n"

//Errore nell'inserimento dell'utente
#define CLI_NOADD "[KO] Errore del server nell'inserimento dell'utente\n"

//Errore di comando
#define CLI_STATT "[Errore] Atteso comando \"STORE\"\n"

//Valore nullo (NULL) non ammesso
#define CLI_NN "[Errore] Non ammessi valori nulli\n"

//Gli oggetti non possono avere lunghezza = 0
#define CLI_0OBJ "[Errore] Gli oggetti non possono avere lunghezza uguale a zero\n"

//Store completata
#define CLI_MEMOBJOK "[OK] Memorizzazione dati nel file: effettuata con successo\n"

//Errore nell'apertura del file
#define CLI_OPFERR "[KO] Errore nell'apertura del file. Riprovare\n"

//Scrittura parizale del file
#define CLI_PARFILE "[KO] Scrittura file: parziale. Il file è stato cancellato\n"

//Il server ha trovato già un file con quel nome
#define CLI_ADV_ITFILE "Esiste già un file con questo nome\n"

//Utile per la comunicazione col server nel caso non abbia trovato un file con lo stesso nome
#define CLI_ADV_NOITFILE "Non esiste un file con questo nome\n"

//Messaggio che chiede la sovrascrittura del file
#define CLI_ADV_ITFILE_REQ "Esiste già un file con questo nome\n \
  Digitare 'si' per sovrascriverlo\n \
  Digitare 'no' per scrivere aggiungere il nuovo contenuto al vecchio\n \
  Risposta: "

//Errore nella cancellazione del file
#define CLI_RIMFERR "[KO] Rimozione del file: non completata\n"

//Il file da cancellare, non esiste
#define CLI_FDE "[KO] Il file non esiste\n"

//Cancellazione file completata
#define CLI_RIMFOK "[OK] Il file è stato rimosso correttamente\n"

//Errore di comando
#define CLI_RETATT "[Errore] Atteso comando \"RETRIEVE\"\n"

//Errore di comando
#define CLI_DELATT "[Errore] Atteso comando \"DELETE\"\n"

//Errore nelle operazioni di seek
#define CLI_SEEKFERR "[KO] Errore nella lettura del file\n"

//Nome più lungo del massimo = LENMAXUNIX
#define CLI_LENMAXUNIX "[Errore] La lunghezza del nome deve essere minore di 255\n"

//Resoconto
#define CLI_INFODIM 1024
#define CLI_RESO "\n*** RESOCONTO OPERAZIONI EFFETTUATE ***\n"
#define CLI_INFONOP " Numero di operazioni effettuate: "
#define CLI_INFOOPOK " Numero di operazioni terminate correttamente: "
#define CLI_INFOOPFAIL " Numero di operazioni fallite: "
#define CLI_INFOTERM "\n\
* Viene considerata fallita anche la rimozione di un file che non esiste\n\
* Sono conteggiate anche le operazione di connessione e disconnessione\n\
* Nel conteggio, non sono considerati gli eventuali fallimenti precedenti\n\
alle effettive operazioni\n\n\
-------------------------------------------------------------------------\n\n"

#endif /* HEADERCLIENT_H */
