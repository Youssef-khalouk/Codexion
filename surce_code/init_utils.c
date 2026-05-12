/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 18:30:50 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/12 17:04:59 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_data(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_mutex_destroy(&data->coders[i].last_time_mutix);
		pthread_mutex_destroy(&data->dongles[i].mutix_dongle);
		pthread_mutex_destroy(&data->dongles[i].mutix_queue);
		pthread_cond_destroy(&data->dongles[i].scheduler_cond);
		i++;
	}
	pthread_mutex_destroy(&data->stop_mutix);
	pthread_mutex_destroy(&data->print_mutix);
	pthread_cond_destroy(&data->stop_cond);
	free(data->coders);
	free(data->dongles);
	free(data);
}

static void	init_cd(t_coder *coder, t_dongle *dongle, int id, int nb_coders)
{
	coder->id = id;
	coder->last_proccess_time = ms_time();
	coder->right_dongle = NULL;
	coder->left_dongle = NULL;
	coder->finish = 0;
	coder->r_d_id = id;
	coder->l_d_id = (id + 1) % nb_coders;
	pthread_mutex_init(&coder->last_time_mutix, NULL);
	dongle->id = id;
	dongle->set_down_time = 0;
	dongle->queue.front = 0;
	dongle->queue.rear = 0;
	dongle->queue.size = 0;
	pthread_cond_init(&dongle->scheduler_cond, NULL);
	pthread_mutex_init(&dongle->mutix_dongle, NULL);
	pthread_mutex_init(&dongle->mutix_queue, NULL);
}

void	init_coders_and_dongles(t_data *data)
{
	int	i;

	i = 0;
	data->coders = malloc(sizeof(t_coder) * data->number_of_coders);
	data->dongles = malloc(sizeof(t_dongle) * data->number_of_coders);
	pthread_mutex_init(&data->stop_mutix, NULL);
	pthread_mutex_init(&data->print_mutix, NULL);
	pthread_cond_init(&data->stop_cond, NULL);
	data->stop = 0;
	while (i < data->number_of_coders)
	{
		init_cd(&data->coders[i], &data->dongles[i], i, data->number_of_coders);
		i++;
	}
}
