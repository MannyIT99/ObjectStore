#define _XOPEN_SOURCE 500
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/stat.h>
#include "clientconnessi.h"
#include "headerserver.h"
#include <signal.h>
#include <ftw.h>

//Versione 1 thread per connessione (thread lanciati in modalità detached)
elemento *head = NULL; //Inizializza la struttura che contiene i nomi dei client connessi
elemento *tail = NULL;
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER; //Accesso al file system
pthread_mutex_t mtx_nclient = PTHREAD_MUTEX_INITIALIZER; //Accesso alla variabile che indica il numero di client connessi
pthread_mutex_t mtx_nobj = PTHREAD_MUTEX_INITIALIZER; //Accesso alla variabile che indica il numero di oggetti presenti nello store
pthread_mutex_t mtx_szstore = PTHREAD_MUTEX_INITIALIZER; //Accesso alla variabile che indica la size totale dello store
int nclient = 0; //Inizializzo il numero dei client connessi
int nobj = 0; //Inizializzero il numero degli oggetti presenti nello store
int szstore = 0; //Inizializzo la size totale dello store
static volatile sig_atomic_t termina = 0;

void cleanup(); //unlink(SOCKNAME)
void spawn_thread(long fd_c); //Thread che genera i worker
void *worker(void *arg); //Thread per la gestione della comunicazione con i client
int isThere(char *pathname); //Controlla se un file è presente oppure no
void *sig_thread(void *arg); //Thread per la gestione dei segnali
static void sighandler(int useless); //Altro thread di supporto alla gestione dei segnali
static int infoser(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf); //Funzione utilizzata per il calcolo delle informazioni di stato del server

int main(int argc, char *argv[]) {
  cleanup(); //unlink(SOCKNAME)
  atexit(cleanup);
  int fd_s; //File descriptor del socket
  int notused; //Variabile utilizzata per il controllo del valore di ritorno dalle funzioni
  struct sockaddr_un serv_addr;
  struct sigaction sa;
  pthread_t thread;
  sigset_t set;
  //Calcolo size totale dello store
  SER_MENO1_BLOCK(fd_s, socket(AF_UNIX, SOCK_STREAM, 0), SER_SC_SOCKF); //SC Socket
  memset(&serv_addr, '0', sizeof(serv_addr)); //Inizializzo serv_addr
  serv_addr.sun_family = AF_UNIX;
  strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);
  SER_MENO1_BLOCK(notused, bind(fd_s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), SER_SC_BINDF); //SC bind
  SER_MENO1_BLOCK(notused, listen(fd_s, SOMAXCONN), SER_SC_LISF); //SC listen
  //Bloccare segnale SIGUSR1
  sigemptyset(&set); //Blocco SIGUSR1. I thread che verranno generati, erediteranno la maschera dei segnali
  sigaddset(&set, SIGUSR1);
  SER_NEZERO_BLOCK(notused, pthread_sigmask(SIG_BLOCK, &set, NULL), SER_ERRSIG, exit(EXIT_FAILURE));
  SER_NEZERO_BLOCK(notused, pthread_create(&thread, NULL, &sig_thread, (void*)&set), SER_ERRSIG, exit(EXIT_FAILURE));
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sighandler;
  SER_MENO1_BLOCK(notused, sigaction(SIGINT, &sa, NULL), SER_ERRSIG);
  SER_MENO1_BLOCK(notused, sigaction(SIGQUIT, &sa, NULL), SER_ERRSIG);
  SER_MENO1_BLOCK(notused, sigaction(SIGTERM, &sa, NULL), SER_ERRSIG);
  while(1) { //Accetto richieste all'infinito
    long fd_c; //File descriptor utilizzato per la comunicazione col client
    if((fd_c = accept(fd_s, (struct sockaddr*)NULL, NULL)) == -1) {
        if(errno == EINTR) { //Se l'errore è EINTR
            if(termina) //e se termina = 1, allora esco dal ciclo
                break;
        } else { //se è un altro errore, comunico il fallimento della SC accept al client
            write(2, SER_SC_ACCPF, strlen(SER_SC_ACCPF));
            exit(EXIT_FAILURE); //ed esco
        }
    }
    //altrimenti connessione accettata
    spawn_thread(fd_c); //Genero un thread per gestire la connessione con il client
  }
  return 0;
}

