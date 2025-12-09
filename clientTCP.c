/*
    Esercizio: 1 - Client TCP
    Autori: Giovanni Musca, Francesco Signorile
    Corso: Reti di Calcolatori aa 2025-26
    Descrizione: Client che invia comandi e numeri al server.
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
    struct sockaddr_in server_addr;
    struct hostent *server_info;
    char nome_server[100];
    char buffer_ricezione[BUFFER_SIZE];
    char lettera_invio;
    int num1, num2, risultato;

    printf("PROGETTO RETI - CLIENT TCP\n");
    printf("Autori: Giovanni Musca, Francesco Signorile\n");
    printf("-----------------------------------------\n\n");

    // Inizializzazione Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Errore Winsock.\n");
        return 1;
    }

    // 1) Creazione socket
    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == INVALID_SOCKET) {
        printf("Errore creazione socket.\n");
        return 1;
    }

    // 2) Richiesta nome server (es. localhost) [cite: 7]
    printf("Inserisci il nome del server (es. localhost): ");
    scanf("%s", nome_server);

    // 3) Risoluzione del nome [cite: 9]
    server_info = gethostbyname(nome_server);
    if (server_info == NULL) {
        printf("Errore: Impossibile risolvere il nome del server.\n");
        return 1;
    }

    // Setup indirizzo
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = *((struct in_addr *)server_info->h_addr);
    server_addr.sin_port = htons(PORTA);

    // Connessione al server [cite: 9]
    if (connect(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Errore di connessione al server.\n");
        return 1;
    }

    // 5) Ricezione stringa "connessione avvenuta" [cite: 13]
    int byte_ricevuti = recv(socket_server, buffer_ricezione, BUFFER_SIZE, 0);
    if (byte_ricevuti > 0) {
        buffer_ricezione[byte_ricevuti] = '\0'; // Chiudo la stringa
        printf("Messaggio dal server: %s\n", buffer_ricezione);
    }

    // 6) Lettura lettera da input e invio [cite: 15]
    printf("\nScegli operazione (A=Addizione, S=Sottrazione, M=Moltiplicazione, D=Divisione): ");
    scanf(" %c", &lettera_invio); // Lo spazio prima di %c serve a pulire il buffer
    send(socket_server, &lettera_invio, sizeof(char), 0);

    // 8) Riceve la stringa inviata dal server (es. "ADDIZIONE") [cite: 23]
    byte_ricevuti = recv(socket_server, buffer_ricezione, BUFFER_SIZE, 0);
    if (byte_ricevuti > 0) {
        buffer_ricezione[byte_ricevuti] = '\0';
        printf("Operazione selezionata: %s\n", buffer_ricezione);
        
        // Controllo se il server ha risposto con TERMINE...
        if (strstr(buffer_ricezione, "TERMINE") != NULL) {
            printf("Operazione non valida. Termino il processo.\n"); // [cite: 24]
        } else {
            // Caso operazione valida: leggo due interi e li spedisco [cite: 23]
            printf("Inserisci il primo numero intero: ");
            scanf("%d", &num1);
            printf("Inserisci il secondo numero intero: ");
            scanf("%d", &num2);

            send(socket_server, (char*)&num1, sizeof(int), 0);
            send(socket_server, (char*)&num2, sizeof(int), 0);

            // 10) Riceve il risultato e lo stampa [cite: 27]
            recv(socket_server, (char*)&risultato, sizeof(int), 0);
            printf("\n--- RISULTATO RICEVUTO DAL SERVER: %d ---\n", risultato);
        }
    }

    closesocket(socket_server);
    WSACleanup();

    printf("\n\n");
    system("PAUSE");
    return 0;
}
