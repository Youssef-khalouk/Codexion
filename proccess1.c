/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proccess.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:39 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/07 19:55:52 by ykhalouk         ###   ########.fr       */
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

int simulation_stoped(t_args * args)
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

void setback_dongles(t_args* args, int	coder_finished)
{
	if (args->coder->right_dongle)
	{
		pthread_mutex_lock(&args->data->dongles[args->coder->r_d_id].mutix_queue);
		args->coder->right_dongle->set_down_time = ms_time();

		// if (args->data->dongles[args->coder->r_d_id].queue.owner_id == args->coder->id)
		// {
		pop_front(&args->data->dongles[args->coder->r_d_id].queue, coder_finished);
			// args->data->dongles[args->coder->r_d_id].queue.owner_id = -1;
		// }

		pthread_cond_broadcast(&args->data->dongles[args->coder->r_d_id].scheduler_cond);
		pthread_mutex_unlock(&args->data->dongles[args->coder->r_d_id].mutix_queue);
		pthread_mutex_unlock(&args->coder->right_dongle->mutix_dongle);
	}
	args->coder->right_dongle = NULL;

	if (args->coder->left_dongle)
	{
		pthread_mutex_lock(&args->data->dongles[args->coder->l_d_id].mutix_queue);
		args->coder->left_dongle->set_down_time = ms_time();
		// if (args->data->dongles[args->coder->l_d_id].queue.owner_id == args->coder->id)
		// {
		pop_front(&args->data->dongles[args->coder->l_d_id].queue, coder_finished);
			// args->data->dongles[args->coder->l_d_id].queue.owner_id = -1;
		// }
		pthread_cond_broadcast(&args->data->dongles[args->coder->l_d_id].scheduler_cond);
		pthread_mutex_unlock(&args->data->dongles[args->coder->l_d_id].mutix_queue);
		pthread_mutex_unlock(&args->coder->left_dongle->mutix_dongle);
	}
	args->coder->left_dongle = NULL;
}

int take_dongle_when_ready(t_args* args, t_dongle* dongle, char r_l)
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
			printf("%-6lld %d has taken a dongle\n",
				ms_time() - args->start_time, args->coder->id + 1);
            return 1;
		}
        pthread_mutex_unlock(&dongle->mutix_dongle);
        usleep(2000);
    }
}

int	fifo_rq_right_d(t_args* args)
{
	t_dongle*	r_dongle;

	r_dongle = &args->data->dongles[args->coder->r_d_id];
	pthread_mutex_lock(&r_dongle->mutix_queue);
	while (1)
	{
		if (simulation_stoped(args))
		{
			pthread_mutex_unlock(&r_dongle->mutix_queue);
			return (0);
		}
		if (r_dongle->queue.buffer[0] == args->coder->id)
			break;
		pthread_cond_wait(&r_dongle->scheduler_cond, &r_dongle->mutix_queue);
	}
	if (!take_dongle_when_ready(args, r_dongle, 'r'))
	{
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		return (0);
	}
	pthread_mutex_unlock(&r_dongle->mutix_queue);
	return (1);
}

int	fifo_rq_left_d(t_args* args)
{
	t_dongle*	l_dongle;

	l_dongle = &args->data->dongles[args->coder->l_d_id];
	pthread_mutex_lock(&l_dongle->mutix_queue);
	while (1)
	{
		if (simulation_stoped(args))
		{
			pthread_mutex_unlock(&l_dongle->mutix_queue);
			return (0);
		}
		if (l_dongle->queue.buffer[0] == args->coder->id)
			break;
		pthread_cond_wait(&l_dongle->scheduler_cond, &l_dongle->mutix_queue);
	}
	if (!take_dongle_when_ready(args, l_dongle, 'l'))
	{
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		return (0);
	}
	pthread_mutex_unlock(&l_dongle->mutix_queue);
	return (1);
}

int	just_one(t_args* args)
{

	take_dongle_when_ready(args, &args->data->dongles[0], 'r');
	while (1)
    {
        pthread_mutex_lock(&args->data->stop_mutix);
		if (args->data->stop)
		{
			setback_dongles(args, 0);
			break;
		}
        pthread_mutex_unlock(&args->data->stop_mutix);
		usleep(2000);
    }
	return (0);
}

