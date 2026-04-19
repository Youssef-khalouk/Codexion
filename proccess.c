#include "codexion.h"

long long get_time_ms(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        return -1;
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

void setback_right_dongle(proccess_args_t* args, int dongle_id, char is_used)
{
	if (is_used)
		args->data->dongles[dongle_id].set_down_time = get_time_ms();
	pthread_mutex_unlock(&args->data->dongles[dongle_id].dongle);
	args->coder->right_dongle = NULL;
}

void setback_left_dongle(proccess_args_t* args, int dongle_id, char is_used)
{
	if (is_used)
		args->data->dongles[dongle_id].set_down_time = get_time_ms();
	pthread_mutex_unlock(&args->data->dongles[dongle_id].dongle);
	args->coder->left_dongle = NULL;
}



int request_right_dongle(proccess_args_t* args, long long start_time, int id)
{
	
}
int request_left_dongle(proccess_args_t* args, long long start_time, int id)
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
	start_time = get_time_ms();
	while (1)
	{
		right_dongle_id = request_right_dongle(args, start_time, right_dongle_id);
		left_dongle_id = request_left_dongle(args, start_time, left_dongle_id);

		compile(args, get_time_ms() - start_time);
		debug(args, get_time_ms() - start_time);
		refactor(args, get_time_ms() - start_time);

		setback_right_dongle(args, right_dongle_id, 1);
		setback_left_dongle(args, left_dongle_id, 1);

		args->coder->last_proccess_time = get_time_ms();
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

