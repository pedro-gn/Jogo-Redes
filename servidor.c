#include <stdio.h>      //Para uso do entrada e saida de dados em alto nivel;
#include <stdlib.h>     //Para uso do "system()"
#include <string.h>     //Manipulacao de strings em geral: "strcpy()", "strcmp()", "strcat()"
#include <unistd.h>     //Para a uso da funcao "close()"
#include <sys/types.h>  //Funcoes de temporizacao
#include <sys/socket.h> //Uso e manipulacao de Sockets
#include <netinet/in.h> //Define a estrutura "sockaddr_in"
#include <arpa/inet.h>  //Disponibiliza o tipo in_port_t e in_addr_t para uso da <netinet/in.h>
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

int main(){
	int sock_programa, i, j, cont=0;
	int sock_cliente_1, sock_cliente_2;

    //  Função retorna um bool para o inicio do servidor,
    // dentro da fucao e realidazo um bind e um listen no
    // socket passado por referencia;
	if(!IniciaSocketServidor(&sock_programa))
		return Erro("Falha ao manipular a variavel sock_programa!", -1);


    //  Função retorna um bool para o inicio dos clientes,
    // dentro da fucao e realidazo um accept para cada cliente;
	if(!IniciaSocketClientes(&sock_programa, &sock_cliente_1, &sock_cliente_2))
		return Erro("Falha no accept()", -1);


	int  qtd_jogadas  = 0;
	int  verifica_fim = 0;
	int  teste_erro   = 0;
	char pacote_envio[100];

    /*Inicio do Jogo*/
    Encapsular(pacote_envio, 0);/*Enviando para ambos aguardar*/
    teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
    teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);

    Encapsular(pacote_envio, 5);/*sock_cliente_1 faz a primeira jogada*/
    teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
	do{
        // Ouve e realiza a jogada de sock_cliente_1
        JogadaClientes(&sock_cliente_1, &sock_cliente_2, &qtd_jogadas, &teste_erro, &verifica_fim);
        StatusJogo(1, 2,qtd_jogadas, 0);

        // Ouve e realiza a jogada de sock_cliente_2
        if(!(verifica_fim==9 || verifica_fim==3 || verifica_fim==-3))
            JogadaClientes(&sock_cliente_2, &sock_cliente_1, &qtd_jogadas, &teste_erro, &verifica_fim);
        StatusJogo(2, 1, qtd_jogadas, 0);

        //Força a saida While quando ocorre o fim do jogo;
        if(verifica_fim==9 || verifica_fim==3 || verifica_fim==-3)
            break;
    }while(teste_erro!=-1);

    if(verifica_fim == 9){
        StatusJogo(2, 1, qtd_jogadas, 3);
        Encapsular(pacote_envio, 4);//Envia mensagem para ambos informando que Deu Velha;
        teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
        teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);
    }else if(verifica_fim == 3){
        StatusJogo(2, 1, qtd_jogadas, 2);
        Encapsular(pacote_envio, 2);//Envia mensagem ao sock_cliente_2 informando que ele Ganhou
        teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);
        Encapsular(pacote_envio, 3);//Envia mensagem ao sock_cliente_1 informando que ele Perdeu
        teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
    }else if(verifica_fim == -3){
        StatusJogo(2, 1, qtd_jogadas, 1);
        Encapsular(pacote_envio, 2);//Envia mensagem ao sock_cliente_1 informando que ele Ganhou
        teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
        Encapsular(pacote_envio, 3);//Envia mensagem ao sock_cliente_2 informando que ele Perdeu
        teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);
    }
    system("sleep 2");
	close(sock_cliente_1);
	close(sock_cliente_2);
	close(sock_programa);
	return 0;
}

int Erro(char mensagem[], int returno){
    printf("\n%s\n", mensagem);
    return returno;
}

void StatusJogo(int origem, int destino, int numero_da_jogada, int status){
    printf("\n+-----------------------------------------------+\n");
    printf("\n           ---STATUS DO JOGO---\n\n");
    if(status==0){
        printf("\n Realizou Jogada              >>  sock_cliente_%d", origem);
        printf("\n Aguardando                   >>  sock_cliente_%d", origem);
        printf("\n Proximo realizar uma Jogada  >>  sock_cliente_%d", destino);
    }else if(status==1){
        printf("\n GANHOU O JOGO                >>  sock_cliente_1");
        printf("\n PERDEU O JOGO                >>  sock_cliente_2");
        printf("\n FIM                          >>  FIM DE JOGO");
    }else if(status==1){
        printf("\n GANHOU O JOGO                >>  sock_cliente_1");
        printf("\n PERDEU O JOGO                >>  sock_cliente_2");
        printf("\n FIM                          >>  FIM DE JOGO");
    }else{
        printf("\n GANHOU O JOGO                >>  EMPATE");
        printf("\n PERDEU O JOGO                >>  EMPATE");
        printf("\n FIM                          >>  FIM DE JOGO");
    }

    printf("\n Numero da Jogada             >>  %d\n", numero_da_jogada);
    printf(" MATRIZ:\n");
    ImprimeMatriz();
    printf("\n+-----------------------------------------------+\n");
    printf("\n");
}

