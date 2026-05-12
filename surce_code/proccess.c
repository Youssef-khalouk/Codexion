/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proccess.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:39 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/12 16:36:16 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	start_work(t_args *args)
{
	pthread_mutex_lock(&args->coder->working_mutix);
	args->coder->working = 1;
	pthread_mutex_unlock(&args->coder->working_mutix);
	if (!compile(args))
		return (0);
	setback_dongles(args);
	pthread_mutex_lock(&args->coder->working_mutix);
	args->coder->last_proccess_time = ms_time();
	args->coder->working = 0;
	pthread_mutex_unlock(&args->coder->working_mutix);
	if (!debug(args))
		return (0);
	if (!refactor(args))
		return (0);
	return (1);
}

static void	*coder_proccess(void *args_t)
{
	int		compiled_times;
	t_args	*args;

	args = (t_args *)args_t;
	compiled_times = 0;
	args->start_time = ms_time();
	while (compiled_times < args->data->number_of_compiles_required)
	{
		if (!request_dongles(args, strcmp(args->data->scheduler, "fifo")))
			break ;
		if (!start_work(args))
			break ;
		compiled_times++;
	}
	setback_dongles(args);
	pthread_mutex_lock(&args->coder->working_mutix);
	args->coder->finish = 1;
	pthread_mutex_unlock(&args->coder->working_mutix);
	return (free(args_t), NULL);
}

void	proccess_data(t_data *data, long long start_time)
{
	int		i;
	t_args	*args;

	i = 0;
	while (i < data->number_of_coders)
	{
		args = malloc(sizeof(t_args));
		args->coder = &data->coders[i];
		args->data = data;
		args->start_time = start_time;
		pthread_create(&data->coders[i].thread_id, NULL,
			coder_proccess, (void *)args);
		i++;
	}
	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_join(data->coders[i].thread_id, NULL);
		i++;
	}
}
