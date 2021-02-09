#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include "utils.h"	
int pontuacao=0;

void recebeSIG(int sig)
{	
	exit(pontuacao);
}

int main(){
	setbuf(stdout,NULL);
	int numero=0;
	int c;
	time_t t;
	srand((unsigned) time(&t));
	signal(SIGUSR1,recebeSIG);
	printf("------- Bem vindo ao jogo da roleta! -------\n\n");
	printf("Para jogar este jogo, simplesmente escolha um numero de 0 a 5\n");
	while (1){
	do{
	scanf("%d",&numero);
	}while(numero < 0 || numero > 5);

	c = rand() % 5;
	if(numero == c){
		printf("\nParabéns,acertou no numero!!!");
		pontuacao++;
	}
	else
		printf("\nErrou...try again!!!");
	}

	return (pontuacao);
}