bool IniciaSocketServidor(int *sock_programa){
	struct sockaddr_in addr;

	*sock_programa = socket(AF_INET,SOCK_STREAM,0);
	if(*sock_programa == -1){
		printf("Erro ao criar o socket!\n");
		return false;
	}

	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(PORTA_PADRAO);
	addr.sin_addr.s_addr = INADDR_ANY;
	memset(&addr.sin_zero,0,sizeof(addr.sin_zero));

	if(bind(*sock_programa,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		printf("Erro na funcao bind()\n");
		return false;
	}

	if(listen(*sock_programa,1) == -1){
		printf("Erro na funcao listen()\n");
		return false;
	}

	return true;
}

bool IniciaSocketClientes(int *sock_programa, int *sock_cliente_1, int *sock_cliente_2){

	printf("Aguardando clientes...\n");
	*sock_cliente_1 = accept(*sock_programa,0,0);
	printf("sock_cliente_1 - Cliente 1 aceito!\n");

	*sock_cliente_2 = accept(*sock_programa,0,0);
	printf("sock_cliente_2 - Cliente 2 aceito!\n");

    // Retorna false por nao ter iniciado corretamente;
    // Retorna true por iniciar corretamente;
	if(*sock_cliente_1 == -1 || *sock_cliente_2 == -1)
        return false;
    else
        return true;
}

void Encapsular(char *pacote, int id_mensagem){
	int  tam = 0;    //tamanho da mensagem presente no campo mensagem
	char tam_char[4];//tam convertido em caracteres
	int i;
	char mensagem_servidor[50];

    // Campo do pacote que e armazenado uma coordenada,
    // e por ser o servidor o remetente, nao e necessario
    // armazenar qualquer coordenada;
    strcpy(pacote, "-");

    // Campo do pacote onde sera armazenada a matriz do jogo,
    // sera colocado um 'X' para o valor inteiro -1(um negativo),
    // 'O' para valor o inteiro 1(um positivo) e '*' para o
    // valor inteiro 0(zero);
	EncapsularMatriz(pacote);

    switch(id_mensagem){
        case 0:
            strcpy(mensagem_servidor, "Aguarde sua vez para jogar!");
        break;
        case 1:
            pacote[0] = 'x';
            strcpy(mensagem_servidor, "Coordenada Invalida Solicitada!");
        break;
        case 2:
            strcpy(mensagem_servidor, "FIM DE JOGO, VOCE GANHOU!!!");
        break;
        case 3:
            strcpy(mensagem_servidor, "FIM DE JOGO, VOCE PERDEU!!!");
        break;
        case 4:
            strcpy(mensagem_servidor, "FIM DE JOGO,DEU VELHA!!!");
        break;
        case 5:
            strcpy(mensagem_servidor, "Faça uma jogada!");
        break;
    }
    // Após converter o tamanho do campo mensagem do pacote que sera enviada em char
    // eh inserido no pacote este char gerado;
    tam = strlen(mensagem_servidor);
    IntParaChar(tam, tam_char);
    strcat(pacote, tam_char);

    // Inclui no pacote a mensagem do servidor;
    strcat(pacote, mensagem_servidor);

    // Preenche o restante do pacote com '-'
	for(i=1+9+tam+1;i<100; i++)
		strcat(pacote, "-");

	pacote[100] = '\0';
}

void EncapsularMatriz(char pacote[]){
	int i, j;

	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			if(Matriz_Jogo[i][j] == 0)
				strcat(pacote, "*");
			else if(Matriz_Jogo[i][j] == 1)
				strcat(pacote, "O");
			else if(Matriz_Jogo[i][j] == -1)
				strcat(pacote, "X");
		}
	}
}

void IntParaChar(int n, char char_gerado[]) {
	int  resto = 0;
	int  cont  = 0;
	char num[2];

	while(n > 0){
        resto = n%10;//Atribui somente a parte inteira;
        n = n/10;    //Atualiza

		num[cont] = (char)GetCaracter(resto);//Fica invertido essa atribuição
		cont++;
	}
	char_gerado[0] = num[1];
	char_gerado[1] = num[0];
	char_gerado[2] = '\0';
}

int CharParaInt(char n){
    return (n)-48;
}

char GetCaracter(int a){
	int b = a+48;
	return (char)b;
}

