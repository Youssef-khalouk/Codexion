/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operations.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:35 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/09 18:17:19 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_time(struct timespec *ts, long ms)
{
	clock_gettime(CLOCK_REALTIME, ts);
	ts->tv_sec += ms / 1000;
	ts->tv_nsec += (ms % 1000) * 1000000;
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec += 1;
		ts->tv_nsec -= 1000000000;
	}
}

void	print(t_print_type type, t_args *args)
{
	long long	cr_time;

	cr_time = ms_time() - args->start_time;
	pthread_mutex_lock(&args->data->print_mutix);
	if (type == COMPILING)
		printf("%-6lld %d is compiling\n", cr_time, args->coder->id + 1);
	if (type == DEBUGGING)
		printf("%-6lld %d is debugging\n", cr_time, args->coder->id + 1);
	if (type == REFACTORING)
		printf("%-6lld %d is refactoring\n", cr_time, args->coder->id + 1);
	if (type == TAKEDONGLE)
		printf("%-6lld %d has taken a dongle\n", cr_time, args->coder->id + 1);
	if (type == BORNOUT)
		printf("%-6lld %d burned out\n", cr_time, args->coder->id + 1);
	pthread_mutex_unlock(&args->data->print_mutix);
}

int	compile(t_args *args)
{
	long long		curent_time;
	int				stopped;
	int				ret;
	struct timespec	ts;
	t_data			*data;

	data = args->data;
	curent_time = ms_time();
	print(COMPILING, args);
	wait_time(&ts, data->time_to_compile);
	pthread_mutex_lock(&data->stop_mutix);
	while (!data->stop && (ms_time() - curent_time) < data->time_to_compile)
	{
		ret = pthread_cond_timedwait(&data->stop_cond, &data->stop_mutix, &ts);
		if (ret == 110)
			break ;
	}
	stopped = data->stop;
	pthread_mutex_unlock(&data->stop_mutix);
	if (stopped)
		return (0);
	return (1);
}

int	debug(t_args *args)
{
	long long		curent_time;
	int				stopped;
	int				ret;
	struct timespec	ts;
	t_data			*data;

	data = args->data;
	curent_time = ms_time();
	print(DEBUGGING, args);
	wait_time(&ts, data->time_to_debug);
	pthread_mutex_lock(&data->stop_mutix);
	while (!data->stop && (ms_time() - curent_time) < data->time_to_debug)
	{
		ret = pthread_cond_timedwait(&data->stop_cond, &data->stop_mutix, &ts);
		if (ret == 110)
			break ;
	}
	stopped = data->stop;
	pthread_mutex_unlock(&data->stop_mutix);
	if (stopped)
		return (0);
	return (1);
}

int	refactor(t_args *args)
{
	long long		curent_time;
	int				stopped;
	int				ret;
	struct timespec	ts;
	t_data			*data;

	data = args->data;
	curent_time = ms_time();
	print(REFACTORING, args);
	wait_time(&ts, data->time_to_refactor);
	pthread_mutex_lock(&data->stop_mutix);
	while (!data->stop && (ms_time() - curent_time) < data->time_to_refactor)
	{
		ret = pthread_cond_timedwait(&data->stop_cond,
				&data->stop_mutix, &ts);
		if (ret == 110)
			break ;
	}
	stopped = data->stop;
	pthread_mutex_unlock(&data->stop_mutix);
	if (stopped)
		return (0);
	return (1);
}