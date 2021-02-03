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
#define MAXP 5
pcliente listaPessoas = NULL;
int duracao = 0; // serve como booleana, a 0 se ainda nao acabou a duracao do campeonato, a 1 se ja
int espera = 0;	 // serve como booleana, a 0 se ainda nao acabou a espera de jogadores, a 1 se ja
int numJogadores = 0;
char fifoThread[40];

int num;
int verificado = 0;
int inscrito = 0;
int res;
char fifo[40], comando[40];
int mp = 0;
int duracaoCampeonato, tempoEspera;
int fd;		   // pipe do servidor
int fdescrita; // pipe do cliente
pthread_t threadEspera;
pthread_t threadDuracao;

void verificaAmbiente()
{
}

//mandar os jogos para os clientes
//provavelmente meter numa thread
void funcaoFork(char *nome)
{
	int res;
	int pid;
	//pid=getpid();
	//printf("\npid:%d",pid);
	pid = getpid();
	int canal[2];
	int continua = 1;
	int n = 0;
	int estado;

	while (continua)
	{
		printf("[%5d]comando:", pid);
		fflush(stdout);
		pipe(canal);
		res = fork();

		if (res == 0)
		{
			pid = getpid();
			printf("sou o filho [%5d]!!", pid);
			//enganar o jogo ,redirecionamento;
			/*close(canal[0]);
			close(1);
			dup(canal[1]);
			close(canal[1]);*/
			fflush(stdout);

			execl(nome, nome, NULL);
			printf("erroa executar jogo[%5d]", pid);
			exit(7);
		}

		printf("\n\n[%5d]pai, criei of filho pid=%d!!", pid, res);
		continua = 0;
		//receber informaçao do jogo stdout
		close(canal[1]);
		while (n = read(canal[0], nome, 200) > 0)
		{
			nome[n] = '\0';
			printf("\narbitro recebi %s\n", nome);
		}
		close(canal[0]);
		wait(&estado);
		if (WIFEXITED(estado))
		{
			printf("\nArbitro jogador terminou com %d\n", WEXITSTATUS(estado));
		}
	}
}

void trataSig(int i)
{
	fprintf(stderr, "\nServidor do arbitro a terminar\n");
	// close(fd); ?
	// unlink(fd); ?
	exit(EXIT_SUCCESS);
}

////////////////////////-threads-/////////////////////////////////////
//fazer duas threds(uma para cada variavel de ambiente)
void *CampeonatoTime(void *dados)
{
	// for(int i = 0;i<10;i++){
	// 	printf("blablalblal");
	// 	fflush(stdout);
	// 	sleep(1);
	// }
	sleep(duracaoCampeonato);
	printf("Acabou o tempo de duracao do campeonato!\n");
	duracao = 1;
	pthread_exit(NULL); /* termina thread */
}

void *esperaJogadores(void *dados)
{
	sleep(tempoEspera);
	printf("Acabou o tempo de espera de jogadores.\n\n");
	espera = 1;
	// para nao ficar preso no read...
	Jogador b;
	b.pid = 0;
	b.ativo = -1;
	strcpy(b.nome, "valor");
	write(fd, &b, sizeof(Jogador));
	pthread_exit(NULL);
}

