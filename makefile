all: 
	gcc -c cliente.c
	gcc -c arbitro.c
	gcc -c jogo.c 
	gcc cliente.o -o cliente
	gcc jogo.o -o jogo
	gcc arbitro.o -o arbitro 
	
	

cliente: cliente.o
	gcc cliente.o -o cliente 

arbitro: arbitro.o
	gcc arbitro.o -o arbitro 

jogo: jogo.o
	gcc jogo.o -o jogo

cliente.o: cliente.c
	gcc cliente.c -c

arbitro.o: arbitro.c
	gcc arbitro.c -c 

jogo.o: jogo.c
	gcc jogo.c -c

clean:
	$(RM) cliente.o arbitro.o jogo.o
	$(RM) cliente arbitro jogo
	
