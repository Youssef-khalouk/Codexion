#include "codexion.h"

static void* coder_proccess(void* the_coder)
{
	coder_t* coder = (coder_t*)the_coder;
	printf("coder is runing -> %p\n", coder);
	coder->last_proccess_time = time(NULL);
	
	pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);



	return NULL;
}


void proccess(data_t* data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_t *coder = malloc(sizeof(pthread_t));
		data->coders[i].thread_id = coder;
		pthread_create(coder, NULL, coder_proccess, (void *)&data->coders[i]);
		pthread_join(*coder, NULL);
		i++;
	}
	
}

