/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 14:46:48 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/10 15:10:26 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	just_one(t_args *args)
{
	take_dongle_when_ready(args, &args->data->dongles[0], 'r');
	while (1)
	{
		pthread_mutex_lock(&args->data->stop_mutix);
		if (args->data->stop)
		{
			pthread_mutex_unlock(&args->data->stop_mutix);
			setback_dongles(args);
			break ;
		}
		pthread_mutex_unlock(&args->data->stop_mutix);
		usleep(2000);
	}
	return (0);
}

static void	lock_mutix_queue(t_dongle *r_dongle, t_dongle *l_dongle)
{
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
}

static int	unlock_mutix_queue(t_dongle *r_dongle, t_dongle *l_dongle,
		t_args *args, int edf)
{
	if (r_dongle->id % 2)
	{
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		usleep(2000);
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		if (!request_right_d(args, edf))
			return (0);
		if (!request_left_d(args, edf))
			return (0);
	}
	else
	{
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		usleep(2000);
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		if (!request_left_d(args, edf))
			return (0);
		if (!request_right_d(args, edf))
			return (0);
	}
	return (1);
}

int	request_dongles(t_args *args, int edf)
{
	t_dongle	*r_dongle;
	t_dongle	*l_dongle;

	if (args->data->number_of_coders == 1)
		return (just_one(args));
	r_dongle = &args->data->dongles[args->coder->r_d_id];
	l_dongle = &args->data->dongles[args->coder->l_d_id];
	lock_mutix_queue(r_dongle, l_dongle);
	push_back_if_missing(&r_dongle->queue, args->coder->id);
	push_back_if_missing(&l_dongle->queue, args->coder->id);
	return (unlock_mutix_queue(r_dongle, l_dongle, args, edf));
}
