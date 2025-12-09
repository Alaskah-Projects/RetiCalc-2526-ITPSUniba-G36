/*
    Esercizio: 2 - Client UDP
    Autori: Giovanni Musca, Francesco Signorile
    Corso: Reti di Calcolatori aa 2025-26
    Descrizione: Client UDP che comunica col server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORTA 8888
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET socket_client;
    struct sockaddr_in server_addr;
    struct hostent *server_info;
    int dimensione_server = sizeof(server_addr);
    char nome_server[100];
    char buffer_ricezione[BUFFER_SIZE];
    char buffer_handshake[] = "HELLO"; // Messaggio per attivare il server
    char lettera_invio;
    int num1, num2, risultato;

    printf("PROGETTO RETI - CLIENT UDP\n");
    printf("Autori: Giovanni Musca, Francesco Signorile\n");
    printf("-----------------------------------------\n\n");

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Errore Winsock.\n");
        return 1;
    }

    // 1) Creazione socket UDP
    socket_client = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_client == INVALID_SOCKET) {
        printf("Errore creazione socket.\n");
        return 1;
    }

    // 2) Richiesta nome server
    printf("Inserisci il nome del server (es. localhost): ");
    scanf("%s", nome_server);

    // 3) Risoluzione nome
    server_info = gethostbyname(nome_server);
    if (server_info == NULL) {
        printf("Errore risoluzione nome server.\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = *((struct in_addr *)server_info->h_addr);
    server_addr.sin_port = htons(PORTA);

    // --- FASE HANDSHAKE SIMULATA ---
    // Invio un pacchetto iniziale per dire al server "Esisto, ecco il mio IP"
    sendto(socket_client, buffer_handshake, strlen(buffer_handshake), 0, (struct sockaddr*)&server_addr, dimensione_server);

    // 5) Ricevo "connessione avvenuta"
    int byte_ricevuti = recvfrom(socket_client, buffer_ricezione, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &dimensione_server);
    if (byte_ricevuti > 0) {
        buffer_ricezione[byte_ricevuti] = '\0';
        printf("Messaggio dal server: %s\n", buffer_ricezione);
    }

    // 6) Scelta operazione
    printf("\nScegli operazione (A=Addizione, S=Sottrazione, M=Moltiplicazione, D=Divisione): ");
    scanf(" %c", &lettera_invio);
    
    // Invio lettera
    sendto(socket_client, &lettera_invio, sizeof(char), 0, (struct sockaddr*)&server_addr, dimensione_server);

    // 8) Ricezione tipo operazione (es. "ADDIZIONE")
    byte_ricevuti = recvfrom(socket_client, buffer_ricezione, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &dimensione_server);
    if (byte_ricevuti > 0) {
        buffer_ricezione[byte_ricevuti] = '\0';
        printf("Operazione selezionata: %s\n", buffer_ricezione);

        if (strstr(buffer_ricezione, "TERMINE") != NULL) {
            printf("Operazione non valida. Termino il processo.\n");
        } else {
            // Lettura e invio numeri
            printf("Inserisci il primo numero intero: ");
            scanf("%d", &num1);
            printf("Inserisci il secondo numero intero: ");
            scanf("%d", &num2);

            sendto(socket_client, (char*)&num1, sizeof(int), 0, (struct sockaddr*)&server_addr, dimensione_server);
            sendto(socket_client, (char*)&num2, sizeof(int), 0, (struct sockaddr*)&server_addr, dimensione_server);

            // 10) Ricezione Risultato
            recvfrom(socket_client, (char*)&risultato, sizeof(int), 0, (struct sockaddr*)&server_addr, &dimensione_server);
            printf("\n--- RISULTATO RICEVUTO DAL SERVER: %d ---\n", risultato);
        }
    }

    closesocket(socket_client);
    WSACleanup();

    printf("\n\n");
    system("PAUSE");
    return 0;
}
