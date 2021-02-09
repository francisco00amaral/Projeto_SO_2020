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
	int numero1=0;
    int numero2 = 0;
	int resposta = 0;
   
	time_t t;
	srand((unsigned) time(&t));
	signal(SIGUSR1,recebeSIG);
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
}


return (pontuacao);
}