void cleanup() {
  unlink(SOCKNAME);
}

void spawn_thread(long fd_c) {
  //Setto il thread in modalità detached e lo genero passandogli come funzione, worker
  pthread_attr_t thattr;
  pthread_t thid;
  sigset_t mask, oldmask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGQUIT);
  //Setto la maschera per la gestione dei segnali
  if (pthread_sigmask(SIG_BLOCK, &mask,&oldmask) != 0) {
    write(2, SER_ERRSIG, strlen(SER_ERRSIG));
    close(fd_c);
    return;
  }
  if(pthread_attr_init(&thattr) != 0) {
    write(2, SER_ERRENVTH, strlen(SER_ERRENVTH));
    close(fd_c);
    return;
  }
  //Settiamo il thread in modalità detached
  if(pthread_attr_setdetachstate(&thattr, PTHREAD_CREATE_DETACHED) != 0) {
    write(2, SER_ERRENVTH, strlen(SER_ERRENVTH));
    pthread_attr_destroy(&thattr);
    close(fd_c);
    return;
  }
  //Creo il thread worker
  if(pthread_create(&thid, &thattr, worker, (void*)fd_c) != 0) {
    write(2, SER_ERRENVTH, strlen(SER_ERRENVTH));
    pthread_attr_destroy(&thattr);
    close(fd_c);
    return;
  }
  if(pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {
    write(2, SER_ERRSIG, strlen(SER_ERRSIG));
    close(fd_c);
  }
}

