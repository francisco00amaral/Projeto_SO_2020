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
#include <pthread.h> 
#include "utils.h"

//##########################################################################################################
//
//		Falta o pipe para enviar o filho ou seja os jogos para o cliente e receber os resultados
//
//
//##########################################################################################################
//#include "arbitro.h"
#define MAXP 2
pcliente listaPessoas = NULL;
int duracao = 0; // serve como booleana, a 0 se ainda nao acabou a duracao do campeonato, a 1 se ja
int espera = 0; // serve como booleana, a 0 se ainda nao acabou a espera de jogadores, a 1 se ja
int numJogadores = 0;
char fifoThread[40];
int mp=0;
int duracaoCampeonato,tempoEspera;
int fd; // pipe do servidor
int fdescrita; // pipe do cliente
pthread_t threadEspera;
pthread_t threadDuracao;






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
void *CampeonatoTime(void *dados){
	// for(int i = 0;i<10;i++){
	// 	printf("blablalblal");
	// 	fflush(stdout);
	// 	sleep(1);
	// }
	sleep(duracaoCampeonato);
	printf("THREAD ACORDOU");
	duracao = 1;
	pthread_exit(NULL);  /* termina thread */
}


void *esperaJogadores(void *dados){
	sleep(tempoEspera);
	printf("THREAD DE ESPERAR JOGADORES ACORDOU"); 
	espera = 1;
	pthread_exit(NULL);
}

// esta thread vai receber uma estrutura com clientes?
void *trataCliente(void* dados){
	Cliente *pdata;

	pdata = (Cliente *) dados;

	// thread do cliente, vai criar um named pipe, apartir daqui o cliente so comunica com este fifo...
	//cli.pid=getpid();
	sprintf(fifoThread,FIFO_ARB,pdata->pid);

	mkfifo(fifoThread,0600);
	printf("Fifo para comunicar com o cliente criado\n");

	pthread_exit(NULL);
}
///////////////////////////////////////////////////////////////////7

// devolve 0 se ja existir uma pessoa com este nome, 1 senao
int verificaNome(pcliente lista,Cliente pessoa){
	pcliente aux;
	aux = lista;
		if(aux == NULL){
			return 1;
	}
	while (aux != NULL)
	{
		// if(strcmp(aux->nome,pessoa.nome) == 0 && aux->pid == pessoa.pid){
		// 	return 1;
		// }
	 	if(strcmp(aux->nome,pessoa.nome) == 0 && aux->pid != pessoa.pid){
			printf("Já existe um jogador com esse nome, não irá jogar!");
			return 0;
		}
		aux = aux->prox;
	}
	return 1;
}

pcliente adicionaLista(pcliente lista,Cliente pessoa,int *npessoas){
		pcliente aux;
		pcliente novo;

		/*int a = verificaNome(lista,pessoa);

		if(a == 0){
		kill(pessoa.pid,SIGINT);
		return lista;
		} */

        
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
        aux->prox = novo;
    }

        //aux->prox = novo;
		if(*npessoas >= mp){
			printf("Numero de jogadores chegou ao limite, %d",mp);
			kill(pessoa.pid,SIGUSR1);
			return lista;
		}

		

		novo = malloc(sizeof(pessoa));
		if(novo == NULL){
			fprintf(stderr, "Nao foi possivel alocar memoria\n");
        	abort();
		}

		novo->ativo = 1;
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

	printf("Novo jogador: %s\n",pessoa.nome);
	(*npessoas)++;

	
	return lista;
}

int verificaExistencia(pcliente lista,Cliente pessoa){
	pcliente aux;
	aux = lista;
		if(aux == NULL){
			return 1;
	}
	while (aux != NULL)
	{
		if(aux->pid == pessoa.pid){
			return 1;
		}
		aux = aux->prox;
	}
	return 0;
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

// void avisaInicio(pcliente lista){
// 		pcliente aux;
// 		aux = lista;

// 		if(aux == NULL){
// 			printf("Nao ha jogadores ativos!\n");
// 			return;
// 		}
// 		while (aux != NULL)
// 		{
// 			kill(aux->pid,SIGUSR2);
// 		aux = aux->prox;
// 		}

// }

void avisaClientes(pcliente lista){
		pcliente aux;
		aux = lista;

		if(aux == NULL){
			printf("Decidi sair sem haver jogadores ativos!\n");
			return;
		}
		while (aux != NULL)
		{
			kill(aux->pid,SIGUSR1);
		aux = aux->prox;
		}

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
	printf("Jogador %s kickado com sucesso!",nome);
	kill(save,SIGUSR1);
    
	(*numJogadores)--;
   return lista;
} 

void libertaLista(pcliente p){

        pcliente aux;

        while(p != NULL){
            aux = p;
            p = p->prox;
            free(aux);
        }
}

void trataTeclado(char* comando){
	int i;
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
		//fechar pipes, limpar memoria e manda sinal aos clientes que o arbitro acabou
		avisaClientes(listaPessoas);
		libertaLista(listaPessoas);
		unlink(FIFO_SERV);
		exit(EXIT_SUCCESS);
		}
}



