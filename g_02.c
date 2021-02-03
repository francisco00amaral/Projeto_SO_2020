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
	int numero1=0;
    int numero2 = 0;
	int resposta = 0;
    int pontuacao=0;
    int tentativas=0;
	time_t t;
	srand((unsigned) time(&t));
	signal(SIGUSR1,recebeSIG);
	printf("------- Bem vindo ao quiz matematico! -------\n\n");
	printf("Para jogar este jogo, acerte na pergunta que ira ser atribuida.\n");

	while (1){
        int operacao = rand() % 4; // numero entre 0 e 3
        int numero1 = rand() % 100;
        int numero2 = rand() % 100;
        switch(operacao){
            case 0: // +
                printf("\n%d + %d = ",numero1,numero2);
                scanf("%d",&resposta);
            if(numero1 + numero2 == resposta){
                printf("Parabens, acertou!");
                pontuacao++;
            }
            else
                printf("Infelizmente errou...");
            break;

            case 1: // -
            printf("\n%d - %d = ",numero1,numero2);
                scanf("%d",&resposta);
            if(numero1 - numero2 == resposta){
                printf("Parabens, acertou!");
                pontuacao++;
            }
            else
                printf("Infelizmente errou...");
            break;

            case 2:
            printf("\n%d * %d = ",numero1,numero2);
                scanf("%d",&resposta);
            if((numero1 * numero2) == resposta){
                printf("Parabens, acertou!");
                pontuacao++;
            }
            else
                printf("Infelizmente errou...");
            break;
            case 3:
            printf("\n%d / %d = ",numero1,numero2);
                scanf("%d",&resposta);
            if((numero1 / numero2) == resposta){
                printf("Parabens, acertou!");
                pontuacao++;
            }
            else
                printf("Infelizmente errou...");
            break;
        }
	
	tentativas++;
}
printf("\nPrecisou de %d tentativas, com a pontuacao de %d!!!",tentativas,pontuacao);


return (pontuacao);//mudar o jogo /nao faz sentido mandar a pontuaçao
}