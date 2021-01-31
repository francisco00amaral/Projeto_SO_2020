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
#include "cliente.h"
#include <sys/select.h>

#define FIFO_SERV "fifo_serv"
#define FIFO_CLI "cli%d"
#define FIFO_ARB "a%d"

typedef struct lista plista;
typedef struct Request pedido;


