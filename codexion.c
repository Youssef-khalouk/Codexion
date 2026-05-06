
#include "codexion.h"

static void	free_data(data_t* data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_mutex_destroy(&data->coders[i].working_mutix);
		pthread_mutex_destroy(&data->dongles[i].mutix_dongle);
		pthread_mutex_destroy(&data->dongles[i].mutix_queue);
		pthread_cond_destroy(&data->dongles[i].scheduler_cond);
		i++;
	}
	pthread_mutex_destroy(&data->stop_mutix);
	pthread_cond_destroy(&data->stop_condation);
	free(data->coders);
	free(data->dongles);
	free(data);
}

static void	init_coders_and_dongles(data_t* data)
{
	int	i;
	
	i = 0;
	data->coders = malloc(sizeof(coder_t) * data->number_of_coders);
	data->dongles = malloc(sizeof(usb_dongle_t) * data->number_of_coders);
	pthread_mutex_init(&data->stop_mutix, NULL);
	pthread_cond_init(&data->stop_condation, NULL);
	data->stop = 0;
	while (i < data->number_of_coders)
	{
		data->coders[i].id = i;
		data->coders[i].last_proccess_time = ms_time();
		data->coders[i].right_dongle = NULL;
		data->coders[i].left_dongle = NULL;
		data->coders[i].working = 0;
		data->coders[i].finish = 0;
		data->coders[i].r_d_id = i;
		data->coders[i].l_d_id = (i + 1) % data->number_of_coders;
		pthread_mutex_init(&data->coders[i].working_mutix, NULL);
		data->dongles[i].id = i;
		data->dongles[i].set_down_time = 0;
		data->dongles[i].queue.front = 0;
		data->dongles[i].queue.rear = 0;
		data->dongles[i].queue.size = 0;
		data->dongles[i].queue.owner_id = -1;
		data->dongles[i].queue.push_later = -1;
		data->dongles[i].queue.use_push_later = 1;
		// data->dongles[i].heap_queue.front = 0;
		// data->dongles[i].heap_queue.rear = 0;
		// data->dongles[i].heap_queue.size = 0;
		pthread_cond_init(&data->dongles[i].scheduler_cond, NULL);
		pthread_mutex_init(&data->dongles[i].mutix_dongle, NULL);
		pthread_mutex_init(&data->dongles[i].mutix_queue, NULL);
		i++;
	}
}

void	broadcast_all_coders(data_t* data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_mutex_lock(&data->dongles[i].mutix_queue);
		pthread_cond_broadcast(&data->dongles[i].scheduler_cond);
		pthread_mutex_unlock(&data->dongles[i].mutix_queue);
		i++;
	}
}

void* monitor(void* d)
{
	data_t*	data;
	int		index;
	int		all_done;
	long long	last_proccess_time;

	data = d;
	index = 0;
	all_done = 1;
	while (1)
	{
		if (index == data->number_of_coders)
		{
			usleep(2000);
			index = 0;
			if (all_done)
				return (NULL);
			all_done = 1;
		}
		pthread_mutex_lock(&data->coders[index].working_mutix);
		if (!data->coders[index].finish)
			all_done = 0;
		if (data->coders[index].working)
		{	
			pthread_mutex_unlock(&data->coders[index].working_mutix);
			index++;
			continue;
		}
		last_proccess_time = data->coders[index].last_proccess_time;
		pthread_mutex_unlock(&data->coders[index].working_mutix);

		if ((ms_time() - last_proccess_time) >= data->time_to_burnout)
		{
			pthread_mutex_lock(&data->stop_mutix);
			data->stop = 1;
			pthread_cond_broadcast(&data->stop_condation);
			pthread_mutex_unlock(&data->stop_mutix);
			broadcast_all_coders(data);
			printf("%lld %d burned out\n", ms_time() - data->start_time, data->coders[index].id);
			return (NULL);
		}
		index++;
	}
	return (NULL);
}


int	main(int argc, char **argv)
{
	data_t*	data;
	pthread_t	monitor_thread;
	long long	start_time;

	start_time = ms_time();
	data = parse_args(argc, argv);
	init_coders_and_dongles(data);
	if (data->error)
		return (free_data(data), 1);
	data->start_time = start_time;
	pthread_create(&monitor_thread, NULL, monitor, (void*)data);
	proccess(data, start_time);
	pthread_join(monitor_thread, NULL);

	return (free_data(data), 0);
}