// esta thread vai receber uma estrutura com clientes?
void *trataCliente(void *dados)
{
	Jogador *pdata;

	pdata = (Jogador *)dados;
	Jogador pedido;
	strcpy(pedido.nome,pdata->nome);
	pedido.pid = pdata->pid;
	strcpy(pedido.comando,pdata->comando);
	char fifoCliente[40];
	sprintf(fifoCliente, FIFO_CLI, pdata->pid);

	int canal_Escrita_Jogo[2];
	int canal_Leitura_Jogo[2];
	// thread do cliente, vai criar um named pipe, apartir daqui o cliente so comunica com este fifo...
	printf("Thread para comunicar com o jogador %s, que tem o PID de: %d, fifo %s criado.\n",pedido.nome,pedido.pid,fifoCliente);
	// char *jogo1 = "jogo";
	// char *jogo2 = "nomeFIcheiro"
	// random();
	// swtich(
	// 	case 1
	// )

	do{
		// int res;
		// pipe(canal_Escrita_Jogo);
		// pipe(canal_Leitura_Jogo);
		// res = fork();
		// if (res == 0)
		// {
		// 	// pipe que escreve, fecho o stdout
		// 	// pipe que le, fecho o stdin para nao receber os comandos normais do arbitro
		// 	//enganar o jogo ,redirecionamento;
		// 	/*close(canal[0]);
		// 	close(1);
		// 	dup(canal[1]);
		// 	close(canal[1]);*/
		// 	fflush(stdout);

		// 	// execl(nome, nome, NULL);
		// 	// printf("erroa executar jogo[%5d]");
		// 	// exit(7);
		// }

	// printf("Estou a espera de receber algo do fifo %s do cliente %s para escrita \n",fifoCliente, pedido.nome);

	sprintf(fifoCliente, FIFO_CLI, pedido.pid);
	// printf("\n ANTESSS: Vou abrir o fifo %s do cliente %s para escrita \n",fifoCliente, pedido.nome);

	fdescrita = open(fifoCliente, O_WRONLY);
	if (fdescrita == -1)
	{
		perror("\nErro ao abrir o FIFO do client.(Para escrita)");
		exit(EXIT_FAILURE);
	}
	// printf("%s ABCASDASDSASASD ",pedido.comando);
	num = write(fdescrita, &pedido, sizeof(Jogador));
	if (num == -1)
	{
		printf("\nErro ao enviar dados para o cliente\n");
		exit(EXIT_FAILURE);
	}
	// printf("\n DEPOISSS Vou abrir o fifo %s do cliente %s para escrita \n",fifoCliente, pedido.nome);

	num = read(fd, &pedido, sizeof(Jogador)); // ler do pipe do servidor o que o cliente escreveu para la;
	// printf("Recebi o %s com o pid de:%d\n e com o comando %s ", pedido.nome,pedido.pid,pedido.comando);
	fflush(stdout);

	//  o problema esta no read

	if(strcmp(pedido.comando,"#mygame") == 0){
		printf("O %s esqueceu-se do jogo...Ira ser enviada uma mensagem para o relembrar\n",pedido.nome);
		pedido.curioso = 500;
	}

	// printf("\n\tEnviei o %d inscrito\n",inscrito);
	// printf("Enviei ao %s o jogo...\n",pedido.nome);

	close(fdescrita);
	}while(strcmp(pedido.comando,"#quit") != 0);

	// mkfifo(fifoThread, 0600);
	printf("Fifo para comunicar com o cliente criado\n");

	pthread_exit(NULL);
}
///////////////////////////////////////////////////////////////////7
// devolve 0 se ja existir uma pessoa com este nome, 1 senao
int verificaNomeEstatico(Jogador *jogadores, char *nome)
{
	for (int i = 0; i < mp; i++)
	{
		if ((strcmp(jogadores[i].nome, nome) == 0) && jogadores[i].ativo == 1)
		{
			return 0;
		}
	}
	return 1;
}

void adicionaCliente(Jogador *jogadores, Jogador user)
{
	int i = 0;
	// if(numJogadores == mp){
	// 	printf("Tentou entrar um cliente porem ja estavamos cheios.");
	// 	kill(user.pid,SIGUSR1);
	// 	return;
	// }
	for (i = 0; i < mp; i++)
	{
		if (jogadores[i].ativo == -1)
			break;
	}
	if (i == mp)
	{
		printf("Tentou entrar um cliente porem ja estavamos cheios.\n");
		kill(user.pid, SIGUSR1);
		return;
	}
	strcpy((jogadores + i)->nome, user.nome);
	(jogadores + i)->pid = user.pid;
	(jogadores + i)->ativo = 1;

	printf("Novo jogador: %s\n", user.nome);
	numJogadores++;
}


// falta isto
// falta isto
// falta isto
// falta isto

void mostraJogos()
{
	struct dirent *de; // Pointer for directory entry

	DIR *dr = opendir("/home/francisco/Desktop/SO_TP/SO_Projeto2020/jogos"); // compor depois,isto so funciona no meu pc por causa diretoria

	if (dr == NULL) // opendir returns NULL if couldn't open directory
	{
		printf("Could not open current directory");
		return;
	}
	while ((de = readdir(dr)) != NULL)
		printf("%s\n", de->d_name);

	closedir(dr);
}

