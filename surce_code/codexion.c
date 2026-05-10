/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:43 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/10 15:07:49 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_data		*data;
	pthread_t	monitor_thread;
	long long	start_time;

	start_time = ms_time();
	data = parse_args(argc, argv);
	init_coders_and_dongles(data);
	if (data->error)
		return (free_data(data), 1);
	data->start_time = start_time;
	pthread_create(&monitor_thread, NULL, monitor, (void *)data);
	proccess_data(data, start_time);
	pthread_join(monitor_thread, NULL);
	return (free_data(data), 0);
}
