#include "codexion.h"

int min(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}


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
	{
		args->coder->right_dongle->set_down_time = ms_time();
		pthread_mutex_unlock(&args->coder->right_dongle->dongle);
	}
	args->coder->right_dongle = NULL;

	if (args->coder->left_dongle)
	{
		args->coder->left_dongle->set_down_time = ms_time();
		pthread_mutex_unlock(&args->coder->left_dongle->dongle);
	}
	args->coder->left_dongle = NULL;
	
	pthread_mutex_lock(&args->data->queue_mutex);
	args->data->serving_ticket++;
	pthread_cond_broadcast(&args->data->queue_cond);
	pthread_mutex_unlock(&args->data->queue_mutex);
}

// wait tell its cooldown and take it
int take_dongle_when_ready(proccess_args_t* args, usb_dongle_t* dongle)
{
	long long	elapsed;

    while (1)
    {
		pthread_mutex_lock(&args->data->stop_mutix);
		if (args->data->stop)
		{
			pthread_mutex_unlock(&args->data->stop_mutix);
			return (0);
		}
		pthread_mutex_unlock(&args->data->stop_mutix);

        pthread_mutex_lock(&dongle->dongle);

        elapsed = ms_time() - dongle->set_down_time;
        if (elapsed >= args->data->dongle_cooldown)
            return 1;

        pthread_mutex_unlock(&dongle->dongle);
        usleep(2000);
    }
}

int fifo_request_dongles(proccess_args_t* args, long long start_time)
{
	int			right;
	int			left;
	unsigned int my_turn;

	right = args->coder->id - 1;
	left = (args->coder->id) % args->data->number_of_coders;
    pthread_mutex_lock(&args->data->queue_mutex);
    my_turn = args->data->next_ticket++;
    while (my_turn != args->data->serving_ticket)
	{
		pthread_mutex_lock(&args->data->stop_mutix);
		if (args->data->stop)
		{
			pthread_mutex_unlock(&args->data->stop_mutix);
			pthread_mutex_unlock(&args->data->queue_mutex);
			return (0);
		}
		pthread_mutex_unlock(&args->data->stop_mutix);

        pthread_cond_wait(&args->data->queue_cond, &args->data->queue_mutex);
	}
	if (!take_dongle_when_ready(args, &args->data->dongles[min(right, left)]))
		return (pthread_mutex_unlock(&args->data->queue_mutex), 0);

    printf("%lld %d has taken a dongle\n", ms_time() - start_time, args->coder->id);

	if (!take_dongle_when_ready(args, &args->data->dongles[max(right, left)]))
	{
		pthread_mutex_unlock(&args->data->dongles[min(right, left)].dongle);
		pthread_mutex_unlock(&args->data->queue_mutex);
		return (0);
	}
    printf("%lld %d has taken a dongle\n", ms_time() - start_time, args->coder->id);

	pthread_mutex_unlock(&args->data->queue_mutex);
	args->coder->right_dongle = &args->data->dongles[right];
    args->coder->left_dongle  = &args->data->dongles[left];
    return (1);
}

static void* coder_proccess(void* args_t)
{
	int		compiled_times;
	long	start_time;

	proccess_args_t* args = (proccess_args_t*)args_t;
	compiled_times = 0;
	start_time = ms_time();
	while (1)
	{
		if (!fifo_request_dongles(args, start_time))
			break;
		pthread_mutex_lock(&args->coder->working_mutix);
		args->coder->working = 1;
		pthread_mutex_unlock(&args->coder->working_mutix);
		
		if (!compile(args, start_time))
			break;
		if (!debug(args, start_time))
			break;
		if (!refactor(args, start_time))
			break;

		setback_dongles(args);
		
		compiled_times++; // stop the coder when he finish hes compiles
		if (compiled_times >= args->data->number_of_compiles_required)
		{
			printf("the coder number %d is finished hes work now\n", args->coder->id);
			break;
		}
		// after if coder finish so if he finish the working still  = 1
		// so that the monitor skeep it 
		args->coder->last_proccess_time = ms_time();
		pthread_mutex_lock(&args->coder->working_mutix);
		args->coder->working = 0;
		pthread_mutex_unlock(&args->coder->working_mutix);
	}
	setback_dongles(args);
	free(args_t);
	return (NULL);

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

		// pthread_t *coder = malloc(sizeof(pthread_t));
		// data->coders[i].thread_id = coder;
		pthread_create(&data->coders[i].thread_id, NULL, coder_proccess, (void *)args);
		i++;
	}
	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_join(data->coders[i].thread_id, NULL);
		i++;
	}
}

