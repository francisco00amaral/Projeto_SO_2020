#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <limits.h>
#include <fcntl.h>
#include <dirent.h>
#include <dirent.h> 
#include "utils.h"

//##########################################################################################################
//
//		Falta o pipe para enviar o filho ou seja os jogos para o cliente e receber os resultados
//
//
//##########################################################################################################
//#include "arbitro.h"
#define MAXP 5
int mp=0;
int duracaoCampeonato,tempoEspera;



void verificaAmbiente(){

}

//mandar os jogos para os clientes
//provavelmente meter numa thread
void funcaoFork(char *nome){
	int res;
	int pid;
	//pid=getpid();
	//printf("\npid:%d",pid);
	pid=getpid();
	int canal[2];
	int continua = 1;
	int n=0;
	int estado;

	

	while(continua){
		printf("[%5d]comando:",pid);
		fflush(stdout);
		pipe(canal);
		res=fork();

		if(res==0){
			pid=getpid();
			printf("sou o filho [%5d]!!",pid);
			//enganar o jogo ,redirecionamento;
			/*close(canal[0]);
			close(1);
			dup(canal[1]);
			close(canal[1]);*/
			fflush(stdout);

			execl(nome,nome,NULL);
			printf("erroa executar jogo[%5d]",pid);
			exit(7);

		}

		printf("\n\n[%5d]pai, criei of filho pid=%d!!",pid,res);
		continua=0;
		//receber informaçao do jogo stdout
		close(canal[1]);
		while(n=read(canal[0],nome,200)>0){
			nome[n]='\0';
			printf("\narbitro recebi %s\n",nome);

		}
		close(canal[0]);
		wait(&estado);
		if(WIFEXITED(estado)){
			printf("\nArbitro jogador terminou com -%d-",WEXITSTATUS(estado));
		}


	}
}

void trataSig(int i){
	fprintf(stderr,"\nServidor do arbitro a terminar\n");
	// close(fd); ?
	// unlink(fd); ?
	exit(EXIT_SUCCESS);
}

////////////////////////-threads-/////////////////////////////////////
//fazer duas threds(uma para cada variavel de ambiente)
void * Duracao(void *dados){
	sleep(duracaoCampeonato);



}
///////////////////////////////////////////////////////////////////7

int verificaNome(pcliente lista,Cliente pessoa){
	pcliente aux;
	aux = lista;
		if(aux == NULL){
			printf("Nao ha jogadores em jogo!\n");
			return 1;
	}
	while (aux != NULL)
	{
		if(strcmp(aux->nome,pessoa.nome) == 0 && aux->pid == pessoa.pid){
			return 1;
		}
		else if(strcmp(aux->nome,pessoa.nome) == 0){
			printf("Já existe um jogador com esse nome, não irá jogar!");
		}
		aux = aux->prox;
	}
	return 1;
}

pcliente adicionaLista(pcliente lista,Cliente pessoa,int *npessoas){
		pcliente aux;
		pcliente novo;

        
        if(lista == NULL) { // Lista vazia
        lista = novo;
	}
    else{
        aux = lista;
        while(aux!= NULL){
        	if(aux->pid==pessoa.pid)
        		return lista;
        	if(aux->prox!=NULL){
            aux = aux->prox;
        }
            else{
            break;
        	}
        	
        }
        //aux->prox = novo;
    }

        int a = verificaNome(lista,pessoa);

		if(a == 0){
		kill(pessoa.pid,SIGINT);
		return lista;
		}
           
        //aux->prox = novo;
    


		if(*npessoas >= mp){
			printf("Numero de jogadores chegou ao limite, %d",mp);
			return lista;
		}

		

		novo = malloc(sizeof(pessoa));
		if(novo == NULL){
			fprintf(stderr, "Nao foi possivel alocar memoria\n");
        	abort();
		}

		novo->ativo = pessoa.ativo;
		novo->jogo = pessoa.jogo;
		strcpy(novo->nome,pessoa.nome); 
		novo->pid = pessoa.pid;
	    novo->prox = NULL; // n sei se isto ta bem

	if(lista == NULL) { // Lista vazia
        lista = novo;
	}
    else{
        aux = lista;
        while(aux->prox != NULL)
            aux = aux->prox;
        aux->prox = novo;
    }

	(*npessoas)++;

	
	return lista;
}


// falta isto
// falta isto
// falta isto
// falta isto