void *worker(void *arg) {
  long fd_c = (long)arg; //File descriptor del socket per la comunicazione col client
  char *nomeclient; //Stringa che contiene il nome utilizzato dal client per la connessione
  char *directory; // Stringa che conterrà la directory del client
  int len_nomeclient; //Lunghezza nome client
  int len_send_msg; //Lunghezza messaggio da inviare al client
  char *nc;
  int len_dirdata = strlen(DIRDATA); //Lunghezza directory "data"
  int conn = 0; //Indica se l'utente è nella lista dei connessi oppure no (1: si, 0: no)
  DIR *dir; //Puntatore alla directory del client
  msg_t msg; //Struct contenente il messaggio che riceve dal client
  int notused; //Variabile utilizzata per il controllo del valore di ritorno dalle funzioni
  do {
    msg_t msg; //Struct contenente il messaggio che riceve dal client
    char *rest; //Utilizzato per le strtok_r
    char *comando; //Stringa contenente il nome del comando ricevuto dal client
    int n; //Intero contenente la lunghezza dei caratteri letti o scritti con readn e writen
    SER_NULL_BLOCK(msg.str, calloc((LENHEADER + MAXLENDATA + 1), sizeof(char))); //Inizializzo la stringa che conterrà il messaggio ricevuto da parte del client
    SER_MENO1_BREAK(n, readn(fd_c, msg.str, (LENHEADER + MAXLENDATA) * sizeof(char)), SER_COMF); //Leggo il messaggio e lo inserisco in msg.str
    if(n == 0) { //Se n = 0
        write(2, SER_COMF, strlen(SER_COMF)); //Riporto l'errore di comunicazione fallita
        break; //ed esco dal ciclo
    } //Socket chiuso -> esco dal ciclo
    msg.str = realloc(msg.str, n + 1); //Altrimenti rialloco la memoria
    comando = strtok_r(msg.str, SEPST, &rest); //Contiene il comando
    //Se il comando è REGISTER
    if(strcmp(comando, REGISTER) == 0) {
      nomeclient = strtok_r(NULL, SEPST, &rest); //Contiene il nome del client
      nc = calloc(sizeof(char), strlen(nomeclient) + 1);
      nc = strncpy(nc, nomeclient, strlen(nomeclient));
      len_nomeclient = strlen(nomeclient); //Calcolo la lunghezza del nome del client
      pthread_mutex_lock(&mtx_nclient);
      notused = cercautente(head, nomeclient);
      pthread_mutex_unlock(&mtx_nclient);
      if(notused == 0) { //Controllo se è già presente un client connesso con quel nome
        pthread_mutex_lock(&mtx_nclient);
        head = inshead(&head, &tail, nomeclient);
        pthread_mutex_unlock(&mtx_nclient);
        if(head == NULL) { //Se no, lo aggiungo alla lista dei client connessi
          //Errore funzione aggiugni utente
          len_send_msg = strlen(SER_NOADD); //Calcolo la lunghezza del messaggio da inviare
          SER_MENO1_BREAK(notused, writen(fd_c, SER_NOADD, len_send_msg), SER_COMF); //Invio il messaggio
          break; //Esco dal ciclo
        } else {
          conn = 1; //conn = 1 perché in questo punto la connessione si considera effettuata correttamente
          pthread_mutex_lock(&mtx_nclient);
          nclient++; //Incremento il numero dei client connessi (in mutua esclusione)
          pthread_mutex_unlock(&mtx_nclient);
          int mallocdir = len_dirdata + len_nomeclient; //Calcolo la lunghezza della stringa contenente il nome della directory del client
          SER_CHECK_NULL(directory, calloc((mallocdir + 1), sizeof(char)), SER_MAF); //Inizializzo la stringa
          directory = strncpy(directory, DIRDATA, len_dirdata); //Copio al suo interno il "prefisso" data/
          directory = strncat(directory, nomeclient, len_nomeclient); //Concateno il nome del client
          pthread_mutex_lock(&mtx1); //Lock
          dir = opendir(directory);
          pthread_mutex_unlock(&mtx1); //Rilascio il lock
          if(dir != NULL) { //Presente una directory con quel nome, ci accedo
            //Registrazione riuscita
            len_send_msg = strlen(SER_REGOK); //Calcolo la lunghezza del messaggio da inviare
            SER_MENO1_BREAK(notused, writen(fd_c, SER_REGOK, len_send_msg), SER_COMF); //Invio il messaggio
            if(notused == 0) break;
          } else if(errno == ENOENT) {
            //Non esiste una directory con quel nome
            SER_MENO1_BREAK(notused, mkdir(directory, 0777), SER_SC_MKDIRF); //La creo
            //Registrazione riuscita
            len_send_msg = strlen(SER_REGOK); //Calcolo la lunghezza del messaggio da inviare
            SER_MENO1_BREAK(notused, writen(fd_c, SER_REGOK, len_send_msg), SER_COMF); //Invio il messaggio
            if(notused == 0) break;
          } else {
            //Altro errore
            //Registrazione fallita
            len_send_msg = strlen(SER_DIRACCF); //Calcolo la lunghezza del messaggio da inviare
            SER_MENO1_BREAK(notused, writen(fd_c, SER_DIRACCF, len_send_msg), SER_COMF); //Invio il messaggio
            break; //Esco dal ciclo
          }
        }
      } else {
        //Utente già connesso
        len_send_msg = strlen(SER_EXISTS); //Calcolo la lunghezza del messaggio da inviare
        SER_MENO1_BREAK(notused, writen(fd_c, SER_EXISTS, len_send_msg), SER_COMF); //Invio il messaggio
        break; //Esco dal ciclo
      }
    }
    //Se il comando è STORE
    else if(strcmp(comando, STORE) == 0) {
        char *objnome; //Contiene il nome dell'oggetto da memorizzare
        char *data; //Contiene i dati che devono essere memorizzati all'interno dell'oggetto
        char *datalenstr; //Contiene la lunghezza dei dati
        char *pathfile; //Contiene il pathname del file
        FILE *fd; //Puntatore al file che rappresenta l'oggetto che conterrà i dati da memorizzare
        int lendir; //Lunghezza della stringa contenente il nome della directory
        int lenobj; //Lunghezza della stringa contenente il nome dell'oggetto
        int notused; //Variabile utilizzata per il controllo del valore di ritorno dalle funzioni
        SER_CHECK_NULL(objnome, strtok_r(NULL, " ", &rest), SER_NN);
        SER_CHECK_NULL(datalenstr, strtok_r(NULL, " ", &rest), SER_NN);
        int datalen = atoi(datalenstr); //Converto la stringa contenete la dimensione dei dati da memorizzare in un interno
        SER_CHECK_NULL(data, strtok_r(NULL, " ", &rest), SER_NN); //Salto il carattere new line
        SER_CHECK_NULL(data, strtok_r(NULL, "\n", &rest), SER_NN); //Prendo i dati effettivi
        lendir = strlen(directory); //Calcolo la lunghezza del nome della directory
        lenobj = strlen(objnome); //Calcolo la lunghezza del nome dell'oggetto
        SER_CHECK_NULL_AND_FREE1(pathfile, calloc((lendir + lenobj + 2), sizeof(char)), SER_MAF, data); //Alloco la memoria per il pathname del file
        pathfile = strncpy(pathfile, directory, lendir); //Copio il nome della directory nel pathname
        pathfile = strncat(pathfile, "/", 1);
        pathfile = strncat(pathfile, objnome, lenobj); //Copio il nome del file
        if((fd = fopen(pathfile, "w+")) == NULL) { //Apro il file
            len_send_msg = strlen(SER_OPFERR); //Se non riesco ad aprirlo, comunico il fallimento
            SER_MENO1_BREAK_AND_FREE2(notused, writen(fd_c, SER_OPFERR, len_send_msg), SER_COMF, data, pathfile);
            if(notused == 0) break;
        } else {
            notused = fwrite(data, 1, datalen, fd);
            if(notused < datalen) { //Altrimenti controllo che siano stati scritti tutti i caratteri
                len_send_msg = strlen(SER_PARFILE); //Se no, comunico l'errore
                SER_MENO1_BREAK_AND_FREE2(notused, writen(fd_c, SER_PARFILE, len_send_msg), SER_COMF, data, pathfile);
                if(notused == 0) break;
                if(remove(pathfile) != 0) { //e rimuovo il file appena creato
                    write(2, SER_RIMFERRLOC, strlen(SER_RIMFERRLOC)); //se non riesco, riporto l'errore al server
                }
            } else {
                len_send_msg = strlen(SER_MEMOBJOK); //Altrimenti comunico il buon esito della memorizzazione del file
                SER_MENO1_BREAK_AND_FREE2(notused, writen(fd_c, SER_MEMOBJOK, len_send_msg), SER_COMF, data, pathfile);
                if(notused == 0) break;
            }
        }
        free(pathfile); //libero pathfile
        fclose(fd); //chiudo il file descriptor del file
    }

    //RETRIEVE
    else if(strcmp(comando, RETRIEVE) == 0) {
        FILE *fd; //File descriptor del file
        char *objnome, *pathfile, *dati, datilenstr[21]; //Nome dell'oggetto, pathname del file, dati effettivi e stringa contenente la lunghezza del file
        int lendir, lenobj, notused; //Lunghezza del nome della directory, dell'oggetto e intero per il controllo del valore di ritorno delle funzioni
        long sz; //Size del file
        objnome = strtok_r(NULL, SEPST, &rest); //Nome dell'oggetto
        lendir = strlen(directory); //Calcolo la lunghezza del nome della directory
        lenobj = strlen(objnome); //Calcolo la lunghezza del nome dell'oggetto
        SER_CHECK_NULL(pathfile, calloc((lendir + lenobj + 2), sizeof(char)), SER_MAF); //Alloco la memoria per il pathname del file
        pathfile = strncpy(pathfile, directory, lendir); //Copio il nome della directory
        pathfile = strncat(pathfile, "/", 1);
        pathfile = strncat(pathfile, objnome, lenobj); //Copio il nome del file
        if(isThere(pathfile)) { //Controllo se esiste un file con quel nome
            //Dobbiamo spedire l'header con la lunghezza e il contenuto del file
            if((fd = fopen(pathfile, "r")) == NULL) { //Errore nell'apertura del file
                len_send_msg = strlen(SER_OPFERR); //Comunico l'errore
                SER_MENO1_BREAK_AND_FREE1(notused, writen(fd_c, SER_OPFERR, len_send_msg), SER_COMF, pathfile);
            } else { //File aperto correttamente
                if((fseek(fd, 0L, SEEK_END)) == -1) { //Errore nella lettura
                    len_send_msg = strlen(SER_SEEKFERR); //Comuncio l'errore
                    SER_MENO1_BREAK_AND_FREE1(notused, writen(fd_c, SER_SEEKFERR, len_send_msg), SER_COMF, pathfile);
                } else {
                    if((sz = ftell(fd)) == -1) { //Errore nel calcolo della lunghezza del file
                        len_send_msg = strlen(SER_SEEKFERR); //Comunico l'errore
                        SER_MENO1_BREAK_AND_FREE1(notused, writen(fd_c, SER_SEEKFERR, len_send_msg), SER_COMF, pathfile);
                    } else {
                        rewind(fd); //Riporto il puntatore all'inizio
                        sprintf(datilenstr, "%lu", sz); //Converto la lunghezza del file in stringa
                        char *testata = calloc(sz + 17, sizeof(char)); //Alloco la memoria che conterrà l'header da inviare al client
                        testata = strncpy(testata, "DATA ", 5); //Copio la stringa DATA
                        testata = strncat(testata, datilenstr, strlen(datilenstr)); //Concateno la lunghezza del file
                        testata = strncat(testata, " \n ", 3);
                        SER_CHECK_NULL_AND_FREE2(dati, calloc(sz+1, sizeof(char)), SER_MAF, testata, pathfile); //Alloco la memoria per contenere i dati
                        fread(dati, 1, sz, fd); //Leggo i dati dal file
                        testata = strncat(testata, dati, sz); //Concateno i dati all'header
                        SER_MENO1_BREAK_AND_FREE3(notused, writen(fd_c, testata, strlen(testata)), SER_COMF, testata, dati, pathfile); //Comunico l'header al client
                        free(dati); //Libero dati
                        free(testata); //Libero testata
                        //Chiudo il file descriptor del file
                        fclose(fd);
                    }
                }
            }
        } else {
            //Il file non esiste
            len_send_msg = strlen(SER_FDE); //e lo comunico al client
            SER_MENO1_BREAK_AND_FREE1(notused, writen(fd_c, SER_FDE, len_send_msg), SER_COMF, pathfile);
        }
        free(pathfile); //libero pathfile
    }

    //DELETE
    else if(strcmp(comando, DELETE) == 0) {
        char *pathfile; //Pathname del file
        char *objnome = strtok_r(NULL, SEPST, &rest); //Nome dell'oggetto
        int lendir = strlen(directory); //Calcolo la lunghezza del nome della directory
        int lenobj = strlen(objnome); //Calcolo la lunghezza del nome dell'oggetto
        SER_CHECK_NULL(pathfile, calloc((lendir + lenobj + 2), sizeof(char)), SER_MAF); //Alloco la memoria per il pathname del file
        pathfile = strncpy(pathfile, directory, lendir); //Copio il nome della directory
        pathfile = strncat(pathfile, "/", 1);
        pathfile = strncat(pathfile, objnome, lenobj); //Concateno il pathname del file
        if(isThere(pathfile)) { //Se esiste il file
            if(remove(pathfile) != 0) { //Lo rimuovo
                //Rimozione non riuscita
                len_send_msg = strlen(SER_RIMFERR); //Se non riesce la rimozione, comunica l'errore
                SER_MENO1_BREAK_AND_FREE1(notused, writen(fd_c, SER_RIMFERR, len_send_msg), SER_COMF, pathfile);
            } else {
                //Rimozione riuscita                len_send_msg = strlen(SER_RIMFOK); //Comunica il successo dell'operazione
                SER_MENO1_BREAK_AND_FREE1(notused, writen(fd_c, SER_RIMFOK, len_send_msg), SER_COMF, pathfile);
            }
        } else {
            //Il file non esiste
            len_send_msg = strlen(SER_FDE); //Comunica al client che il file non esiste nello store
            SER_MENO1_BREAK_AND_FREE1(notused, writen(fd_c, SER_FDE, len_send_msg), SER_COMF, pathfile);
        }
        free(pathfile); //Libero pathfile
    }

    //LEAVE
    else if(strcmp(comando, LEAVE) == 0) {
      pthread_mutex_lock(&mtx_nclient);
      head = deleten(&head, &tail, nc); //Elimino il client dalla struttura dati contenente i nomi dei client connessi
      nclient--; //Diminuisco il numero dei client connessi (in mutua esclusione)
      conn = 0; //Indico che non è più connesso
      if(dir) { //Se era aperta la sua directory, la chiudo
        if(closedir(dir) != 0) {
            //Se non riesco a chiuderla, comunico l'errore
            fprintf(stderr, SER_SC_CLODIRF);
        }
      }
      pthread_mutex_unlock(&mtx_nclient);
      len_send_msg = strlen(SER_DISCOK); //Altrimenti comunico il buon esito dell'operazione
      SER_MENO1_BREAK(notused, writen(fd_c, SER_DISCOK, len_send_msg), SER_COMF);
      break;
    }
    free(msg.str);
  }while(1);
  if(conn) { //Se non sono entrato in LEAVE, faccio le stesse operazioni che avrei fatto li
    pthread_mutex_lock(&mtx_nclient);
    head = deleten(&head, &tail, nomeclient);
    nclient--;
    if(dir) { //Se era aperta la sua directory, la chiudo
        if(closedir(dir) != 0) {
            //Se non riesco a chiuderla, comunico l'errore
            fprintf(stderr, SER_SC_CLODIRF);
        }
    }
    pthread_mutex_unlock(&mtx_nclient);
  }
  free(directory); //Libero directory
  close(fd_c); //Chiudo il file descriptor del socket
  return NULL;
}

