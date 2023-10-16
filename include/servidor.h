#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>    
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <netdb.h>


#define PORTA_PADRAO 4343

//Definindo um tipo boolean;
typedef enum boolean{false, true}bool;

// Variáveis globais
int Matriz_Jogo    [3][3];  // Matriz representando o jogo da velha
int Coord_Digitadas[9];     // Coordenadas já escolhidas no jogo
int jogada;                 // Contador de jogadas


int  Erro(char mensagem[], int returno);         //Funcao generica para impressar de mensagem de erro e retorno de inteiro
void StatusJogo(int, int, int, int);             //Imprime no tela, do servidor o status do jogo: matriz, jogada, realizador da jogada;

/*Funcoes para estabelecimento de conexao  */
bool IniciaSocketServidor(int *);               // Cria o Socket do Servidor, vincula e ouve uma porta[funções: bind() e listen];
bool IniciaSocketClientes(int *, int *, int *); // Aceita os clientes[funcao: accept()];

/*Funcoes para formatacao do pacote enviado*/
void Encapsular      (char *, int);             // Gera um pacote para ser enviado
void EncapsularMatriz(char pacote[]);           // Encapsula matriz no jogo, funcao chamada por "Encapsular()"
void IntParaChar     (int, char char_gerado[]); // Converte um inteiro > 9 em um char, ex. int n = 129102 -> char m[7] ={"129102"};
int  CharParaInt     (char);                    // Converte um char em um inteiro;
char GetCaracter     (int);                     // Obtem um caractere a partir da tabela ASCII, ex. int n = 48 -> char m = '1';

/* Função principal que trata de intercalar os clientes realizar uma jogada, reenvia o pacote solicitando uma nova jogada quando
  invalida e determina o vencedor */
void JogadaClientes(int *, int *, int *, int *, int *);
void ImprimeMatriz(void);

/*Funções para manipulacao da matriz do jogo*/
bool ValidaCoordenada      (int coordenada);            // Verifica se a coordenada solicitada esta entre 1 e 9 e tambem se nao ja foi digitada anteriormente
bool VerifCoordJaDigitada  (int aux);                   // Usada para verificar se a coordenada ja foi digitada
int  VerificaFimMatriz     (int jogadas);               // Verifica se ja se tem um ganhador ou ocorreu um empate
int  SomaCoordenadas       (int x, int y, int z);       // Na matriz eh preenchido com valores 1 e -1 dependendo do jogador, entao essa funcao soma os
                                                        //valores para posteriormente verificar se ha a soma eh 3 ou -3, significando que ha um ganhador
bool VerificaSomaCoord     (int aux);                   // Verifica se a soma eh 3 ou -3
void ConverteEmCoordenadaXY(int coord, int *i, int *j); // Como as coordenadas sao solicitada por numeros de 1 a 9, essa funcao converte em coordenadas
                                                        //i e j para alterar a matriz do jogo;
