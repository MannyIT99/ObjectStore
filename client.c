#include "accesso.h"

void printInfo(char *tt, int nop, int opok, int opfail);

int main(int argc, char *argv[]) {
  write(1, CLI_TITLE, strlen(CLI_TITLE)); //Segna l'avvio del client
  char *testype; //Indica il tipo di test che sarà effettuato
  int op_effettuate = 0; //Numero di operazioni effettuare
  int op_successo = 0; //Numero di operazioni terminate correttamente
  int op_fallite = 0; //Numero di operazioni fallita
  if(argc != NPARAM) { //se il numero dei parametri è diverso da 3 (compreso il nome dell'eseguibile)
    write(2, CLI_NE3, strlen(CLI_NE3)); //Comunico l'errore
    printInfo(TESTND, op_effettuate, op_successo, op_fallite); //Stampo il resoconto delle operazioni
    exit(EXIT_FAILURE); //ed esco
  }
  int test = atoi(argv[2]); //Altrimenti converto il tipo di test
  if(test < 1 || test > 3) { //Se il valore per il testo è maggiore di 3 o minore di 1, riposto l'errore al client
    write(2, CLI_TNE, strlen(CLI_TNE));
    printInfo(TESTND, op_effettuate, op_successo, op_fallite); // e stampo il resoconto delle operazioni
    exit(EXIT_FAILURE); //ed esco
  }
  //Controllo la tipologia di test e assegno a testype il valore relativo
  if(test == 1) {
    testype = TEST1_STR;
  } else if(test == 2) {
    testype = TEST2_STR;
  } else {
    testype = TEST3_STR;
  }
  char *nomeclient;
  if((nomeclient = argv[1]) == NULL) { //Se il nome del client è nullo
    write(2, CLI_NN, strlen(CLI_NN)); //Comunico l'errore
    printInfo(testype, op_effettuate, op_successo, op_fallite); //riporto il resoconto delle operazioni effettuate
    exit(EXIT_FAILURE); //ed esco
  }
  if(strlen(nomeclient) > LENMAXUNIX) { //se il nome del client è maggiore della lunghezza massima consentita
    write(2, CLI_LENMAXUNIX, strlen(CLI_LENMAXUNIX)); //riporto l'errore
    printInfo(testype, op_effettuate, op_successo, op_fallite); //stampo il resoconto delle operazioni effettuate
    exit(EXIT_FAILURE); //ed esco
  }
  switch(os_connect(nomeclient)) { //Tento la connessione al server
    case ACC_OK: //Successo
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(1, CLI_REGOK, strlen(CLI_REGOK)); //Comunico che la connessione è andata a buon fine
      op_successo++; //Incremento il numero di operazioni terminate correttamente
      break;
    case ACC_MAF: //Allocazione memoria: fallita
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(2, CLI_MAF, strlen(CLI_MAF)); //Comunico l'errore
      op_fallite++; //Incremeneto il numero di operazioni fallite
      printInfo(testype, op_effettuate, op_successo, op_fallite);
      exit(EXIT_FAILURE);
    case ACC_SOCKF: //SC socket fallita
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(2, CLI_SC_SOCKF, strlen(CLI_SC_SOCKF)); //Comunico l'errore
      op_fallite++; //Incremeneto il numero di operazioni fallite
      printInfo(testype, op_effettuate, op_successo, op_fallite);
      exit(EXIT_FAILURE);
    case ACC_CONF: //SC connect fallita
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(2, CLI_SC_CONF, strlen(CLI_SC_CONF)); //Comunico l'errore
      op_fallite++; //Incremeneto il numero di operazioni fallite
      printInfo(testype, op_effettuate, op_successo, op_fallite);
      exit(EXIT_FAILURE);
    case ACC_COMF: //Errore writen o readn (conn.h)
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(2, CLI_COMF, strlen(CLI_COMF)); //Comunico l'errore
      op_fallite++; //Incremeneto il numero di operazioni fallite
      printInfo(testype, op_effettuate, op_successo, op_fallite);
      exit(EXIT_FAILURE);
    case ACC_NOADD: //Errore inserimento utente nella lista dei client connessi al server
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(2, CLI_NOADD, strlen(CLI_NOADD)); //Comunico l'errore
      op_fallite++; //Incremeneto il numero di operazioni fallite
      printInfo(testype, op_effettuate, op_successo, op_fallite);
      exit(EXIT_FAILURE);
    case ACC_EXISTS: //Esiste già un utente connesso con quel nome
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(2, CLI_EXISTS, strlen(CLI_EXISTS)); //Comunico l'errore
      op_fallite++; //Incremeneto il numero di operazioni fallite
      printInfo(testype, op_effettuate, op_successo, op_fallite);
      exit(EXIT_FAILURE);
    case ACC_DIRACCF: //Accesso alla directory: fallito
      op_effettuate++; //Incremento il numero di operazioni effettute
      write(2, CLI_DIRACCF, strlen(CLI_DIRACCF)); //Comunico l'errore
      op_fallite++; //Incremeneto il numero di operazioni fallite
      printInfo(testype, op_effettuate, op_successo, op_fallite);
      exit(EXIT_FAILURE);
  }
  switch(test) { //Controllo il tipo di test
    case TEST1: { //Eseguo test di tipo 1 (STORE)
      for(int it=0; it<IT_TEST1; it++) { //Esco il ciclo 20 volte
          //Header da inviare al server, nome dell'oggetto, lunghezza dei dati, dati, comando, rest per la strtok
          char *header, *nomeoggetto, *stringalunghezzadati, *dati, *comando, *rest;
          //lunghezza dei dati
          size_t lunghezzadati;
          if((header = calloc((LENHEADER + 1), sizeof(char))) == NULL) { //Alloco la memoria per l'header
            write(2, CLI_MAF, strlen(CLI_MAF)); //Se errore, lo comunico
            break; //ed esco dal ciclo
          }
          if(fgets(header, (LENHEADER + 1), stdin) == NULL) { //Controllo se la stringa contenete l'header è nulla
            free(header); //libero header
            write(2, CLI_NN, strlen(CLI_NN)); //Se errore, lo comunico
            break; //ed esco dal ciclo
          }
          if((comando = strtok_r(header, SEPST, &rest)) == NULL) { //Controllo se la stringa contenete il comando è nulla
            free(header); //libero header
            write(2, CLI_NN, strlen(CLI_NN)); //Se errore, lo comunico
            break; //ed esco dal ciclo
          }
          if(strcmp(comando, STORE) != 0) { //Controllo se il comando sia STORE
            free(header); //libero header
            write(2, CLI_STATT, strlen(CLI_STATT)); //Se errore, lo comunico
            break; //ed esco dal ciclo
          }
          if((nomeoggetto = strtok_r(NULL, SEPST, &rest)) == NULL) { //Controllo se la stringa contenete il nome dell'oggetto è nulla
            free(header); //libero header
            write(2, CLI_NN, strlen(CLI_NN)); //Se errore, lo comunico
            break; //ed esco dal ciclo
          }
          if((stringalunghezzadati = strtok_r(NULL, SEPST, &rest)) == NULL) { //Controllo se la stringa contenete la lunghezza dei dati è nulla
            free(header); //libero header
            write(2, CLI_NN, strlen(CLI_NN)); //Se errore, lo comunico
            break; //ed esco dal ciclo
          }
          lunghezzadati = atoi(stringalunghezzadati); //Converto la lnghezza dei dati
          if(lunghezzadati == 0) { //Se la lunghezza è uguale a zero
            free(header); //libero header
            write(2, CLI_0OBJ, strlen(CLI_0OBJ)); //Comunico l'errore
            break; //ed esco dal ciclo
          }
          CLI_CHECKNULL_AND_FREE1(dati, calloc(lunghezzadati + 1, sizeof(char)), header, CLI_MAF, op_fallite); //Alloco la memoria per i dati
          CLI_CHECKNULL_AND_FREE2(dati, fgets(dati, (lunghezzadati + 2), stdin), dati, header, CLI_NN, op_fallite);; //Leggo i dati dallo standard input
          dati[lunghezzadati] = '\0'; //Inserisco terminatore di stringa
          switch(os_store(nomeoggetto, dati, lunghezzadati)) { //Esco la store
            case ACC_OK: //Oggetto memorizzato corrttamente
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_successo++; //Incremento il numero di operazioni terminate correttamente
                write(1, CLI_MEMOBJOK, strlen(CLI_MEMOBJOK)); //lo comunico al client
                break;
            case ACC_OPFERR: //Errore nell'apertura del file
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_OPFERR, strlen(CLI_OPFERR)); //comunico l'errore
                break;
            case ACC_PARFILE: //Scrittura parziale del file
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_PARFILE, strlen(CLI_PARFILE)); //comunico l'errore
                break;
            case ACC_COMF: //Comunicazione fallita
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_COMF, strlen(CLI_COMF)); //comunico l'errore
                break;
          }
        }
        break; //Esco dallo switch
    }
    case TEST2: { //Eseguo test di tipo 2 (RETRIEVE)
        for(int i=0; i<20; i++) { //Eseguo il ciclo 20 volte
            //Header, rest per la strtok, comando, dati e nome dell'oggetto
            char *header, *rest, *comando, *dati, *nomeoggetto;
            if((header = calloc((LENHEADER + 1), sizeof(char))) == NULL) { //Alloco la memoria per l'header
                write(2, CLI_MAF, strlen(CLI_MAF));//Se errore, stampo
                break; //ed esco dal ciclo
            }
            if(fgets(header, (LENHEADER + 1), stdin) == NULL) { //Controllo se la stringa contenete l'header è nulla
                free(header); //se errore, libero l'header
                write(2, CLI_NN, strlen(CLI_NN)); //stampo
                break; //ed esco dal ciclo
            }
            if((comando = strtok_r(header, SEPST, &rest)) == NULL) { //Controllo se la stringa contenete il comando è nulla
                free(header); //se errore, libero l'header
                write(2, CLI_NN, strlen(CLI_NN)); //stampo
                break; //ed esco dal ciclo
            }
            if(strcmp(comando, RETRIEVE) != 0) { //Controllo se il comando è diverso da RETRIEVE
                free(header); //se errore, libero l'header
                write(2, CLI_RETATT, strlen(CLI_RETATT)); //stampo
                break; //ed esco dal ciclo
            }
            if((nomeoggetto = strtok_r(NULL, SEPST, &rest)) == NULL) { //Controllo se la stringa contenete il nome dell'oggetto è nulla
                free(header); //se errore, libero l'header
                write(2, CLI_NN, strlen(CLI_NN)); //stampo
                break; //ed esco dal ciclo
            }
            dati = os_retrieve(nomeoggetto); //Eseguo la retrieve
            op_effettuate++; //Incremento il numero di operazioni effettuate
            if(strcmp(dati, CLI_OPFERR) == 0) { //Se il messaggio comunica un errore nell'apertura del file
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_OPFERR, strlen(CLI_OPFERR)); //stampo l'errore
            } else if(strcmp(dati, CLI_SEEKFERR) == 0) { //Se il messaggio comunica un errore nella lettura del file
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_SEEKFERR, strlen(CLI_SEEKFERR)); //stampo l'errore
            } else if(strcmp(dati, CLI_MAF) == 0) { //Se il messaggio comunica un errore nell'allocazione della memoria
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_MAF, strlen(CLI_MAF)); //stampo l'errore
            } else if(strcmp(dati, CLI_COMF) == 0) { //Se il messaggio comunica un errore di comunicazione
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_COMF, strlen(CLI_COMF)); //stampo l'errore
            } else if(strcmp(dati, CLI_FDE) == 0) { //Se il messaggio comunica che il file non esiste
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_FDE, strlen(CLI_FDE)); //stampo l'errore
            } else { //Altrimenti
                op_successo++; //Incrementa il numero di operazioni terminate correttamente
                write(1, dati, strlen(dati)); //Stampa i dati
                write(1, "\n", 1);
            }
            free(header); //Libera l'header
        }
        break; //Esco dallo switch
    }
    case TEST3: { //Eseguo test di tipo 3 (DELETE)
        for(int i=0; i<20; i++) { //Eseguo per 20 volte il ciclo
            //Header, rest per la strtok, comando e nome dell'oggetto
            char *header, *rest, *comando, *nomeoggetto;
            if((header = calloc((LENHEADER + 1), sizeof(char))) == NULL) { //Alloco la memoria per l'header
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                write(2, CLI_MAF, strlen(CLI_MAF)); //Stampo
                break; //ed esco dal ciclo
            }
            if(fgets(header, (LENHEADER + 1), stdin) == NULL) {
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                free(header); //libero l'header
                write(2, CLI_NN, strlen(CLI_NN)); //stampo
                break; //ed esco dal ciclo
            }
            if((comando = strtok_r(header, SEPST, &rest)) == NULL) {
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                free(header); //libero l'header
                write(2, CLI_NN, strlen(CLI_NN)); //stampo
                break; //ed esco dal ciclo
            }
            if(strcmp(comando, DELETE) != 0) {
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                free(header); //libero l'header
                write(2, CLI_DELATT, strlen(CLI_DELATT)); //stampo
                break; //ed esco dal ciclo
            }
            if((nomeoggetto = strtok_r(NULL, SEPST, &rest)) == NULL) {
                op_effettuate++; //Incremento il numero di operazioni effettuate
                op_fallite++; //Incremento il numero di operazioni fallite
                free(header); //libero l'header
                write(2, CLI_NN, strlen(CLI_NN)); //stampo
                break; //ed esco dal ciclo
            }
            switch(os_delete(nomeoggetto)) {
                case ACC_OK: //Operazione riuscita
                    op_effettuate++; //Incremento il numero di operazioni effettuate
                    op_successo++; //Incremento il numero di operazioni terminate correttamente
                    write(1, CLI_RIMFOK, strlen(CLI_RIMFOK)); //stampo
                    break; //ed esco dallo switch
                case ACC_RIMFERR: //Errore nella rimozione del file
                    op_effettuate++; //Incremento il numero di operazioni effettuate
                    op_fallite++; //Incremento il numero di operazioni fallite
                    write(2, CLI_RIMFERR, strlen(CLI_RIMFERR)); //stampo
                    break; //ed esco dallo switch
                case ACC_FDE: //Il file non esiste
                    op_effettuate++; //Incremento il numero di operazioni effettuate
                    op_fallite++; //Incremento il numero di operazioni fallite
                    write(2, CLI_FDE, strlen(CLI_FDE)); //stampo
                    break; //ed esco dallo switch
                case ACC_COMF: //Errore nella comunicazione col server
                    op_effettuate++; //Incremento il numero di operazioni effettuate
                    op_fallite++; //Incremento il numero di operazioni fallite
                    write(2, CLI_COMF, strlen(CLI_COMF)); //stampo
                    break; //ed esco dallo switch
            }
        }
        break; //Esco dallo switch
    }
  }
  switch(os_disconnect()) { //Eseguo la disconnessione
    case ACC_OK: //Disconnessione riuscita
      op_effettuate++; //Incremento il numero di operazioni effettuate
      op_successo++; //Incremento il numero di operazioni terminate correttamente
      write(1, CLI_DISCOK, strlen(CLI_DISCOK)); //Stamp l'esito
      printInfo(testype, op_effettuate, op_successo, op_fallite); //Stampo il resoconto delle operazioni
      exit(EXIT_SUCCESS); //Termino correttamente
    case ACC_COMF: //Errore di comunicazione
      op_effettuate++; //Incremento il numero di operazioni effettuate
      op_fallite++; //Incremento il numero di operazioni fallite
      write(2, CLI_DISCNOK, strlen(CLI_DISCNOK)); //Stampo l'errore nella disconnessione
      printInfo(testype, op_effettuate, op_successo, op_fallite); //Stampo il resoconto delle operazioni
      exit(EXIT_FAILURE); //Esco
  }
  return 0;
}