int main(int argc,char **argv){
	char comeca[40];
	time_t c;
	srand((unsigned) time(&c));
	int i=0;
	int t;
	

	if(argc!=3||
		sscanf(argv[1],"%d",&duracaoCampeonato)!=1||
		sscanf(argv[2],"%d",&tempoEspera)!=1){//pq a chamada do programa ja é um argumento
		printf("sintaxe: %s duracao_do_campeonato tempo_de_espera\n\n",argv[0]);
		return (EXIT_FAILURE);
	}

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

	Cliente pessoa;

	
	// thread para o tempo de espera que ele tem pelos jogadores
	if(pthread_create(&threadEspera,NULL,esperaJogadores,(void *)NULL) != 0){
		perror("Erro a criar thread\n");
	}

	if(access(FIFO_SERV,F_OK)!=0){
		mkfifo(FIFO_SERV,0600);
	}
	else{
		printf("\nServidor ja se encontra aberto!!!!\n");
		exit(5);
	}


	int num;
	int verificado = 0;
	int res;
	char fifo[40],comando[40];
	fd_set fds;


	fd = open(FIFO_SERV,O_RDWR); // em RDWR para ele nao bloquear quando nao tem jogadores a espera;
	if(fd ==-1){
			perror("\nErro a abrir o pipe do servidor(RDWR)");
			exit(EXIT_FAILURE);
	}

	printf("Servidor foi aberto em modo nao bloqueante!\n\n");
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

				trataTeclado(comando);
		}
	
	 else if(res > 0 && FD_ISSET(fd,&fds)){ // fd corresponde ao pipe
		// PARTE DE LER DO CLIENTE abrir o fifo do cliente para ler o que ele enviou
		// if(espera == 1){
		// 	printf("Campeonato terminou!");
		// 	strcpy(comando,"exit");
		// 	printf("abc");
		// }
		// pthread_join(threadDuracao,NULL);

		num = read(fd,&pessoa,sizeof(Cliente)); // ler do pipe do servidor o que o cliente escreveu para la;
		if(num == -1){
			printf("\nErro ao ler dados do cliente");
			exit(EXIT_FAILURE);
		}
		// verifica se ja existe uma pessoa com este nome. talvez abre o pipe do cliente e escreve la?
		if(verificaNome(listaPessoas,pessoa) == 0){
			pessoa.jaExiste = 1;
        	kill(pessoa.pid,SIGUSR1);
			verificado = 1;
		}

		if(verificaExistencia(listaPessoas,pessoa) == 0 && espera == 1){ // se jogador nao existe e se ja acabou o tempo de espera
			kill(pessoa.pid,SIGUSR1);
		}
		// printf("Jogador %s a comunicar com o arbitro!\n",pessoa.nome);
		
		if(espera == 0 && verificado == 0){
				listaPessoas = adicionaLista(listaPessoas,pessoa,&numJogadores);
				verificado = 0;
		}


		fflush(stdout);
		pessoa.jogo = 5;
		if(espera == 1){ // espera ja acabou, vai comecar o campeonato, avisar clientes e comecar threads!
        //    // thread para a duracao do campeonato,comeca a contar o tempo de campeonato. aqui ta mal posta, n devia ser aqui
    	//    if(pthread_create(&threadDuracao, NULL, CampeonatoTime,(void *) NULL) != 0){
		//    		perror("Erro a criar thread\n");
		// 		close(fd);
		// 		avisaClientes(listaPessoas);
		// 		exit(EXIT_FAILURE);
		//    }
		pessoa.emJogo = 1;

		// int total = numJogadores;  
		// pthread_t threadTarefa[total];

		//  for(int j=0;i<numJogadores;i++){
		// 	pthread_create(threadTarefa[j],NULL,trataCliente,NULL);
		// 	}
		}

		// PARTE DE LOGICA

		if(strcmp(pessoa.comando,"#mygame") == 0){
			pessoa.curioso = descobreJogo(listaPessoas,pessoa.nome);
		}
		if(strcmp(pessoa.comando,"#quit") == 0){
			printf("Jogador %s saiu do jogo!",pessoa.nome);
			listaPessoas = kickJogador(pessoa.nome,listaPessoas,&numJogadores);
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

	printf("Campeonato a terminar!");
	libertaLista(listaPessoas);

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

	



