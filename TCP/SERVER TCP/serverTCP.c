/*
    Esercizio: 1 - Server TCP
    Autori: Giovanni Musca, Francesco Signorile
    Corso: Reti di Calcolatori aa 2025-26
    Descrizione: Server che gestisce operazioni matematiche su richiesta del client.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // Libreria per i socket su Windows

#define PORTA 8888
#define BUFFER_SIZE 1024

int main() {
    // --- Inizializzazione Winsock ---
    WSADATA wsa;
    SOCKET socket_ascolto, socket_client;
    struct sockaddr_in server_addr, client_addr;
    int dimensione_client;
    char buffer_invio[BUFFER_SIZE];
    char lettera_ricevuta;
    int num1, num2, risultato;

    printf("PROGETTO RETI - SERVER TCP\n");
    printf("Autori: Giovanni Musca, Francesco Signorile\n");
    printf("-----------------------------------------\n\n");

    // Avvio Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Errore inizializzazione Winsock. Codice: %d\n", WSAGetLastError());
        return 1;
    }

    // 1) Creazione del socket
    socket_ascolto = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ascolto == INVALID_SOCKET) {
        printf("Impossibile creare il socket.\n");
        return 1;
    }

    // Preparazione della struttura indirizzo server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORTA);

    // 2) Bind (Associazione porta)
    if (bind(socket_ascolto, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Errore nel Bind.\n");
        return 1;
    }

    // 3) Listen (Messa in ascolto)
    listen(socket_ascolto, 3);
    printf("Server avviato sulla porta %d. In attesa di connessioni...\n", PORTA); // [cite: 5]

    // Ciclo infinito per gestire un client alla volta [cite: 26]
    while (1) {
        dimensione_client = sizeof(client_addr);
        
        // Accetta connessione
        socket_client = accept(socket_ascolto, (struct sockaddr *)&client_addr, &dimensione_client);
        if (socket_client == INVALID_SOCKET) {
            printf("Errore nell'accettare la connessione.\n");
            continue;
        }

        printf("\nClient connesso.\n");

        // 4) Invia "connessione avvenuta" [cite: 11]
        strcpy(buffer_invio, "connessione avvenuta");
        send(socket_client, buffer_invio, strlen(buffer_invio), 0);

        // 7) Riceve la lettera (comando) [cite: 17]
        if (recv(socket_client, &lettera_ricevuta, sizeof(char), 0) > 0) {
            printf("Lettera ricevuta dal client: %c\n", lettera_ricevuta);

            int operazione_valida = 1;

            // Logica di selezione operazione [cite: 18, 19, 20, 21]
            if (lettera_ricevuta == 'A' || lettera_ricevuta == 'a') {
                strcpy(buffer_invio, "ADDIZIONE");
            } else if (lettera_ricevuta == 'S' || lettera_ricevuta == 's') {
                strcpy(buffer_invio, "SOTTRAZIONE");
            } else if (lettera_ricevuta == 'M' || lettera_ricevuta == 'm') {
                strcpy(buffer_invio, "MOLTIPLICAZIONE");
            } else if (lettera_ricevuta == 'D' || lettera_ricevuta == 'd') {
                strcpy(buffer_invio, "DIVISIONE");
            } else {
                strcpy(buffer_invio, "TERMINE PROCESSO CLIENT"); // [cite: 22]
                operazione_valida = 0;
            }

            // Invia la stringa dell'operazione al client
            send(socket_client, buffer_invio, strlen(buffer_invio) + 1, 0);

            // 9) Se l'operazione è valida, riceve i numeri ed esegue il calcolo [cite: 25]
            if (operazione_valida) {
                // Ricevo i due interi
                recv(socket_client, (char*)&num1, sizeof(int), 0);
                recv(socket_client, (char*)&num2, sizeof(int), 0);
                printf("Ricevuti numeri: %d e %d\n", num1, num2);

                // Calcolo
                if (lettera_ricevuta == 'A' || lettera_ricevuta == 'a') risultato = num1 + num2;
                else if (lettera_ricevuta == 'S' || lettera_ricevuta == 's') risultato = num1 - num2;
                else if (lettera_ricevuta == 'M' || lettera_ricevuta == 'm') risultato = num1 * num2;
                else if (lettera_ricevuta == 'D' || lettera_ricevuta == 'd') {
                    if(num2 != 0) risultato = num1 / num2;
                    else risultato = 0; // Gestione diviso zero base
                }

                // Invia il risultato [cite: 25]
                send(socket_client, (char*)&risultato, sizeof(int), 0);
                printf("Risultato inviato: %d\n", risultato);
            }
        }

        // Chiusura connessione corrente [cite: 25]
        closesocket(socket_client);
        printf("Connessione chiusa. In attesa di nuovo client...\n");
    }

    closesocket(socket_ascolto);
    WSACleanup();
    return 0;
}
