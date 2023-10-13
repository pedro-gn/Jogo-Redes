#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 8080   // Porta do servidor para se conectar
#define BUFFER_SIZE 1024   // Tamanho do buffer para mensagens

int main() {
    int sockfd;  // Identificador do socket
    char buffer[BUFFER_SIZE];  // Buffer para armazenar mensagens enviadas/recebidas
    struct sockaddr_in serverAddr;  // Estrutura para armazenar o endereço do servidor
    int addr_len = sizeof(serverAddr);  // Tamanho do endereço do servidor

    // Criação do socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Configuração do endereço do servidor
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Coleta a escolha do jogador
    printf("Digite sua escolha (pedra, papel, tesoura): ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;  // Remove o '\n' do final

    // Envia a escolha para o servidor
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // Aguarda e recebe o resultado do servidor
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    printf("Resultado: %s\n", buffer);

    return 0;
}
