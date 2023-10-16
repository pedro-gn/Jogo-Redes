#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>//close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORTA_PADRAO 4343

#define INICIO_MATRIZ  1
#define INICIO_TAM_MSG 10
#define INICIO_MSG     12


//Definindo um tipo boolean;
typedef enum boolean{false, true}bool;

//Exibe mensagem de erro e fornece um retorno;
int Erro(char mensagem[], int);

//Estabelece Conexao com o Servidor
void IniciaSocket(int *, char ip[]);

//Verifica se a coordenada foi recusada;
bool VerificaRecuso(char c);

//Verifica se o servidor enviou uma mensagem de fim de jogo;
bool AnalisaFimPacote(char pacote[]);

void Desencapsular(char pacote[], char matriz_jogo[], char mensagem_servidor[]);
int  ConvertChar  (char);
void CharParaInt  (char pacote[], int *);
void ImprimirRecebido(char matriz_jogo[], char mensagem_servidor[]);