void printInfo(char *tt, int nop, int opok, int opfail) {
    char *info;
    CHECKNULL(info, calloc(CLI_INFODIM, sizeof(char)), CLI_MAF); //Alloco la memoria per il resoconto delle operazioni
    char conv[12]; //Per la conversione dei valori da stampare
    int strtt = strlen(tt);
    int lenpre;
    char *pre = calloc(strtt + 5, sizeof(char));
    pre = strncpy(pre, "[ ", 2);
    pre = strncat(pre, tt, strtt);
    pre = strncat(pre, " ]", 2);
    lenpre = strlen(pre);
    info = strncpy(info, CLI_RESO, strlen(CLI_RESO));
    info = strncat(info, pre, lenpre);
    info = strncat(info, CLI_INFONOP, strlen(CLI_INFONOP));
    sprintf(conv, "%d", nop);
    info = strncat(info, conv, strlen(conv));
    info = strncat(info, "\n", 1);
    info = strncat(info, pre, lenpre);
    info = strncat(info, CLI_INFOOPOK, strlen(CLI_INFOOPOK));
    sprintf(conv, "%d", opok);
    info = strncat(info, conv, strlen(conv));
    info = strncat(info, "\n", 1);
    info = strncat(info, pre, lenpre);
    info = strncat(info, CLI_INFOOPFAIL, strlen(CLI_INFOOPFAIL));
    sprintf(conv, "%d", opfail);
    info = strncat(info, conv, strlen(conv));
    info = strncat(info, "\n", 1);
    info = strncat(info, CLI_INFOTERM, strlen(CLI_INFOTERM)); 
    write(1, info, strlen(info));
    free(info);
}
