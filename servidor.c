#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir o socket");
        exit(1);
    }

    // Configuração do endereço do servidor
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Associação do socket com o endereço do servidor
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erro ao fazer bind");
        exit(1);
    }

    // Espera por conexões
    if (listen(sockfd, 5) < 0) {
        perror("Erro ao escutar por conexões");
        exit(1);
    }

    printf("Servidor aguardando conexões...\n");

    // Aceita uma conexão
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("Erro ao aceitar conexão");
        exit(1);
    }

    printf("Conexão estabelecida!\n");

    // Lê a mensagem enviada pelo cliente
    memset(buffer, 0, BUFFER_SIZE);
    if (read(newsockfd, buffer, BUFFER_SIZE - 1) < 0) {
        perror("Erro ao ler do socket");
        exit(1);
    }

    printf("Mensagem recebida do cliente: %s\n", buffer);

    // Envia uma resposta para o cliente
    const char *response = "Mensagem recebida pelo servidor!";
    if (write(newsockfd, response, strlen(response)) < 0) {
        perror("Erro ao escrever no socket");
        exit(1);
    }

    // Fecha os sockets
    close(newsockfd);
    close(sockfd);

    return 0;
}