void mostraJogos(){
	 struct dirent *de;  // Pointer for directory entry 
   
    DIR *dr = opendir("/home/francisco/Desktop/SO_TP/SO_Projeto2020/jogos"); // compor depois,isto so funciona no meu pc por causa diretoria
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return; 
    } 
    while ((de = readdir(dr)) != NULL) 
            printf("%s\n", de->d_name); 
  
    closedir(dr);     

}

int descobreJogo(pcliente lista,char *nome){
		pcliente aux;
		aux = lista;

		if(aux == NULL){
			printf("Não ha jogadores ativos!\n");
			return 0;
		}
		while (aux != NULL)
		{
			if(strcmp(aux->nome,nome) == 0)
				return aux->jogo;
		aux = aux->prox;
		}
	return 0;
}

void mostraJogadores(pcliente jogador){
	pcliente aux;
	aux = jogador;

	if(aux == NULL){
		printf("Não ha jogadores ativos!\n");
		return;
	}
	printf("Lista de jogadores: \n");
	while (aux != NULL)
	{
		printf("Jogador [%s] com o pid de [%d]\n",aux->nome,aux->pid);
		aux = aux->prox;
	}
}



pcliente kickJogador(char *nome,pcliente lista,int *numJogadores){
	pcliente aux,guarda;
	pcliente ant = NULL;

	aux = lista;
	if(aux == NULL){
		printf("Nao ha jogadores em jogo!\n");
		return NULL;
	}

	int save;
	int flag=0;
	
while(aux != NULL)
    {
    	//printf("\n pid:%d nome:%s",aux->pid,aux->nome);
        if(strcmp(nome, aux->nome) == 0){
			save = aux->pid;
			flag = 1;
			guarda = aux->prox;
			break;
		}
		aux=aux->prox;
        //ant = aux;
       // aux = aux->prox;
    }
    if(aux->prox==NULL&&aux==lista){//para garantir que ta no primeiro
    	if(strcmp(nome, aux->nome) == 0)
    	{
    		free(aux);
    		lista=NULL;
    		kill(save,SIGUSR1);
    		return lista;
    	}
    }else if(aux->prox==NULL&&aux==lista){

    }
    else{

    aux=lista;
    pcliente liberta;
    while(aux != NULL)
    {

        if(strcmp(nome, aux->prox->nome) == 0){//

        	liberta=aux->prox;
			aux->prox=guarda;
			free(liberta);
			break;
		}
		aux=aux->prox;
       
    }
	}
	//printf("\npid",save);
	kill(save,SIGUSR1);
    
	(*numJogadores)--;
   return lista;
} 



