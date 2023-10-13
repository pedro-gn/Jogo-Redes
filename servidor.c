#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080         // Definição da porta que o servidor irá ouvir
#define BUFFER_SIZE 1024  // Tamanho do buffer para as mensagens

int main() {
    int sockfd;  // Identificador do socket
    char buffer[BUFFER_SIZE];  // Buffer para armazenar as mensagens recebidas/enviadas
    struct sockaddr_in serverAddr, clientAddr[2]; // Estruturas para armazenar endereço do servidor e dos dois clientes
    int addr_len = sizeof(clientAddr[0]);  // Tamanho do endereço do cliente
    char *players[2];  // Array para armazenar as jogadas dos dois jogadores
    int playerCount = 0;  // Contador para rastrear o número de jogadores que fizeram uma jogada


    int table[3][3];


    // Criação do socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Configuração do endereço do servidor
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Vinculação do socket ao endereço do servidor
    bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("Servidor inicializado na porta %d.\n", PORT);

    while (1) {
        // Recebendo a jogada do jogador
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr[playerCount], &addr_len);
        players[playerCount] = strdup(buffer);  // Armazena a jogada do jogador
        playerCount++;

        // Verifica se ambas as jogadas foram recebidas
        if (playerCount == 2) {
            char *result;

            // Determina o vencedor ou se é um empate
            if (strcmp(players[0], players[1]) == 0) {
                result = "Empate";
            } else if ((strcmp(players[0], "pedra") == 0 && strcmp(players[1], "tesoura") == 0) ||
                       (strcmp(players[0], "tesoura") == 0 && strcmp(players[1], "papel") == 0) ||
                       (strcmp(players[0], "papel") == 0 && strcmp(players[1], "pedra") == 0)) {
                result = "Jogador 1 ganhou";
            } else {
                result = "Jogador 2 ganhou";
            }

            // Envie o resultado para ambos os jogadores
            sendto(sockfd, result, strlen(result), 0, (struct sockaddr*)&clientAddr[0], addr_len);
            sendto(sockfd, result, strlen(result), 0, (struct sockaddr*)&clientAddr[1], addr_len);
            
            playerCount = 0;  // Reinicia o contador de jogadores
        }
    }

    return 0;
}