int descobreJogo(pcliente lista, char *nome)
{
	pcliente aux;
	aux = lista;

	if (aux == NULL)
	{
		printf("Não ha jogadores ativos!\n");
		return 0;
	}
	while (aux != NULL)
	{
		if (strcmp(aux->nome, nome) == 0)
			return aux->jogo;
		aux = aux->prox;
	}
	return 0;
}

void avisaInicio(Jogador *jogadores)
{
	int i;
	for (i = 0; i < mp; i++)
	{
		if (jogadores[i].ativo == 1)
		{
			sprintf(fifo, FIFO_CLI, jogadores[i].pid);
			fdescrita = open(fifo, O_WRONLY);

			if (fdescrita == -1)
			{
				perror("\nErro ao abrir o FIFO do client.(Para escrita)");
				exit(EXIT_FAILURE);
			}

			inscrito = 1;
			num = write(fdescrita, &inscrito, sizeof(inscrito));
			// printf("\n\tEnviei o %d inscrito\n",inscrito);
			if (num == -1)
			{
				printf("\nErro ao enviar dados para o cliente\n");
				exit(EXIT_FAILURE);
			}

			inscrito = 0;
			close(fdescrita);
		}
	}
}

void avisaClientesE(Jogador* jogadores){
	int i;
	for (i = 0; i < mp; i++)
	{
		if(jogadores[i].ativo == 1)
			kill(jogadores[i].pid,SIGUSR1);
	}
}

void mostraJogadoresE(Jogador *jogadores)
{
	int i;
	for (i = 0; i < mp; i++)
	{
		if (jogadores[i].ativo == 1)
			printf("Jogador [%s] com o pid de [%d]\n", jogadores[i].nome, jogadores[i].pid);
	}
}


void kickJogadorE(Jogador *jogadores, char *nome)
{
	int i;
	for (i = 0; i < mp; i++)
	{
		if (jogadores[i].ativo == 1 && strcmp(jogadores[i].nome, nome) == 0){
			jogadores[i].ativo = -1;
			printf("Jogador %s kickado com sucesso!", nome);
			kill(jogadores[i].pid, SIGUSR1);	
			numJogadores--;
			return;
		}	
	}
	printf("Jogador %s nao existe", nome);
}