void JogadaClientes(int *sock_origem, int *sock_destino, int *qtd_jogadas, int *teste_erro, int *verifica_fim){
    int  coordenada, coord_x, coord_y;
    char pacote_receber[100];
    char pacote_envio[100];
    bool coordenada_valida = true;

    do{
        *teste_erro = recv(*sock_origem,pacote_receber,100,0);// Recebe pacote do cliente;

        //Se nao houver erro e ainda nao for o fim jogo,
        if(*teste_erro!=-1 && !(*verifica_fim == 9 || *verifica_fim == -3 || *verifica_fim == 3)){
            coordenada = CharParaInt(pacote_receber[0]);
            coordenada_valida = ValidaCoordenada(coordenada);
            coordenada = coordenada - 1;

            if(coordenada_valida){/*Caso a coordenada solicitada seja valida*/
                ConverteEmCoordenadaXY(coordenada, &coord_x, &coord_y);//Obtem a coordenada em x e y da matriz

                //De acordo com o numero da jogada é possivel alterar a matriz com o numero correspondente;
                if(*qtd_jogadas%2==0)
                    Matriz_Jogo[coord_x][coord_y] = -1;
                else
                    Matriz_Jogo[coord_x][coord_y] = 1;

                *qtd_jogadas = *qtd_jogadas + 1;

                *verifica_fim = VerificaFimMatriz(*qtd_jogadas);
                if(!(*verifica_fim == 9 || *verifica_fim == -3 || *verifica_fim == 3)){
                    //Envia para o destino a matriz alterada pela origem;
                    Encapsular(pacote_envio, 5);//Mensagem solicitando uma jogada;
                    *teste_erro = send(*sock_destino, pacote_envio, strlen(pacote_envio),0);
                }else{
                    //Coordenada_valida recebe true para sair do laço;
                    coordenada_valida = true;
                }

                // Envia para a origem a matriz que ele acabou de alterar
                // assim ela poder observar sua coordenada alterada;
                 Encapsular(pacote_envio, 0);//Mensagem dizendo para aguardar.
                *teste_erro = send(*sock_origem, pacote_envio, strlen(pacote_envio),0);

            }else{/*Caso a coordenada solicitada nao seja valida*/
                //Reenvia o pacote solicitando uma coordenada correta;
                 Encapsular(pacote_envio, 1);//Encapsula com mensagem dizendo que a coordenada eh invalida;
                *teste_erro = send(*sock_origem,pacote_envio,strlen(pacote_envio),0);  /* Envia a string */
            }
        }
        *verifica_fim = VerificaFimMatriz(*qtd_jogadas);
        // Como pode ter ocorrido um erro ou o fim do jogo,atribui-se
        // true para a coordenada_valida, para sair do laço;
        if(*teste_erro == -1 || *verifica_fim == 9 || *verifica_fim == -3 || *verifica_fim == 3)
            coordenada_valida = true;

    }while(!coordenada_valida);
}

void ImprimeMatriz(void){
    int i, j;

    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            if(Matriz_Jogo[i][j] == 1)
                printf(" O ");
            else if (Matriz_Jogo[i][j] == -1)
                printf(" X ");
            else
                printf(" - ");
        }
        printf("\n");
    }
}

bool ValidaCoordenada(int coordenada){
	if(coordenada>9 || coordenada <1)
		return false;
	else if(VerifCoordJaDigitada(coordenada))
		return false;

	return true;
}

bool VerifCoordJaDigitada(int aux){
    int i;

    for(i=0; i<9; i++){
        if(Coord_Digitadas[i] == aux){
            return true;
        }
    }
    for(i=0; i<9; i++){
        if(Coord_Digitadas[i] == 0){
            Coord_Digitadas[i] = aux;
            return false;
        }
    }
}

int VerificaFimMatriz(int jogadas){
    int diagonal[2], vertical[3], horizontal[3], i;

    vertical  [0] = vertical  [1] = vertical  [2] = 0;
    horizontal[0] = horizontal[1] = horizontal[2] = 0;
    diagonal  [0] = diagonal  [1] = 0;

    horizontal[0] = SomaCoordenadas(0,1,2);
    horizontal[1] = SomaCoordenadas(3,4,5);
    horizontal[2] = SomaCoordenadas(6,7,8);

    vertical[0] = SomaCoordenadas(0,3,6);
    vertical[1] = SomaCoordenadas(1,4,7);
    vertical[2] = SomaCoordenadas(2,5,8);

    diagonal[0] = SomaCoordenadas(0,4,8);
    diagonal[1] = SomaCoordenadas(2,4,6);


    for(i=0; i<3; i++){
        if(VerificaSomaCoord(horizontal[i])){
            return horizontal[i];
        }
        if(VerificaSomaCoord(vertical[i])){
            return vertical[i];
        }
        if(i<2){
            if(VerificaSomaCoord(diagonal[i])){
                return diagonal[i];
            }
        }
    }

    if(jogadas == 9)
        return 9;

    return 0;
}

int SomaCoordenadas(int x, int y, int z){
    int i, j, cont=0;

    ConverteEmCoordenadaXY(x, &i, &j);
    cont = cont + Matriz_Jogo[i][j];
    ConverteEmCoordenadaXY(y, &i, &j);
    cont = cont + Matriz_Jogo[i][j];
    ConverteEmCoordenadaXY(z, &i, &j);
    cont = cont + Matriz_Jogo[i][j];

    return cont;
}

bool VerificaSomaCoord(int aux){
    if(aux==3 || aux==-3){
        return true;
    }else{
        return false;
    }
}

void ConverteEmCoordenadaXY(int coord, int *i, int *j){
    *i = (coord/3);
    *j = (coord%3);
}