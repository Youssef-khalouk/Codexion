#include "codexion.h"

long long get_time_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return -1; // error

    return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
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

int get_right_dongle(proccess_args_t* args, long long start_time, int id)
{
	long long	time_passed;
	// if dongle is not cooldown calculate the left time
	// and sleep for that time and check again if its ready to use
	// becuse another thread could take it thats why 
	while (1)
	{
		pthread_mutex_lock(&args->data->dongles[id].dongle);
		time_passed = get_time_ms() - args->data->dongles[id].set_down_time;

		if (get_time_ms() - args->coder->last_proccess_time > args->data->time_to_burnout)
		{
			printf("%lld %d burned out\n", get_time_ms() - start_time, args->coder->id);
			return (setback_right_dongle(args, id, 0), 0);
		}
		if (time_passed >= args->data->dongle_cooldown)
		{
			args->coder->right_dongle = &args->data->dongles[id];
			printf("%lld %d has token a dongle %d.\n",
				get_time_ms() - start_time , args->coder->id, args->coder->right_dongle->id);
			return (1);
		}
		pthread_mutex_unlock(&args->data->dongles[id].dongle);
		usleep((args->data->dongle_cooldown - time_passed) * 1000);
	}
}

int get_left_dongle(proccess_args_t* args, long long start_time, int id)
{
	long long	time_passed;
	while (1)
	{
		pthread_mutex_lock(&args->data->dongles[id].dongle);
		time_passed = get_time_ms() - args->data->dongles[id].set_down_time;
		if (get_time_ms() - args->coder->last_proccess_time > args->data->time_to_burnout)
		{
			printf("%lld %d burned out\n", get_time_ms() - start_time, args->coder->id);
			return (setback_left_dongle(args, id, 0), 0);
		}
		if (time_passed >= args->data->dongle_cooldown)
		{
			args->coder->left_dongle = &args->data->dongles[id];
			printf("%lld %d has token a dongle %d.\n",
				get_time_ms() - start_time, args->coder->id, args->coder->left_dongle->id);
			return (1);
		}
		pthread_mutex_unlock(&args->data->dongles[id].dongle);
		usleep((args->data->dongle_cooldown - time_passed) * 1000);
	}
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
		right_dongle_id = args->coder->id;
		left_dongle_id = (right_dongle_id+1) % args->data->number_of_coders;

		if (args->coder->id % 2)
		{
			if (!get_right_dongle(args, start_time, right_dongle_id))
				return (free(args_t), NULL);
			if (!get_left_dongle(args, start_time, left_dongle_id))
				return (setback_right_dongle(args, right_dongle_id, 0), free(args_t), NULL);
		}
		else
		{
			if (!get_left_dongle(args, start_time, left_dongle_id))
				return (NULL);
			if (!get_right_dongle(args, start_time, right_dongle_id))
				return (setback_left_dongle(args, left_dongle_id, 0), free(args_t), NULL);
		}

		compile(args, get_time_ms() - start_time);
		debug(args, get_time_ms() - start_time);
		refactor(args, get_time_ms() - start_time);

		setback_right_dongle(args, right_dongle_id, 1);
		setback_left_dongle(args, left_dongle_id, 1);


		usleep(1000);
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

