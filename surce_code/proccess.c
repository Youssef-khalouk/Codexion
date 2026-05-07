/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proccess.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:39 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/08 00:09:51 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long ms_time(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        return -1;
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

int	just_one(t_args* args)
{

	take_dongle_when_ready(args, &args->data->dongles[0], 'r');
	while (1)
    {
        pthread_mutex_lock(&args->data->stop_mutix);
		if (args->data->stop)
		{
			pthread_mutex_unlock(&args->data->stop_mutix);
			setback_dongles(args);
			break;
		}
        pthread_mutex_unlock(&args->data->stop_mutix);
		usleep(2000);
    }
	return (0);
}

int request_dongles(t_args* args, int edf)
{
	t_dongle*	r_dongle;
	t_dongle*	l_dongle;

	if (args->data->number_of_coders == 1)
		return (just_one(args));

	r_dongle = &args->data->dongles[args->coder->r_d_id];
	l_dongle = &args->data->dongles[args->coder->l_d_id];

	if (r_dongle->id % 2)
	{
		pthread_mutex_lock(&r_dongle->mutix_queue);
		usleep(2000);
		pthread_mutex_lock(&l_dongle->mutix_queue);
	}
	else
	{
		pthread_mutex_lock(&l_dongle->mutix_queue);
		usleep(2000);
		pthread_mutex_lock(&r_dongle->mutix_queue);
	}
	
	push_back_if_missing(&r_dongle->queue, args->coder->id);
	push_back_if_missing(&l_dongle->queue, args->coder->id);

	if (r_dongle->id % 2)
	{
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		usleep(2000);
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		if (!request_right_d(args, edf))
			return 0;
		if (!request_left_d(args, edf))
			return 0;
	}
	else
	{
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		usleep(2000);
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		if (!request_left_d(args, edf))
			return 0;
		if (!request_right_d(args, edf))
			return 0;
	}
    return (1);
}

static void* coder_proccess(void* args_t)
{
	int		compiled_times;

	t_args* args = (t_args*)args_t;
	compiled_times = 0;
	args->start_time = ms_time();
	while (compiled_times < args->data->number_of_compiles_required)
	{
		if (!request_dongles(args, strcmp(args->data->scheduler, "fifo")))
			break;

		pthread_mutex_lock(&args->coder->working_mutix);
		args->coder->working = 1;
		pthread_mutex_unlock(&args->coder->working_mutix);
		
		if (!compile(args))
			break;

		setback_dongles(args);

		if (!debug(args))
			break;
		if (!refactor(args))
			break;

		pthread_mutex_lock(&args->coder->working_mutix);
		args->coder->last_proccess_time = ms_time();
		args->coder->working = 0;
		pthread_mutex_unlock(&args->coder->working_mutix);
		compiled_times++;
	}
	setback_dongles(args);
	pthread_mutex_lock(&args->coder->working_mutix);
	args->coder->finish = 1;
	pthread_mutex_unlock(&args->coder->working_mutix);
	return (free(args_t), NULL);
}

void proccess_data(t_data* data, long long start_time)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		t_args* args = malloc(sizeof(t_args));
		args->coder = &data->coders[i];
		args->data = data;
		args->start_time = start_time;

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

