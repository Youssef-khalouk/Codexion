
#include "codexion.h"

void* my_function(void* arg){
	int i = 10;
	printf("thread is runing here -> %s:\n", (char*)arg);
	while (i){
		printf("\tcount -> %d\n", i);
		i--;
	}

	return NULL;
}


int main(int argc, char **argv)
{
	data_t* data;
	if (argc > 8)
	{
		fprintf(stderr, "there is more arguments!");
		return (1);
	}
	if (argc < 8)
	{
		fprintf(stderr, "your arguments is not enugh!");
		return (1);
	}
	data = parse_args(argv);
	if (data->error)
		return (1);

	pthread_t thread1;
	pthread_t thread2;
	pthread_create(&thread1, NULL, my_function, "first");
	pthread_create(&thread2, NULL, my_function, "second");
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	printf("Back in main\n");
	return (0);
}
