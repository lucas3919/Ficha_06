#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


int limite, buffer_size, *buffer = NULL, valor, x;
int posicao_produtor = 0, posicao_consumidor = 0;

sem_t mutex, espaco_vazio, delay;

pthread_mutex_t mutex_produtor = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_consumidor = PTHREAD_MUTEX_INITIALIZER;


void consome(int *id) {

	// Anda pelo buffer até achar um espaço vazio
	while (buffer[posicao_consumidor] == -1) {
		posicao_consumidor = (posicao_consumidor + 1) % buffer_size;
	}

	printf("Consumidor %d consumindo %d na posição %d\n", *id, buffer[posicao_consumidor], posicao_consumidor);

	// Esvazia a posição atual do Buffer
	buffer[posicao_consumidor] = -1;

	// Anda para a próxima casa do Buffer
	posicao_consumidor = (posicao_consumidor + 1) % buffer_size;

}


void *consumidor(void *args) {
	int *id = (int *)args;
	while (1) {
		sem_wait(&delay);
		sem_wait(&mutex);
		consome(id);
		sem_post(&espaco_vazio);
		sem_post(&mutex);
	}
}


int funcao_limite() {
	int resultado = 2*x + 1;
	if (x == limite)
		x = 0;
	else
		x++;
	return resultado;
}


void produz(int *id) {

	// Realiza a função do limite f(x) = 2x+1, entrega resultado
	int valor = funcao_limite();

	// Anda pelo buffer até achar um espaço vazio
	while (buffer[posicao_produtor] != -1) {
		posicao_produtor = (posicao_produtor + 1) % buffer_size;
	}

	printf("Produtor %d produzindo %d na posição %d\n", *id, valor, posicao_produtor);
	
	// Poem o valor na posição do produtor no Buffer
	buffer[posicao_produtor] = valor;

	// Anda para a próxima casa do Buffer
	posicao_produtor = (posicao_produtor + 1) % buffer_size;
}


void *produtor(void *args) {
	int *id = args; 
	while (1) {
		sem_wait(&espaco_vazio);
		sem_wait(&mutex);
		produz(id);
		sem_post(&mutex);
		sem_post(&delay);
	}
}


int main() {
	int produtor_qnt, consumidor_qnt, index;

	printf("Insira a quantidade de Produtores, Consumidores, o limite e o tamanho do Buffer respectivamente: ");
	scanf("%d %d %d %d", &produtor_qnt, &consumidor_qnt, &limite, &buffer_size);
	
	// Cria o Buffer
	buffer = (int *) malloc (sizeof(int) * buffer_size);
	memset(buffer, -1, buffer_size * sizeof(int));

	// Inicializa os Semaforos
	sem_init(&espaco_vazio, 0, buffer_size);
	sem_init(&mutex, 0, 1);
	sem_init(&delay, 0, 0);

	pthread_t produtores[produtor_qnt];
	pthread_t consumidores[consumidor_qnt];

	// Cria o array de produtores e consumidores
	int produtor_id[produtor_qnt];
	int consumidor_id[consumidor_qnt];

	// Cria a Thread dos Produtores
	for (index = 0; index < produtor_qnt; index++) {
		produtor_id[index] = index;
		pthread_create(&(produtores[index]), NULL, produtor, &produtor_id[index]);
	}

	// Cria a Thread dos consumidores
	for (index = 0; index < consumidor_qnt; index++) {
		consumidor_id[index] = index;
		pthread_create(&(consumidores[index]), NULL, consumidor, &consumidor_id[index]);
	}

	// Elimina a Thread dos Produtores
	for (index = 0; index < produtor_qnt; index++) {
		pthread_join(produtores[index], NULL);
	}

	// Elimina a Thread dos Consumidores
	for (index = 0; index < produtor_qnt; index++) {
		pthread_join(consumidores[index], NULL);
	}

	return 0;
}