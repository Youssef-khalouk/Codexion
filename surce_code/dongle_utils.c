/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 23:55:25 by marvin            #+#    #+#             */
/*   Updated: 2026/05/10 15:13:32 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	simulation_stoped(t_args *args)
{
	pthread_mutex_lock(&args->data->stop_mutix);
	if (args->data->stop)
	{
		pthread_mutex_unlock(&args->data->stop_mutix);
		return (1);
	}
	pthread_mutex_unlock(&args->data->stop_mutix);
	return (0);
}

void	setback_dongles(t_args *args)
{
	t_dongle	*r_dongle;
	t_dongle	*l_dongle;

	r_dongle = &args->data->dongles[args->coder->r_d_id];
	l_dongle = &args->data->dongles[args->coder->l_d_id];
	if (args->coder->right_dongle)
	{
		pthread_mutex_lock(&r_dongle->mutix_queue);
		r_dongle->set_down_time = ms_time();
		pop_front(&r_dongle->queue);
		pthread_cond_broadcast(&r_dongle->scheduler_cond);
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		pthread_mutex_unlock(&r_dongle->mutix_dongle);
		args->coder->right_dongle = NULL;
	}
	if (args->coder->left_dongle)
	{
		pthread_mutex_lock(&l_dongle->mutix_queue);
		l_dongle->set_down_time = ms_time();
		pop_front(&l_dongle->queue);
		pthread_cond_broadcast(&l_dongle->scheduler_cond);
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		pthread_mutex_unlock(&l_dongle->mutix_dongle);
		args->coder->left_dongle = NULL;
	}
}

int	take_dongle_when_ready(t_args *args, t_dongle *dongle, char r_l)
{
	long long	elapsed;

	while (1)
	{
		if (simulation_stoped(args))
			return (0);
		pthread_mutex_lock(&dongle->mutix_dongle);
		elapsed = ms_time() - dongle->set_down_time;
		if (elapsed >= args->data->dongle_cooldown)
		{
			if (r_l == 'r')
				args->coder->right_dongle = dongle;
			else if (r_l == 'l')
				args->coder->left_dongle = dongle;
			print(TAKEDONGLE, args);
			return (1);
		}
		pthread_mutex_unlock(&dongle->mutix_dongle);
		usleep(2000);
	}
}

int	request_right_d(t_args *args, int edf)
{
	t_dongle	*r_dongle;

	r_dongle = &args->data->dongles[args->coder->r_d_id];
	pthread_mutex_lock(&r_dongle->mutix_queue);
	while (1)
	{
		if (simulation_stoped(args))
		{
			pthread_mutex_unlock(&r_dongle->mutix_queue);
			return (0);
		}
		if (edf)
			heap_deadline(&r_dongle->queue, args);
		if (r_dongle->queue.buffer[0] == args->coder->id)
			break ;
		pthread_cond_wait(&r_dongle->scheduler_cond, &r_dongle->mutix_queue);
	}
	pthread_mutex_unlock(&r_dongle->mutix_queue);
	if (!take_dongle_when_ready(args, r_dongle, 'r'))
		return (0);
	return (1);
}

int	request_left_d(t_args *args, int edf)
{
	t_dongle	*l_dongle;

	l_dongle = &args->data->dongles[args->coder->l_d_id];
	pthread_mutex_lock(&l_dongle->mutix_queue);
	while (1)
	{
		if (simulation_stoped(args))
		{
			pthread_mutex_unlock(&l_dongle->mutix_queue);
			return (0);
		}
		if (edf)
			heap_deadline(&l_dongle->queue, args);
		if (l_dongle->queue.buffer[0] == args->coder->id)
			break ;
		pthread_cond_wait(&l_dongle->scheduler_cond, &l_dongle->mutix_queue);
	}
	pthread_mutex_unlock(&l_dongle->mutix_queue);
	if (!take_dongle_when_ready(args, l_dongle, 'l'))
		return (0);
	return (1);
}
