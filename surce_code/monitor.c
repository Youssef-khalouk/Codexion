/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 20:09:48 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/09 20:10:01 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	broadcast_all_coders(t_data *data)
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

static void	some_one_bornout(t_data *data, int id)
{
	t_args		args;

	args.data = data;
	args.start_time = data->start_time;
	pthread_mutex_lock(&data->stop_mutix);
	data->stop = 1;
	pthread_cond_broadcast(&data->stop_cond);
	pthread_mutex_unlock(&data->stop_mutix);
	broadcast_all_coders(data);
	args.coder = &data->coders[id];
	print(BORNOUT, &args);
}

static int	reset_monitor_loop(int *index, int *all_done, t_data *data)
{
	if (*index == data->number_of_coders)
	{
		usleep(2000);
		*index = 0;
		if (*all_done)
			return (0);
		*all_done = 1;
	}
	return (1);
}

static int	coder_is_working(t_data *data, int i, long long *l_p_t, int *done)
{
	int	working;

	pthread_mutex_lock(&data->coders[i].working_mutix);
	if (!data->coders[i].finish)
		*done = 0;
	working = data->coders[i].working;
	*l_p_t = data->coders[i].last_proccess_time;
	pthread_mutex_unlock(&data->coders[i].working_mutix);
	return (working);
}

void	*monitor(void *d)
{
	t_data		*data;
	int			index;
	int			all_done;
	long long	last_proccess_time;

	data = d;
	index = 0;
	all_done = 1;
	while (1)
	{
		if (!reset_monitor_loop(&index, &all_done, data))
			return (NULL);
		if (coder_is_working(data, index, &last_proccess_time, &all_done))
		{
			index++;
			continue ;
		}
		if ((ms_time() - last_proccess_time) >= data->time_to_burnout)
			return (some_one_bornout(data, index), NULL);
		index++;
	}
}
