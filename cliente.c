#include "utils.h"
char fifo[40];
Cliente cli;
Jogador user;
int comeca = 0;

void trataSig(int i)
{
	printf("Deixei de comunicar com o arbitro por motivos superiores.\n");
	printf("Ou inscricoes ja terminaram, ou o arbitro nao gosta de mim ou saiu, ou decidi sair.\n");

	// close(fd);
	// unlink(fd);
	sleep(1);
	unlink(fifo);
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	setbuf(stdout,NULL);
	int fd, num;
	int inscrito = 0;
	int i = 0;
	int fdleitura;
	int fdAux;
	char fifoAux[40];

	if (access(FIFO_SERV, F_OK) != 0)
	{
		fprintf(stderr, "\nServidor nao se encontra aberto!!!!\n");
		exit(5);
	}
	signal(SIGUSR1, trataSig);

	user.pid = getpid();
	user.ativo = -1;
	user.jaExiste = 0;

	// cli.pid = getpid();
	// cli.ativo = -1;
	// cli.jogo = 3; // mudar isto depois;
	// cli.prox = NULL;
	// cli.jaExiste = 0;
	printf("Tenho o pid %d",user.pid);

	sprintf(fifo, FIFO_CLI, user.pid);

	mkfifo(fifo, 0600);
	printf("Fifo do cliente criado\n");

	printf("Introduza o seu nome: ");

	scanf("%s", user.nome);
	fflush(stdout);

	fd = open(FIFO_SERV, O_WRONLY); // abrir pipe do servidor para escrita, mandar informaçoes para ele
	if (fd == -1)
	{
		printf("\nErro a abrir o pipe do servidor");
		exit(EXIT_FAILURE);
	}

	/*Enviar pedido de confirmacao de login ao arbitro */
	num = write(fd, &user, sizeof(Jogador)); // escrever no pipe do servidor
	if (num == -1)
	{
		printf("\nErro ao enviar dados para o arbitro\n");
		exit(EXIT_FAILURE);
	}

	printf("Foi enviado um pedido de autenticacao ao arbitro...\n");

	/* Receber resposta do servidor (read)*/
	fdleitura = open(fifo, O_RDONLY);
	num = read(fdleitura, &user, sizeof(Jogador));


	if (num == -1)
	{
		printf("\nErro ao ler dados do o arbitro\n");
		exit(EXIT_FAILURE);
	}
	close(fdleitura);

	if (user.jaExiste == 0)
	{
		printf("\nLamentamos mas ja existe um jogador com esse nome,nao ira jogar!\n");
		close(fd);

		unlink(fifo);

		exit(5);
	}
	else
	{
		printf("\nAutenticado com sucesso!\n");
		printf("Aguarde que o campeonato comece!\n");
	}

	// receber mensagens que o torneio vai comecar.
	fdleitura = open(fifo, O_RDONLY);

	num = read(fdleitura, &inscrito, sizeof(inscrito));
	close(fdleitura);
	if (num == -1)
	{
		printf("\nErro ao ler dados do o arbitro\n");
		exit(EXIT_FAILURE);
	}
	printf("O campeonato esta prestes a comecar!\n");
	


	printf("Comando disponiveis: #mygame // #quit \n");

	char resposta[20];
	char buffer[4096];
	sprintf(fifo, FIFO_CLI, user.pid);
	sprintf(fifoAux,FIFO_ARB,getpid());
	do{
		fdleitura = open(fifo,O_RDONLY);
		printf("\n[SUCESSO]: %s aberto\n",fifo);
		memset(buffer,'\0',sizeof(char) * 4096);
		num = read(fdleitura,&buffer,sizeof(buffer));
		if (num == -1)
		{
		printf("\nErro ao ler dados do arbitro\n");
		exit(EXIT_FAILURE);
		}
		printf("\n\t[SUCESSO]: Leitura do %s bem efetuada \n",fifo);
		close(fdleitura);
		printf("\n\t[FECHADO]: Pipe do %s fechado\n",fifo);
		printf("%s",buffer);
		memset(resposta,'\0',sizeof(char) * 20);
		printf("Resposta: ");
		scanf("%s",resposta);
		fdAux = open(fifoAux,O_WRONLY);
		printf("\n[SUCESSO]: %s aberto\n",fifoAux);
		
		num = write(fdAux,&resposta,sizeof(resposta));
		if (num == -1)
		{
		printf("\nErro ao enviar dados para o arbitro\n");
		exit(EXIT_FAILURE);
		}
		close(fdAux);

		// if(strcmp(resposta,"#mygame") == 0){
		// char mygame[30];
		// fdleitura = open(fifo,O_RDONLY);
		// printf("\n[SUCESSO]: %s aberto\n",fifo);
		// num = read(fdleitura,&mygame,sizeof(mygame));
		// printf("\n\t[SUCESSO]: O meu jogo e: %s",mygame);
		// close(fdleitura);
		// }
		
		printf("\n\t[SUCESSO]: Escrita para o %s bem efetuada \n",fifoAux);
		
		printf("\n\t[FECHADO]: Pipe do %s fechado\n",fifoAux);
	}while(strcmp(resposta,"#quit") != 0);

	// fazer o write do nome e receber o jogo, e depois ter um do while que fica a correr enquanto nao receber um exit ou pedir exit ao arbitro

	close(fd);

	unlink(fifo);

	exit(5);
}