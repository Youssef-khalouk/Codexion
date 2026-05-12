/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 20:09:48 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/12 17:04:59 by ykhalouk         ###   ########.fr       */
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
		pthread_mutex_lock(&data->coders[index].last_time_mutix);
		if (!data->coders[index].finish)
			all_done = 0;
		last_proccess_time = data->coders[index].last_proccess_time;
		pthread_mutex_unlock(&data->coders[index].last_time_mutix);
		if ((ms_time() - last_proccess_time) >= data->time_to_burnout)
			return (some_one_bornout(data, index), NULL);
		index++;
	}
}
