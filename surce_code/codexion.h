/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:23 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/08 00:11:50 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <time.h>
# include <sys/time.h>

typedef struct s_queue
{
	int	buffer[256];
	int	front;
	int	rear;
	int	size;
}	t_queue;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutix_dongle;
	pthread_cond_t	scheduler_cond;
	long long		set_down_time;
	pthread_mutex_t	mutix_queue;
	t_queue			queue;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	long long		last_proccess_time;
	int				r_d_id;
	int				l_d_id;
	t_dongle		*right_dongle;
	t_dongle		*left_dongle;
	pthread_t		thread_id;
	int				working;
	int				finish;
	pthread_mutex_t	working_mutix;
}	t_coder;

typedef struct s_data
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	char			*scheduler;
	int				error;
	volatile int	stop;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_cond_t	stop_condation;
	pthread_mutex_t	stop_mutix;
	long long		start_time;
}	t_data;

typedef struct s_args
{
	t_data		*data;
	t_coder		*coder;
	long long	start_time;
}	t_args;

t_data		*parse_args(int argc, char **argv);

void		proccess_data(t_data *data, long long start_time);

int			compile(t_args *args);

int			debug(t_args *args);

int			refactor(t_args *args);

long long	ms_time(void);

int			queue_has(t_queue *q, int id);

int			push_back(t_queue *queue, int value);

void		push_back_if_missing(t_queue *q, int id);

int			pop_front(t_queue *queue);

void		heap_deadline(t_queue *heap_queue, t_args *args);

void		setback_dongles(t_args* args);

int			take_dongle_when_ready(t_args* args, t_dongle* dongle, char r_l);

int			simulation_stoped(t_args * args);

int			request_right_d(t_args* args, int edf);

int			request_left_d(t_args* args, int edf);

#endif