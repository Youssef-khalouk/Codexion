#include "codexion.h"

void	push_back(t_queue* queue, int value)
{
	queue->buffer[queue->rear] = value;
	queue->rear++;
	queue->size++;
}

int pop_front(t_queue* queue)
{
	int	i;
	int	value;

	if (queue->size == 0)
		return (-1);
	i = 0;
	value = queue->buffer[queue->front];
	while (i < queue->size - 1)
	{
		queue->buffer[i] = queue->buffer[i+1];
		i++;
	}
	queue->size--;
	queue->rear--;
	return (value);
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
		pop_front(&args->data->dongles[args->coder->r_d_id].queue);
		pthread_cond_broadcast(&args->data->dongles[args->coder->r_d_id].scheduler_cond);
		pthread_mutex_unlock(&args->coder->right_dongle->mutix_dongle);
	}
	args->coder->right_dongle = NULL;

	if (args->coder->left_dongle)
	{
		args->coder->left_dongle->set_down_time = ms_time();
		pop_front(&args->data->dongles[args->coder->l_d_id].queue);
		pthread_cond_broadcast(&args->data->dongles[args->coder->l_d_id].scheduler_cond);
		pthread_mutex_unlock(&args->coder->left_dongle->mutix_dongle);
	}
	args->coder->left_dongle = NULL;
}

// wait tell its cooldown and take it
int take_dongle_when_ready(proccess_args_t* args, usb_dongle_t* dongle, char r_l)
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

        pthread_mutex_lock(&dongle->mutix_dongle);

        elapsed = ms_time() - dongle->set_down_time;
        if (elapsed >= args->data->dongle_cooldown)
		{
			if (r_l == 'r')
				args->coder->right_dongle = dongle;
			else if (r_l == 'l')
				args->coder->left_dongle = dongle;
            return 1;
		}
        pthread_mutex_unlock(&dongle->mutix_dongle);
        usleep(2000);
    }
}

int	fifo_r_right_d(proccess_args_t* args)
{
	usb_dongle_t*	r_dongle;

	r_dongle = &args->data->dongles[args->coder->r_d_id];
	pthread_mutex_lock(&r_dongle->mutix_dongle);
	push_back(&r_dongle->queue, args->coder->id);
	
	while (1)
	{
		if (r_dongle->queue.buffer[0] == args->coder->id)
			break;
		pthread_cond_wait(&r_dongle->scheduler_cond, &r_dongle->mutix_dongle);
	}
	pthread_mutex_unlock(&r_dongle->mutix_dongle);

	if (!take_dongle_when_ready(args, r_dongle, 'r'))
		return (0);
	return (1);
}

int	fifo_r_left_d(proccess_args_t* args)
{
	usb_dongle_t*	l_dongle;

	l_dongle = &args->data->dongles[args->coder->l_d_id];
	pthread_mutex_lock(&l_dongle->mutix_dongle);
	push_back(&l_dongle->queue, args->coder->id);
	
	while (1)
	{
		if (l_dongle->queue.buffer[0] == args->coder->id)
			break;
		pthread_cond_wait(&l_dongle->scheduler_cond, &l_dongle->mutix_dongle);
	}
	pthread_mutex_unlock(&l_dongle->mutix_dongle);

	if (!take_dongle_when_ready(args, l_dongle, 'l'))
		return (0);
	return (1);
}

int fifo_request_dongles(proccess_args_t* args, long long start_time)
{
	// pthread_mutex_lock(&r_dongle->mutix_dongle);
	// push_back(&r_dongle->queue, args->coder->id);
	// pthread_mutex_unlock(&r_dongle->mutix_dongle);

	// pthread_mutex_lock(&l_dongle->mutix_dongle);
	// push_back(&l_dongle->queue, args->coder->id);
	// pthread_mutex_unlock(&l_dongle->mutix_dongle);

	// the fix is that i need to get the position in the queue and then try to get dongles thats the inser

	if (args->coder->id % 2)
	{
		if (!fifo_r_left_d(args))
			return (0);
		printf("%lld %d has taken a dongle\n", ms_time() - start_time, args->coder->id);
		
		if (!fifo_r_right_d(args))
			return (0);
		printf("%lld %d has taken a dongle\n", ms_time() - start_time, args->coder->id);
		return (1);
	}
	
	if (!fifo_r_right_d(args))
		return (0);
	printf("%lld %d has taken a dongle\n", ms_time() - start_time, args->coder->id);
	
	if (!fifo_r_left_d(args))
		return (0);
	printf("%lld %d has taken a dongle\n", ms_time() - start_time, args->coder->id);
	
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

