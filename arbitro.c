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
char fifo[40], comando[40];
int mp = 0;
int duracaoCampeonato, tempoEspera;
int fd;		   // pipe do servidor
int fdescrita; // pipe do cliente
pthread_t threadEspera;
pthread_t threadDuracao;

/*mandar os jogos para os clientes
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
			close(canal[1]);
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
} */

void trataSig(int i)
{
	fprintf(stderr, "\nServidor do arbitro a terminar\n");
	// close(fd); ?
	// unlink(fd); ?
	exit(EXIT_SUCCESS);
}

////////////////////////-threads-/////////////////////////////////////
void *CampeonatoTime(void *dados)
{
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
	num = write(fd, &b, sizeof(Jogador));
	pthread_exit(NULL);
}

void *trataCliente(void *dados)
{
	Jogador *pdata = (Jogador *)dados;

	char fifoCliente[40];
	char fifoAux[40];
	int fdAux;
	int fdCliente;
	int le;
	sprintf(fifoAux,FIFO_ARB,pdata->pid); // pipe auxiliar.
	sprintf(fifoCliente, FIFO_CLI, pdata->pid);


	mkfifo(fifoAux, 0600); // criar o fifo do auxiliar
	//printf("Fifo do auxiliar criado\n");

	int canal_Jogo_Arbitro[2];
	int canal_Arbitro_Jogo[2];

	// thread do cliente, vai criar um named pipe, apartir daqui o cliente so comunica com este fifo...
	printf("Thread para comunicar com o jogador %s, que tem o PID de: %d, fifo %s.\n", pdata->nome, pdata->pid, fifoCliente);
	char *jogo1 = "g2";
	// char *jogo2 = "nomeFIcheiro"
	// random();
	// swtich(
	// 	case 1
	// )
	pipe(canal_Jogo_Arbitro);
	pipe(canal_Arbitro_Jogo);
	int p;
	p = fork();
	if (p == 0)
	{
		// pipe que escreve, fecho o stdout
		// printf("Sou o filho com o pid %d\n", getpid());
		//enganar o jogo ,redirecionamento;
		// sou o filho
		// fechar o stdin do jogo, pois ele nao quer escrever do arbitro
		close(canal_Arbitro_Jogo[1]);
		close(canal_Jogo_Arbitro[0]);
		// fflush(stdout);
		close(1); // fechar o stdout porque nao quero dar print aqui.

		// em vez de dar printn terminal,e u quero dar print p outro lado
		// dup mete o descritor na posicao disponivel, neste caso vai ser no stdout.
		// O arbitro vai receber este printf
		// redirecionar para o cliente.
		dup(canal_Jogo_Arbitro[1]);
		close(canal_Jogo_Arbitro[1]);

		// fechar o stdin, pois nao quero escrever para la, quem vai fazer isso e o cliente.
		close(0);

		dup(canal_Arbitro_Jogo[0]);
		close(canal_Arbitro_Jogo[0]);

		execl(jogo1, jogo1, NULL);
		printf("erroa executar jogo");
		exit(7);
	}

	
	close(canal_Jogo_Arbitro[1]);
	close(canal_Arbitro_Jogo[0]);
	char buffer[4096];
	char resposta[20];
	char enter = '\n';
	int res;
	int estado;
	do
	{
		printf("Duracao : %d",duracao);
		memset(buffer,'\0',sizeof(char) * 4096);
		res = read(canal_Jogo_Arbitro[0],&buffer,sizeof(buffer));
		if (res == -1)
		{
		printf("\nErro ao ler do pipe do jogo para o arbitro\n");
		exit(EXIT_FAILURE);
		}
		printf("\n\t[SUCESSO]: Leitura do jogo bem efetuada\n");

		// strcpy(buffer,"Ola jogador!");
		printf("Recebi : %s",buffer);
		fflush(stdout);
		sprintf(fifoCliente, FIFO_CLI, pdata->pid);

		fdescrita = open(fifoCliente,O_WRONLY);
		printf("\n\t[SUCESSO]: %s aberto\n",fifoCliente);
		
		res = write(fdescrita,&buffer,sizeof(buffer));
		close(fdescrita);
		if (res == -1)
		{
		printf("\nErro ao enviar dados para o cliente\n");
		exit(EXIT_FAILURE);
		}
		printf("\n\t[SUCESSO]: Escrita para o %s bem efetuada \n",fifoCliente);
		
		printf("\n\t[FECHADO]: Pipe do %s fechado\n",fifoCliente);

		printf("\n\t[A ABRIR]: Vou abrir o %s aberto\n",fifoAux);

		fdAux = open(fifoAux,O_RDWR);

		printf("\n\t[ABERTO]: Abrir o %s aberto\n",fifoAux);
		memset(resposta,'\0',sizeof(char) * 20);
		res = read(fdAux,&resposta,sizeof(resposta));
		close(fdAux);
		if (res == -1)
		{
		printf("\nErro ao ler dados do cliente\n");
		exit(EXIT_FAILURE);
		}
		printf("\n\t[SUCESSO]: Leitura do %s bem efetuada \n",fifoAux);
		
		printf("\n\t[FECHADO]: Pipe do %s fechado\n",fifoAux);
		printf("\n\t[RECEBIDO]: Recebi do cliente: %s\n",resposta);
		strncat(resposta,&enter,1);


		// if(strcmp(resposta,"#mygame") == 0){
		// 	// abro o pipe mando/lhe o jogo
		// 	char mygame[30];
		// 	strcpy(mygame,jogo1);
		// 	fdescrita = open(fifoCliente,O_WRONLY);
		// 	res = write(fdescrita,&mygame,sizeof(mygame));
		// 	close(fdescrita);
		// }
		
		printf("\n\t[ENVIADO]: Enviei ao jogo %s \n",resposta);
		res = write(canal_Arbitro_Jogo[1],&resposta,strlen(resposta));
		if (res == -1)
		{
		printf("\nErro ao enviar dados para o jogo\n");
		exit(EXIT_FAILURE);
		}
		printf("\n\t[SUCESSO]: Escrita para o jogo bem efetuada \n");
		
		
	} while (strcmp(resposta,"#quit") != 0 && duracao == 0);
	printf("Thread do %s e acabou o tempo.",pdata->nome);

	// enviar mensagem ao jogo para terminar
	kill(p,SIGUSR1);
	// vou ter de mandar um sinal ao jogador para sair do ciclo.
	// obter a pontuacao
	wait(&estado);
	if (WIFEXITED(estado)){
		pdata->pontuacao = WEXITSTATUS(estado);
	}
	printf("Jogador %s acabou com : %d",pdata->nome,pdata->pontuacao);
	unlink(fifoAux);

	if(strcmp(resposta,"#quit") == 0){
		pdata->ativo = -1;
		kill(pdata->pid, SIGUSR1);
		numJogadores--;
	}

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

	DIR *dr = opendir("./jogos/"); // compor depois,isto so funciona no meu pc por causa diretoria

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
			close(fdescrita);
			// printf("\n\tEnviei o %d inscrito\n",inscrito);
			if (num == -1)
			{
				printf("\nErro ao enviar dados para o cliente\n");
				exit(EXIT_FAILURE);
			}

			inscrito = 0;
			
		}
	}
}

void avisaClientesE(Jogador *jogadores)
{
	int i;
	for (i = 0; i < mp; i++)
	{
		if (jogadores[i].ativo == 1)
			kill(jogadores[i].pid, SIGUSR1);
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
		if (jogadores[i].ativo == 1 && strcmp(jogadores[i].nome, nome) == 0)
		{
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
	while (espera == 0 || numJogadores < 2)
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

	close(fd);
	// enviar mensagem a todos os clientes que o torneio vai comecar...
	// sprintf(fifo,FIFO_CLI,pessoa.pid);
	avisaInicio(jogadores);

	// aqui seria comecar a fazer as threads para os jogos...
	// toda a comunicacao cliente/servidor passa a ser feitos nesta thread.
	pthread_t threadTarefa[numJogadores];
	for (int j = 0; j < numJogadores; j++)
	{
		if (jogadores[j].ativo == 1)
		{
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

	do
	{
		printf("Comando: ");
		scanf("%s", comando);

		fflush(stdout);
		trataTeclado(comando, jogadores);

	} while (strcmp(comando, "exit") != 0);

	printf("Campeonato a terminar!\n");

	close(fd);

	unlink(FIFO_SERV);

	return 0;
}