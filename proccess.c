#include "codexion.h"

static long get_time_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return -1; // error

    return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

int get_right_dongle(proccess_args_t* args, long my_time)
{
	pthread_mutex_lock(&args->data->dongles[0].dongle);
	args->coder->right_dongle = &args->data->dongles[0];
	printf("time:%ld coder:%d has token right dongle %d.\n",
		my_time, args->coder->id, args->coder->right_dongle->id);
	
	if (args->coder->last_proccess_time &&
		get_time_ms() - args->coder->last_proccess_time > args->data->time_to_burnout)
	{
		printf("%d is burnout..............\n", args->coder->id);
		pthread_mutex_unlock(&args->data->dongles[0].dongle);
		return (0);
	}
	return (1);
}

int get_left_dongle(proccess_args_t* args, long my_time)
{
	pthread_mutex_lock(&args->data->dongles[1].dongle);
	args->coder->left_dongle = &args->data->dongles[1];
	printf("time:%ld coder:%d has token left dongle %d.\n",
		my_time, args->coder->id, args->coder->left_dongle->id);
	
	if (args->coder->last_proccess_time &&
		get_time_ms() - args->coder->last_proccess_time > args->data->time_to_burnout)
	{
		printf("%d is burnout..............\n", args->coder->id);
		pthread_mutex_unlock(&args->data->dongles[0].dongle);
		pthread_mutex_unlock(&args->data->dongles[1].dongle);
		return (0);
	}
	return (1);
}

void setback_right_dongle(proccess_args_t* args, int dongle_id)
{
	pthread_mutex_unlock(&args->data->dongles[dongle_id].dongle);
	args->coder->right_dongle = NULL;
}

void setback_left_dongle(proccess_args_t* args, int dongle_id)
{
	pthread_mutex_unlock(&args->data->dongles[dongle_id].dongle);
	args->coder->left_dongle = NULL;
}

static void* coder_proccess(void* args_t)
{
	int	compiled_times;
	long	start_time;

	compiled_times = 0;
	proccess_args_t* args = (proccess_args_t*)args_t;
	start_time = get_time_ms();

	while (1)
	{

		if (!get_right_dongle(args, get_time_ms() - start_time))
			return NULL;
		if (!get_left_dongle(args, get_time_ms() - start_time))
			return NULL;

		compile(args, get_time_ms() - start_time);
		debug(args, get_time_ms() - start_time);
		refactor(args, get_time_ms() - start_time);

		setback_right_dongle(args, 0);
		setback_left_dongle(args, 1);


		usleep(1000);
		args->coder->last_proccess_time = get_time_ms();
		compiled_times++;
		if (compiled_times >= args->data->number_of_compiles_required)
		{
			printf("%d is finished.....\n", args->coder->id);
			break;
		}
	}
	free(args_t);
	return NULL;
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

