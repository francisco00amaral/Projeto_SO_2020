#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include "utils.h"	

void recebeSIG(int sig)
{	
	printf("vai ser encerrado!!!");
	kill(getpid(),SIGKILL);
	
}


int main(){
	int numero=0;
	int c,pontuacao=0,tentativas=0;
	time_t t;
	srand((unsigned) time(&t));
	signal(SIGUSR1,recebeSIG);
	printf("------- Bem vindo ao jogo da roleta! -------\n\n");
	while (pontuacao<3){

	
	printf("Para jogar este jogo, simplesmente escolha um numero de 0 a 5\n");
	printf("Introduza o seu numero no intervalo dito em cima: ");
	do{
	scanf("%d",&numero);
	}while(numero < 0 || numero > 5);

	c = rand() % 5;
	tentativas++;
	if(numero == c){
		printf("\nParabéns,acertou no numero!!!");
		pontuacao++;
	}
	else
		printf("\nErrou...try again!!!");
}
printf("\nParabéns,ganhou o jogo precisando de %d tentativas, com a pontuacao de %d!!!",tentativas,pontuacao);


	return (pontuacao);//mudar o jogo /nao faz sentido mandar a pontuaçao
}