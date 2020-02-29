#include "accesso.h" //L'header della libreria che contiene le funzioni della traccia

socketnode_t snode; //Struttura per la connessione al server

//os_connect, utilizzata per la connessione al server
int os_connect(char *name) {
  char *header; //Conterrà l'header (REGISTER name);
  int n; //Contiene la lunghezza dei messaggi scambiati col server
  int notused; //Usato per controlli delle funzioni
  msg_t msg; //Struttura utilizzata per la ricezione dei messaggi

  ACC_CHECKNULL(header, calloc((LENHEADER + 1), sizeof(char)), ACC_MAF); //L'header sarà lungo al massimo LENHEADER (Qui allocati dinamicamente)
  ACC_MENO1(snode.fd, socket(AF_UNIX, SOCK_STREAM, 0), ACC_SOCKF); //Controllo l'esito della SC socket
  snode.addr.sun_family = AF_UNIX; //Indico socket locake
    strncpy(snode.addr.sun_path, SOCKNAME, strlen(SOCKNAME)); //Indico il nome del socket (SOCKNAME: objstore.sock definito in conn.h)
  ACC_MENO1_AND_FREE1(notused, connect(snode.fd, (struct sockaddr *)&(snode.addr), sizeof(snode.addr)), header, ACC_CONF); //Controllo l'esito della SC connect
  header = strncpy(header, REGISTER, strlen(REGISTER)); //Copio nell'header il comando REGISTER
  header = strncat(header, " ", 1); //Aggiungo uno spazio
  header = strncat(header, name, strlen(name)); //Concateno il nome del client
  n = strlen(header); //Calcolo la lunghezza dell'header
  ACC_MENO1_AND_FREE1(notused, writen(snode.fd, header, n), header, ACC_COMF); //Invio l'header al server
  ACC_CHECKNULL_AND_FREE1(msg.str, calloc((LENGENMSG + 1), sizeof(char)), header, ACC_MAF); //Alloco memoria per la risposta del server
  ACC_MENO1_AND_FREE2(n, readn(snode.fd, msg.str, LENGENMSG * sizeof(char)), header, msg.str, ACC_COMF); //Leggo la risposta del server
  if(n == 0) { //Se il numero di caratteri letti è zero
    free(header); //libero header
    return ACC_COMF; //e ritorno ACC_COMF che indica che la comunicazione è fallita
  }
  if(strcmp(msg.str, CLI_NOADD) == 0) { //Controllo se il server non abbia avuto errori nell'inserimento del nome del client, nella struttura dati che mantiene i nomi dei client connessi
    free(header); //Se c'è stato questo errore, libero header
    free(msg.str); //e msg.str
    return ACC_NOADD; //e ritorno il valore che indica il suddetto errore al client che provvederà alla stampa del messaggio opportuno
  }
  if(strcmp(msg.str, CLI_EXISTS) == 0) { //Controllo se non sia già collegato un client con lo stesso nome indicato dal client corrente
    free(header); //Se così, libero header
    free(msg.str); //e msg.str
    return ACC_EXISTS; //e ritrono il valore che indica il suddetto errore al client che provvederà alla stampa del messaggio opportuno
  }
  if(strcmp(msg.str, CLI_DIRACCF) == 0) { //Controllo se il server abbia avuto problemi nell'accesso o la creazione della directory per il client corrente
    free(header); //Nel caso ci fosse l'errore, libero header
    free(msg.str); //e msg.str
    return ACC_DIRACCF; //e ritorno il valore che indica il suddetto errore al client che provvederà alla stampa del messaggio opportuno
  }
  free(header); //Altrimenti, se non si è verificato nessuno degli errori precedentemente controllati
  free(msg.str); //libero header e msg.str
  return ACC_OK; //e ritorno il valore che indica al client che la connessione è riuscita
}