void trataTeclado(char *comando, Jogador *jogadores)
{
	int i;
	if (strcmp(comando, "players") == 0)
	{
		mostraJogadoresE(jogadores);
	}
	if (strcmp(comando, "games") == 0)
	{
		mostraJogos();
	}
	if (comando[0] == 'k')
	{
		for (i = 0; i < strlen(comando); i++)
		{
			comando[i] = comando[i + 1];
		}
		kickJogadorE(jogadores, comando);
	}
	if (strcmp(comando, "exit") == 0)
	{
		//fechar pipes, limpar memoria e manda sinal aos clientes que o arbitro acabou
		avisaClientesE(jogadores);
		unlink(FIFO_SERV);
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char **argv)
{
	char comeca[40];
	time_t c;
	srand((unsigned)time(&c));
	int i = 0;
	int t;

	if (argc != 3 ||
		sscanf(argv[1], "%d", &duracaoCampeonato) != 1 ||
		sscanf(argv[2], "%d", &tempoEspera) != 1)
	{ //pq a chamada do programa ja é um argumento
		printf("sintaxe: %s duracao_do_campeonato tempo_de_espera\n\n", argv[0]);
		return (EXIT_FAILURE);
	}

	// METER ISTO NUMA FUNCAO
	char *maxplayer;
	maxplayer = getenv("MAXPLAYER");
	if (maxplayer != NULL)
	{
		mp = atoi(maxplayer);
		if (mp > 30)
			mp = 30;
		else if (mp == 0)
			return (EXIT_FAILURE);
	}
	else
	{
		mp = MAXP; //como nao conseguio obter a variavel de ambiente toma o valor de maxp por default
		printf("\nMaximo de jogadores por omissao=%d", mp);
	}

	char *gamedir;
	gamedir = getenv("GAMEDIR");
	if (gamedir == NULL)
	{
		char cwd[100];
		gamedir = getcwd(cwd, sizeof(cwd));
	}
	printf("\ndiretoria:%s", gamedir);

	printf("\n\nNumero maximo de jogadores e de : %d\n", mp);
	printf("duracao do campeonato: %d\ntempo de espera por jogadores: %d\n\n", duracaoCampeonato, tempoEspera);

	Cliente pessoa;

	// thread para o tempo de espera que ele tem pelos jogadores
	if (pthread_create(&threadEspera, NULL, esperaJogadores, (void *)NULL) != 0)
	{
		perror("Erro a criar thread\n");
	}

	if (access(FIFO_SERV, F_OK) != 0)
	{
		mkfifo(FIFO_SERV, 0600);
	}
	else
	{
		printf("\nServidor ja se encontra aberto!!!!\n");
		exit(5);
	}

	Jogador jogadores[mp];
	Jogador user;
	memset(jogadores, 0, sizeof(jogadores));

	for (int k = 0; k < mp; k++)
	{
		jogadores[k].ativo = -1;
	}

	fd_set fds;

	fd = open(FIFO_SERV, O_RDWR); // em RDWR para ele nao bloquear quando nao tem jogadores a espera;
	if (fd == -1)
	{
		perror("\nErro a abrir o pipe do servidor(RDWR)");
		exit(EXIT_FAILURE);
	}

	printf("Servidor foi aberto em modo nao bloqueante!\n\n");
	printf("Consola do arbitro\n\n");
	setbuf(stdout, NULL);

	// aceita jogadores enquanto o tempo de espera ainda nao acabar(espera == 0) ou nao houverem pelo menos dois jogadores.
	while (espera == 0 || numJogadores < 1)
	{
		num = read(fd, &user, sizeof(Jogador)); // ler do pipe do servidor o que o cliente escreveu para la;
		if (user.pid == 0)
			break;
		if (num == -1)
		{
			printf("\nErro ao ler dados do cliente");
			exit(EXIT_FAILURE);
		}

		// verifica se o nome ja existe, devolve 0 se ja existir.;
		if (verificaNomeEstatico(jogadores, user.nome) == 0)
		{
			user.jaExiste = 0;
			// kill(pessoa.pid,SIGUSR1);
			verificado = 1;
		}
		else
		{
			//se ainda nao existir, coloco a variavel a 1. e adiciono este cliente a lista ligada.
			user.jaExiste = 1;

			adicionaCliente(jogadores, user);
			// printf("Numero: %d",numJogadores);
			// for(int i=0;i<mp;i++)
			// {
			// 	printf("TESTE : NOME %s E ATIVO %d\n", jogadores[i].nome,jogadores[i].ativo);
			// 	}
		}

		sprintf(fifo, FIFO_CLI, user.pid);
		fdescrita = open(fifo, O_WRONLY);

		if (fdescrita == -1)
		{
			perror("\nErro ao abrir o FIFO do client.(Para escrita)");
			exit(EXIT_FAILURE);
		}

		num = write(fdescrita, &user, sizeof(Jogador));
		if (num == -1)
		{
			printf("\nErro ao enviar dados para o cliente\n");
			exit(EXIT_FAILURE);
		}

		close(fdescrita);
	}

	// enviar mensagem a todos os clientes que o torneio vai comecar...
	// sprintf(fifo,FIFO_CLI,pessoa.pid);
	avisaInicio(jogadores);

	// aqui seria comecar a fazer as threads para os jogos...
	// toda a comunicacao cliente/servidor passa a ser feitos nesta thread.
	pthread_t threadTarefa[numJogadores];

	for (int j = 0; j < numJogadores; j++)
	{
		if(jogadores[j].ativo == 1){
			if (pthread_create(&threadTarefa[j], NULL, trataCliente, (void *)&jogadores[j]) != 0)
			{
				perror("Erro a criar thread\n");
				close(fd);
				avisaClientesE(jogadores);
				exit(EXIT_FAILURE);
			}
		}
	}

	// Comecar logo a contar o tempo mal o campeonato comece.
	if (pthread_create(&threadDuracao, NULL, CampeonatoTime, (void *)NULL) != 0)
	{
		perror("Erro a criar thread\n");
		close(fd);
		avisaClientesE(jogadores);
		exit(EXIT_FAILURE);
	}
	printf("------Consola do arbitro------\n\n");
	printf("Comando disponiveis: \nk[nomeJogador]\texit\tplayers\ts[nomeJogador\tr[nomeJogador]\tend \n");

	do{
		printf("Comando: ");
		scanf("%s", comando);
		
		fflush(stdout);
		trataTeclado(comando, jogadores);

	}while(strcmp(comando,"exit") != 0);

	printf("Campeonato a terminar!\n");

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