int main(int argc,char **argv){
	
	time_t c;
	srand((unsigned) time(&c));
	int numJogadores = 0;
	int i=0;
	int t;
	

	if(argc!=3||
		sscanf(argv[1],"%d",&duracaoCampeonato)!=1||
		sscanf(argv[2],"%d",&tempoEspera)!=1){//pq a chamada do programa ja é um argumento
		printf("sintaxe: %s duracao_do_campeonato tempo_de_espera\n\n",argv[0]);
		return (EXIT_FAILURE);
	}
  pthread_t Duracao_do_Campeonato;
  pthread_create(&Duracao_do_Campeonato, NULL, Duracao, NULL);


	// METER ISTO NUMA FUNCAO
	char *maxplayer;
	maxplayer = getenv("MAXPLAYER");
	if(maxplayer!= NULL){
		mp = atoi(maxplayer);
		if(mp>30)
			mp=30;
		else if(mp==0)
		return (EXIT_FAILURE);
	}else{
		mp=MAXP;//como nao conseguio obter a variavel de ambiente toma o valor de maxp por default
		printf("\nMaximo de jogadores por omissao=%d",mp);
	}

	char *gamedir;
	gamedir=getenv("GAMEDIR");
	if(gamedir==NULL){
		char cwd[100];
		gamedir = getcwd(cwd,sizeof(cwd));

	}
	printf("\ndiretoria:%s",gamedir);

	printf("\n\nNumero maximo de jogadores e de :-%d-\n",mp);
	printf("duracao do campeonato:%d\ntempo:%d\n\n",duracaoCampeonato,tempoEspera); 

	pcliente listaPessoas = NULL;
	Cliente pessoa;
	
	if(access(FIFO_SERV,F_OK)!=0){
		mkfifo(FIFO_SERV,0600);
	}
	else{
		printf("\nServidor ja se encontra aberto!!!!\n");
		exit(5);
	}

	int fd;
	int num;
	int res;
	int fdescrita;
	char fifo[40],comando[40];
	fd_set fds;

	fd = open(FIFO_SERV,O_RDWR); // em RDWR para ele nao bloquear quando nao tem jogadores a espera;
	if(fd ==-1){
			perror("\nErro a abrir o pipe do servidor(RDWR)");
			exit(EXIT_FAILURE);
	}

	printf("Fifo foi aberto!\n\n");
	printf("Consola do arbitro\n");
	setbuf(stdout,NULL);  

	do{
		
		// (1X) ABRE O PIPE, RECEBE A PESSOA, LE A PESSOA, ADICIONA A LISTA,ESCREVE PARA O PIPE DO CLIENTE O JOGO, FECHA O PIPE DO E DO SERVIDOR CLIENTE
		// WHILE(1) ABRE O PIPE, RECEBE O COMANDO, LE O COMANDO, ESCREVE PARA O PIPE DO CLIENTE A RESPOTA
		FD_ZERO(&fds);
		FD_SET(0,&fds);
		FD_SET(fd,&fds);

		res = select(fd + 1,&fds,NULL,NULL,NULL);  // BLOQUEIA AQUI ATE RECEBER
		if(res > 0 && FD_ISSET(0,&fds)){ //  ---- veio do stdin,teclado;
				//fgets(comando,49,stdin);
				scanf("%s",comando);
				fflush(stdout);

				if(strcmp(comando,"players") == 0){
					mostraJogadores(listaPessoas);
				}
				if(strcmp(comando,"games") == 0){
					mostraJogos();
				}
				if(comando[0] == 'k'){
					for(i=0;i<strlen(comando);i++){
						comando[i] = comando[i+1];
					}
					listaPessoas = kickJogador(comando,listaPessoas,&numJogadores);
				}
				if(strcmp(comando,"exit") == 0){
					//fechar pipes, limpar memoria
					unlink(FIFO_SERV);
					exit(EXIT_SUCCESS);
				}
		}
		
	else if(res > 0 && FD_ISSET(fd,&fds)){ // fd corresponde ao pipe
		// PARTE DE LER DO CLIENTE abrir o fifo do cliente para ler o que ele enviou
		num = read(fd,&pessoa,sizeof(Cliente)); // ler do pipe do servidor o que o cliente escreveu para la;
		if(num == -1){
			printf("\nErro ao ler dados do cliente");
			exit(EXIT_FAILURE);
		}
		// TA COM BUG, SEMPRE QUE DA READ DE ALGUMA CENA ACRESCENTA NOVO USER.....
		listaPessoas = adicionaLista(listaPessoas,pessoa,&numJogadores);

		fflush(stdout);
		pessoa.jogo = 5;
		printf("Jogador %s a comunicar com o arbitro!\n",pessoa.nome);
		// PARTE DE LOGICA

		if(strcmp(pessoa.comando,"#mygame") == 0){
			pessoa.curioso = descobreJogo(listaPessoas,pessoa.nome);
		}
		if(strcmp(pessoa.comando,"#quit") == 0){
			printf("Jogador %s saiu do jogo!",pessoa.nome);
		}

		// PARTE DE ENVIAR PARA O CLIENTE abrir o fifo do cliente para escrever a resposta para la para dentro.
		sprintf(fifo,FIFO_CLI,pessoa.pid);
		fdescrita = open(fifo,O_WRONLY); 
		if(fdescrita == -1){
			perror("\nErro ao abrir o FIFO do client.(Para escrita)");
			exit(EXIT_FAILURE);
		}
		
		num = write(fdescrita,&pessoa,sizeof(Cliente));
		if(num == -1){
			printf("\nErro ao enviar dados para o cliente\n");
			exit(EXIT_FAILURE);
		}
		close(fdescrita);
	}
	
	}while(strcmp(comando,"exit") != 0);

	close(fd);

	unlink(FIFO_SERV);
	
	return 0;
}

	

	//############################################################
	//			completar a funçao dos jogos random
	//############################################################

	// menu vai ter o kick(ta feito),lista(ta feito);

	
	

	// MENU PARA TESTE
/*	do{
		printf("\n1-jogo acerta numero!");
		printf("\n5-sair");
		printf("\n>");
		scanf("%d",&t);
		switch(t){
			case 1:
			funcaoFork("jogo");
			break;

			case 2:
					printf("\njogo2\n");

					break;
			case 3:
					printf("\njogo3\n");
					break;
			case 5:
				// ir a lista,limpar a memoria bla bla;
				exit(EXIT_SUCCESS);
		}
	}while(i<=duracaoCampeonato || t!=5 ); */

	