//Funzione utilizzata per la memorizzazione degli oggetti nello store
int os_store(char *name, void *block, size_t len) {
    char *header, datastrlen[12]; //Header conterrà l'header da inviare al server e datastrlen sarà utile alla conversione in strina della lunghezza dei dati.
    int n, notused; //Usati per la lunghezza dei messaggi e per il controllo dei valori di ritorno
    msg_t msg; //Struttura dati per contenere i messaggi che il server invierà
    sprintf(datastrlen, "%lu", len); //Conversione della lunghezza dei dati in stringa
    ACC_CHECKNULL(header, calloc((LENHEADER + MAXLENDATA + 1), sizeof(char)), ACC_MAF); //Controllo che l'allocazione della memoria per l'header vada a buon termine
    header = strncpy(header, STORE, strlen(STORE)); //Copio al suo interno il comando STORE
    header = strncat(header, " ", 1); //Aggiungo uno spazio
    header = strncat(header, name, strlen(name)); //Concateno il nome dell'oggetto
    header = strncat(header, " ", 1); //Aggiungo uno spazio
    header = strncat(header, datastrlen, strlen(datastrlen)); //Concateno la lunghezza dei dati (convertita a stringa)
    header = strncat(header, " \n ", 3); //Aggiungo il new line che precede i dati
    header = strncat(header, block, len); //Concateno i dati
    n = strlen(header); //Calcolo la lunghezza dell'header
    ACC_MENO1_AND_FREE1(notused, writen(snode.fd, header, n), header, ACC_COMF); //Spedisco l'header con i dati
    ACC_CHECKNULL_AND_FREE1(msg.str, calloc((LENGENMSG + 1), sizeof(char)), header, ACC_MAF); //Alloco dinamicamente la memoria che conterrà la risposta del server
    ACC_MENO1_AND_FREE2(n, readn(snode.fd, msg.str, LENGENMSG * sizeof(char)), header, msg.str, ACC_COMF); //Leggo la risposta del server
    if(n == 0) { //Se il numero di caratteri letti è uguale a 0,
        free(header); //libero l'header
        return ACC_COMF; //e comunico al client un errore di comunicazione
    }
    if(strcmp(msg.str, CLI_OPFERR) == 0) { //Se il messaggio ricevuto indica un errore nell'apertura del file
        free(msg.str); //libero msg.str
        free(header); //e l'header
        return ACC_OPFERR; //e comunico l'errore al client
    } else if(strcmp(msg.str, CLI_PARFILE) == 0) { //Se il messaggio ricevuto indica un errore di scrittura parziale del file
        free(msg.str); //libero msg.str
        free(header); //e l'header
        return ACC_PARFILE; //e comunico al client l'errore
    } else { //Altrimenti
        free(msg.str); //libero msg.str
        free(header); //e l'header
        return ACC_OK; //e comunico il buon esito dell'operazione al client
    }
}

//Funzione utilizzata per il recupero dei dati
void *os_retrieve(char *name) {
    char *header, *datalenstr, *rest; //Header, lunghezza dei dati e rest usato per la strtok
    int notused, n, lendata; //Interi che indicano la lunghezza dei messaggi, i valori di ritorno delle funzioni e la lunghezza dei dati
    msg_t msg; //Struttura per i messaggi ricevuti dal server
    ACC_CHECKNULL(header, calloc((LENHEADER + 1), sizeof(char)), ACC_MAF_RET); //Controllo il buon esito dell'allocazione di memoria che conterrò l'header da inviare al server
    header = strncpy(header, RETRIEVE, strlen(RETRIEVE)); //Copio nell'header il comando RETRIEVE
    header = strncat(header, " ", 1);
    header = strncat(header, name, strlen(name)); //Copio il nome dell'oggetto
    n = strlen(header); //Calcolo la lunghezza dell'header
    ACC_MENO1_AND_FREE1(notused, writen(snode.fd, header, n), header, ACC_COMF_RET); //Invio l'header
    ACC_CHECKNULL_AND_FREE1(msg.str, calloc((LENHEADER + MAXLENDATA + 1), sizeof(char)), header, ACC_MAF_RET); //Leggo l'header del server
    ACC_MENO1_AND_FREE2(n, readn(snode.fd, msg.str, 15 + MAXLENDATA * sizeof(char)), header, msg.str, ACC_COMF_RET); //Leggo la risposta del server
    if(n == 0) { //Se il numero di caratteri letto è zero
        free(header); //libero l'header
        return ACC_COMF_RET; //e comunico l'errore di comunicazione
    }
    msg.str = realloc(msg.str, n + 1); //Rialloco la memoria per la rispsota del server
    if(strcmp(msg.str, CLI_OPFERR) == 0) { //Se il messaggio riporta un errore nell'apertura del file
        free(header); //libero l'header
        free(msg.str); //libero msg.str
        return ACC_OPFERR_RET; //e riporto l'errore al client
    } else if(strcmp(msg.str, CLI_SEEKFERR) == 0) { //Se il messaggio riporta un errore nella lettura del file
        free(header); //libero l'header
        free(msg.str); //libero msg.str
        return ACC_SEEKFERR_RET; //e riporto l'errore al client
    } else if(strcmp(msg.str, CLI_FDE) == 0) { //Se il messaggio afferma che il file non esiste
        free(header); //libero l'header
        free(msg.str); //libero msg.str
        return ACC_FDE_RET; //e riporto il messaggio al client
    } else { //altrimenti
        free(header); //libero l'header
        return msg.str; //e "invio" i dati al client
    }
}

