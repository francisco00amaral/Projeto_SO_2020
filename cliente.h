
// #ifndef CLIENTE_H
#define CLIENTE_H

typedef struct cliente Cliente;
typedef struct cliente *pcliente;

typedef struct{
	char nome[20];
	int jogo; //jogo vai ser 1 ou 2 ou 3;
	int pid;
	char comando[20];
	int curioso;
	int quit; // 1 sai 0 continua
	int ativo;
    int emJogo;
	int jaExiste;
}Jogador;

struct cliente{
	char nome[20];
	int jogo; //jogo vai ser 1 ou 2 ou 3;
	int pid;
	char comando[20];
	int curioso;
	int quit; // 1 sai 0 continua
	int ativo;
    int emJogo;
	int jaExiste;
	pcliente prox;
};



// #endif CLIENTE_H
