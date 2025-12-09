/*
    Esercizio: 2 - Server UDP
    Autori: Giovanni Musca, Francesco Signorile
    Corso: Reti di Calcolatori aa 2025-26
    Descrizione: Server UDP che gestisce operazioni matematiche.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORTA 8888
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET socket_server;
    struct sockaddr_in server_addr, client_addr;
    int dimensione_client;
    char buffer_invio[BUFFER_SIZE];
    char buffer_ricezione[BUFFER_SIZE]; // Usato per l'handshake iniziale
    char lettera_ricevuta;
    int num1, num2, risultato;

    printf("PROGETTO RETI - SERVER UDP\n");
    printf("Autori: Giovanni Musca, Francesco Signorile\n");
    printf("-----------------------------------------\n\n");

    // Inizializzazione Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Errore Winsock.\n");
        return 1;
    }

    // 1) Creazione del socket UDP (SOCK_DGRAM)
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_server == INVALID_SOCKET) {
        printf("Errore creazione socket.\n");
        return 1;
    }

    // Setup indirizzo server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORTA);

    // 2) Bind
    if (bind(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Errore nel Bind.\n");
        return 1;
    }

    printf("Server UDP avviato sulla porta %d.\nIn attesa di pacchetti...\n", PORTA);

    // Ciclo infinito
    while (1) {
        dimensione_client = sizeof(client_addr);

        // --- SIMULAZIONE CONNESSIONE ---
        // In UDP non c'è accept. Aspettiamo un primo pacchetto "HELLO" dal client
        // per capire chi è e rispondergli "connessione avvenuta".
        
        int byte_rx = recvfrom(socket_server, buffer_ricezione, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &dimensione_client);
        if (byte_rx == SOCKET_ERROR) {
            printf("Errore nella ricezione handshake.\n");
            continue;
        }

        printf("\nNuovo client rilevato (IP: %s)\n", inet_ntoa(client_addr.sin_addr));

        // 4) Invia "connessione avvenuta"
        strcpy(buffer_invio, "connessione avvenuta");
        sendto(socket_server, buffer_invio, strlen(buffer_invio) + 1, 0, (struct sockaddr*)&client_addr, dimensione_client);

        // 7) Ricezione lettera (Comando)
        // Nota: client_addr è già impostato dalla recvfrom precedente, ma recvfrom lo aggiorna ad ogni chiamata.
        recvfrom(socket_server, &lettera_ricevuta, sizeof(char), 0, (struct sockaddr*)&client_addr, &dimensione_client);
        printf("Lettera ricevuta: %c\n", lettera_ricevuta);

        int operazione_valida = 1;

        // Logica selezione stringa
        if (lettera_ricevuta == 'A' || lettera_ricevuta == 'a') {
            strcpy(buffer_invio, "ADDIZIONE");
        } else if (lettera_ricevuta == 'S' || lettera_ricevuta == 's') {
            strcpy(buffer_invio, "SOTTRAZIONE");
        } else if (lettera_ricevuta == 'M' || lettera_ricevuta == 'm') {
            strcpy(buffer_invio, "MOLTIPLICAZIONE");
        } else if (lettera_ricevuta == 'D' || lettera_ricevuta == 'd') {
            strcpy(buffer_invio, "DIVISIONE");
        } else {
            strcpy(buffer_invio, "TERMINE PROCESSO CLIENT");
            operazione_valida = 0;
        }

        // Invio stringa operazione
        sendto(socket_server, buffer_invio, strlen(buffer_invio) + 1, 0, (struct sockaddr*)&client_addr, dimensione_client);

        // 9) Calcolo (se valido)
        if (operazione_valida) {
            // Ricevo i due numeri
            recvfrom(socket_server, (char*)&num1, sizeof(int), 0, (struct sockaddr*)&client_addr, &dimensione_client);
            recvfrom(socket_server, (char*)&num2, sizeof(int), 0, (struct sockaddr*)&client_addr, &dimensione_client);
            
            printf("Numeri ricevuti: %d e %d. Calcolo in corso...\n", num1, num2);

            if (lettera_ricevuta == 'A' || lettera_ricevuta == 'a') risultato = num1 + num2;
            else if (lettera_ricevuta == 'S' || lettera_ricevuta == 's') risultato = num1 - num2;
            else if (lettera_ricevuta == 'M' || lettera_ricevuta == 'm') risultato = num1 * num2;
            else if (lettera_ricevuta == 'D' || lettera_ricevuta == 'd') {
                if(num2 != 0) risultato = num1 / num2;
                else risultato = 0; 
            }

            // Invio risultato
            sendto(socket_server, (char*)&risultato, sizeof(int), 0, (struct sockaddr*)&client_addr, dimensione_client);
            printf("Risultato inviato: %d\n", risultato);
        }
        
        printf("Sessione UDP terminata (logica). Torno in attesa.\n");
    }

    closesocket(socket_server);
    WSACleanup();
    return 0;
}