int fifo_request_dongles(t_args* args)
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
		pthread_mutex_lock(&l_dongle->mutix_queue);
	}
	else
	{
		pthread_mutex_lock(&l_dongle->mutix_queue);
		pthread_mutex_lock(&r_dongle->mutix_queue);
	}
	
	if (r_dongle->queue.size >= 1 && l_dongle->queue.size >= 1)
	{
		push_back_if_missing(&r_dongle->queue, args->coder->id);
		push_back_if_missing(&l_dongle->queue, args->coder->id);
	}
	else if (r_dongle->queue.size == 0 && l_dongle->queue.size == 0)
	{
		push_back(&r_dongle->queue, args->coder->id);
		push_back(&l_dongle->queue, args->coder->id);

		if (r_dongle->queue.push_later != -1)
		{
			push_back(&r_dongle->queue, r_dongle->queue.push_later);
			r_dongle->queue.push_later = -1;
		}
		if (l_dongle->queue.push_later != -1)
		{
			push_back(&l_dongle->queue, l_dongle->queue.push_later);
			l_dongle->queue.push_later = -1;
		}
	}
	else if (r_dongle->queue.size == 1 && l_dongle->queue.size == 0)
	{
		push_back(&r_dongle->queue, args->coder->id);
		if (!l_dongle->queue.use_push_later)
			push_back(&l_dongle->queue, args->coder->id);
		else if (l_dongle->queue.push_later == -1 &&
    			!queue_has(&l_dongle->queue, args->coder->id))
			l_dongle->queue.push_later = args->coder->id;
		else
		{
			push_back(&l_dongle->queue, args->coder->id);
			push_back(&l_dongle->queue, l_dongle->queue.push_later);
			l_dongle->queue.push_later = -1;
		}
	}
	else if (r_dongle->queue.size == 0 && l_dongle->queue.size == 1)
	{
		push_back(&l_dongle->queue, args->coder->id);
		if (!r_dongle->queue.use_push_later)
			push_back(&r_dongle->queue, args->coder->id);
		else if (r_dongle->queue.push_later == -1 &&
    			!queue_has(&r_dongle->queue, args->coder->id))
			r_dongle->queue.push_later = args->coder->id;
		else
		{
			push_back(&r_dongle->queue, args->coder->id);
			push_back(&r_dongle->queue, r_dongle->queue.push_later);
			r_dongle->queue.push_later = -1;
		}
	}

	if (r_dongle->id % 2)
	{
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		if (!fifo_rq_right_d(args))
			return 0;
		if (!fifo_rq_left_d(args))
			return 0;
	}
	else
	{
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		if (!fifo_rq_left_d(args))
			return 0;
		if (!fifo_rq_right_d(args))
			return 0;
	}
	
    return (1);
}


int	edf_rq_right_d(t_args* args)
{
	t_dongle*	r_dongle;

	r_dongle = &args->data->dongles[args->coder->r_d_id];
	pthread_mutex_lock(&r_dongle->mutix_queue);
	while (1)
	{
		if (simulation_stoped(args))
		{
			pthread_mutex_unlock(&r_dongle->mutix_queue);
			return (0);
		}
		heap_deadline(&r_dongle->queue, args);
		if (r_dongle->queue.buffer[0] == args->coder->id)
			break;
		pthread_cond_wait(&r_dongle->scheduler_cond, &r_dongle->mutix_queue);
	}

	if (!take_dongle_when_ready(args, r_dongle, 'r'))
	{
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		return (0);
	}
	pthread_mutex_unlock(&r_dongle->mutix_queue);
	return (1);
}

int	edf_rq_left_d(t_args* args)
{
	t_dongle*	l_dongle;

	l_dongle = &args->data->dongles[args->coder->l_d_id];
	pthread_mutex_lock(&l_dongle->mutix_queue);
	while (1)
	{
		if (simulation_stoped(args))
		{
			pthread_mutex_unlock(&l_dongle->mutix_queue);
			return (0);
		}
		heap_deadline(&l_dongle->queue, args);
		if (l_dongle->queue.buffer[0] == args->coder->id)
			break;
		pthread_cond_wait(&l_dongle->scheduler_cond, &l_dongle->mutix_queue);
	}
	if (!take_dongle_when_ready(args, l_dongle, 'l'))
	{
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		return (0);
	}
	pthread_mutex_unlock(&l_dongle->mutix_queue);
	return (1);
}


int edf_request_dongles(t_args* args)
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
		if (!edf_rq_right_d(args))
			return 0;
		if (!edf_rq_left_d(args))
			return 0;
	}
	else
	{
		pthread_mutex_unlock(&l_dongle->mutix_queue);
		usleep(2000);
		pthread_mutex_unlock(&r_dongle->mutix_queue);
		if (!edf_rq_left_d(args))
			return 0;
		if (!edf_rq_right_d(args))
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
		if (strcmp(args->data->scheduler, "fifo") == 0)
		{
			if (!fifo_request_dongles(args))
				break;
		}
		else
		{
			if (!edf_request_dongles(args))
				break;
		}
		pthread_mutex_lock(&args->coder->working_mutix);
		args->coder->working = 1;
		pthread_mutex_unlock(&args->coder->working_mutix);
		
		if (!compile(args))
			break;

		if (++compiled_times >= args->data->number_of_compiles_required)
			setback_dongles(args, 1);
		else
			setback_dongles(args, 0);

		if (!debug(args))
			break;
		if (!refactor(args))
			break;

		pthread_mutex_lock(&args->coder->working_mutix);
		args->coder->last_proccess_time = ms_time();
		args->coder->working = 0;
		pthread_mutex_unlock(&args->coder->working_mutix);
	}
	setback_dongles(args, 1);
	pthread_mutex_lock(&args->coder->working_mutix);
	args->coder->finish = 1;
	pthread_mutex_unlock(&args->coder->working_mutix);
	free(args_t);
	return (NULL);
}


void proccess(t_data* data, long long start_time)
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