int os_delete(char *name) {
    char *header; //Header da inviare al server
    int notused, n; //Valori per il controllo dei valori di ritorno delle funzioni e il calcolo dei caratteri letti o scritti
    msg_t msg; //Struttura dati per la risposta del server
    ACC_CHECKNULL(header, calloc((LENHEADER + 1), sizeof(char)), ACC_MAF); //Alloco la memoria per l'header
    header = strncpy(header, DELETE, strlen(DELETE)); //Copio il comando DELETE
    header = strncat(header, " ", 1);
    header = strncat(header, name, strlen(name)); //Concateno il nome dell'oggetto
    n = strlen(header); //Calcolo la lunghezza dell'header
    ACC_MENO1_AND_FREE1(notused, writen(snode.fd, header, n), header, ACC_COMF); //Invio l'header al server
    ACC_CHECKNULL_AND_FREE1(msg.str, calloc((LENGENMSG + 1), sizeof(char)), header, ACC_MAF); //Alloco la memoria per la risposta del server
    ACC_MENO1_AND_FREE2(n, readn(snode.fd, msg.str, LENGENMSG * sizeof(char)), header, msg.str, ACC_COMF); //Leggo la risposta del server
    if(n == 0) { //se i caratteri letti sono zero
        free(header); //libero header
        return ACC_COMF; //e riporto un errore di comunicazione
    }
    if(strcmp(msg.str, CLI_RIMFERR) == 0) { //se il messaggio comunica un errore nella rimozione del file
        free(msg.str); //libero msg.str
        free(header); //libero header
        return ACC_RIMFERR; //e lo comunico al client
    } else if(strcmp(msg.str, CLI_FDE) == 0) { //se il messaggio comunica che il file non esiste
        free(msg.str); //libero msg.str
        free(header); //libero l'header
        return ACC_FDE; //e lo riporto al client
    } else { //altrimenti
        free(msg.str); //libero msg.str
        free(header); //libero l'header
        return ACC_OK; //e riporto al client il buon esito dell'operazione di cancellazione del file
    }
}

int os_disconnect() {
  char *header = LEAVE; //Inserisco in header il comando LEAVE
  msg_t msg; //Struttura dati per conentere il messaggio di riposta del server
  int notused; //Intero per il controllo del ritorno delle funzioni
  int n = strlen(LEAVE); //Calcolo la lunghezza dle comando LEAVE (header da inviare)
    
  ACC_MENO1(notused, writen(snode.fd, header, n), ACC_COMF); //Invio l'header al server
  ACC_CHECKNULL(msg.str, calloc((LENGENMSG + 1), sizeof(char)), ACC_MAF); //Alloco la memoria per la risposta
  ACC_MENO1_AND_FREE1(notused, readn(snode.fd, msg.str, LENGENMSG * sizeof(char)), msg.str, ACC_COMF); //Leggo la risposta del server
  if(notused == 0) return ACC_COMF; //Se i caratteri letti sono zero, comunico un errore di comunicazione
  if(strcmp(msg.str, CLI_DISCOK) == 0) { //Se la risposta comunica il buon esito dell'operazione
    free(msg.str); //libero msg.str
    return ACC_OK; //e comunico al client
  }
  free(msg.str); //altrimenti libero msg.str
  return ACC_COMF; //e comunico l'errore
}