//Funzione per controllare se esiste già un file con il nome dato come argomento
int isThere(char *pathname) {
    struct stat buffer;
    return (stat(pathname, &buffer) == 0);
}

//Thread per la gestione dei segnali
void *sig_thread(void *arg) {
    int sig, notused, flags = 0;
    char conv[12];
    sigset_t *set = arg;
    for(;;) {
        //Mi metto in attesa del segnale
        SER_NEZERO_BLOCK(notused, sigwait(set, &sig), SER_ERRSIG, exit(EXIT_FAILURE));
        //Se arriva, stampo le informazioni di stato del server e azzero i valori corrispondenti al numero dei client connessi, degli oggetti presenti e la size totale dello store
        write(1, SER_RESO, strlen(SER_RESO));
        write(1, SER_NCLICON, strlen(SER_NCLICON));
        pthread_mutex_lock(&mtx_nclient);
        sprintf(conv, "%d", nclient);
        write(1, conv, strlen(conv));
        pthread_mutex_unlock(&mtx_nclient);
        write(1, SER_NOBJIN, strlen(SER_NOBJIN));
        pthread_mutex_lock(&mtx_nobj);
        SER_MENO1_BLOCK(notused, nftw(DIRDATA, infoser, 20, flags), SER_ERRNOBJ); //Calcolo il numero degli oggetti presenti nello store
        sprintf(conv, "%d", nobj);
        write(1, conv, strlen(conv));
        nobj = 0;
        pthread_mutex_unlock(&mtx_nobj);
        write(1, SER_SZSTO, strlen(SER_SZSTO));
        pthread_mutex_lock(&mtx_szstore);
        SER_MENO1_BLOCK(notused, access(DIRDATA, R_OK), SER_DIRACCF_LOC);
        sprintf(conv, "%d", szstore);
        write(1, conv, strlen(conv));
        szstore = 0;
        pthread_mutex_unlock(&mtx_szstore);
        write(1, SER_RESOTERM, strlen(SER_RESOTERM));
    }
}

static void sighandler(int useless) {
    termina = 1;
}

//Funzione per calcolare numero di oggetti presenti nello store e size totale
static int infoser(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if(tflag == FTW_F ) {
        nobj++;
        szstore += sb -> st_size;
    }
    return 0;
}
