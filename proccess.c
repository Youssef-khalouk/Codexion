#include "codexion.h"

long long ms_time(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        return -1;
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}



void setback_dongles(proccess_args_t* args)
{
	if (args->coder->right_dongle)
		args->coder->right_dongle->set_down_time = ms_time();
	pthread_mutex_unlock(args->coder->right_dongle);
	args->coder->right_dongle = NULL;

	if (args->coder->left_dongle)
		args->coder->left_dongle->set_down_time = ms_time();
	pthread_mutex_unlock(args->coder->left_dongle);
	args->coder->left_dongle = NULL;
}


int request_dongles(proccess_args_t* args, long long start_time)
{
	
}

static void* coder_proccess(void* args_t)
{
	int		compiled_times;
	long	start_time;
	int		right_dongle_id;
	int		left_dongle_id;

	proccess_args_t* args = (proccess_args_t*)args_t;
	compiled_times = 0;
	start_time = ms_time();
	while (1)
	{
		request_dongles(args, start_time);

		if (!compile(args, start_time))
			return (free(args_t), NULL);
		if (!debug(args, start_time))
			return (free(args_t), NULL);
		if (!refactor(args, start_time))
			return (free(args_t), NULL);

		setback_dongles(args);

		args->coder->last_proccess_time = ms_time();
		compiled_times++;

		// stop the coder when he finish hes compiles
		if (compiled_times >= args->data->number_of_compiles_required)
			return (free(args_t), NULL);
	}
}

void proccess(data_t* data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		proccess_args_t* args = malloc(sizeof(proccess_args_t));
		args->coder = &data->coders[i];
		args->data = data;

		pthread_t *coder = malloc(sizeof(pthread_t));
		data->coders[i].thread_id = coder;
		pthread_create(coder, NULL, coder_proccess, (void *)args);
		i++;
	}
	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_join(*data->coders[i].thread_id, NULL);
		i++;
	}
}

