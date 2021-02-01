#include "utils.h"
	char fifo[40];

void trataSig(int i){
	printf("Deixei de comunicar com o arbitro por motivos superiores.\n");
	printf("Ou inscricoes ja terminaram, ou o arbitro nao gosta de mim ou saiu, ou decidi sair.\n");

	// close(fd); 
	// unlink(fd); 
	sleep(1);
	unlink(fifo);
	exit(EXIT_SUCCESS);
}


int main(int argc,char **argv){
	Cliente cli;
	int fd,num;
	int i = 0;
	int fdleitura;

	if(access(FIFO_SERV,F_OK)!=0){
		fprintf(stderr,"\nServidor nao se encontra aberto!!!!\n");
		exit(5);
	}
	signal(SIGUSR1,trataSig);

	cli.pid=getpid();
	sprintf(fifo,FIFO_CLI,cli.pid);

	mkfifo(fifo,0600);
	printf("Fifo do cliente criado\n");

	printf("Introduza o seu nome: ");
		
	scanf("%s",cli.nome);
	fflush(stdout);

	fd=open(FIFO_SERV,O_WRONLY); // abrir pipe do servidor para escrita, mandar informaçoes para ele
	if(fd == -1){
		printf("\nErro a abrir o pipe do servidor");
		exit(EXIT_FAILURE);
	}
	printf("Estou em conexacao com o arbitro...\n");

	cli.pid = getpid();
	cli.ativo = -1;
	cli.jogo = 3; // mudar isto depois;
	cli.prox = NULL;

	do{
		printf("Comando: ");
		fflush(stdout);
		scanf("%s",cli.comando);

		num = write(fd,&cli,sizeof(Cliente)); // escrever no pipe do servidor
		if(num == -1){
			printf("\nErro ao enviar dados para o cliente");
			exit(EXIT_FAILURE);
		}

		// CRIAR E ABRIR FIFO DO CLIENTE PARA LER O QUE O SERVIDOR ENVIOU.
		fdleitura = open(fifo,O_RDONLY);
		num = read(fdleitura,&cli,sizeof(Cliente));
		close(fdleitura);
		if(strcmp(cli.comando,"#mygame") == 0){
			printf("O meu jogo e o %d\n",cli.curioso);
		}
		if(cli.jaExiste)
			printf("Ja existe um jogador com esse nome! Nao ira jogar!\n");
			
		// if(cli.emJogo == 1 && i == 0){
		// 	printf("O campeonato vai comecar, prepare-se!\n");
		// 	i = 1;
		// }

	}while(strcmp(cli.comando,"#quit") != 0);
	


// fazer o write do nome e receber o jogo, e depois ter um do while que fica a correr enquanto nao receber um exit ou pedir exit ao arbitro
	

	close(fd);

	unlink(fifo);
	
	exit(5);	
}

/*
res = select(fd + 1,&fds,NULL,NULL,NULL);  // BLOQUEIA AQUI ATE RECEBER
	if(res > 0 && FD_ISSET(0,&fds)){  // veio do teclado;
		printf("Comandos: ");
		scanf("%s",cli.comando);
		fflush( stdout);

		num = write(fd,&cli,sizeof(Cliente));
		if(num == -1){
			printf("\nErro a escrever para o servidor");
			exit(EXIT_FAILURE);
		}

		close(fd);
}//  ---- veio do stdin,teclado;
	else if(res > 0 && FD_ISSET(fd,&fds)){
		do{
		num = read(fdleitura,&cli,sizeof(Cliente));
		if(num == -1){
			printf("\nErro a fazer a leitura");
			exit(EXIT_FAILURE);
		}
		}while(cli.quit != 1);
	} // veio do teclado; */