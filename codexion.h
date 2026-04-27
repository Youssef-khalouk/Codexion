#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

typedef struct s_req {
	int		id;
	long	key;	/* arrival time (fifo) or deadline (edf) */
}	t_req;

typedef struct s_pq {
	t_req	data[256];
	int		n;
}	t_queue;

typedef struct usb_dongle_t
{
	int				id;
	pthread_mutex_t	dongle;
	pthread_cond_t	scheduler_cond;
	long long		set_down_time;
	t_queue			queue;
}	usb_dongle_t;

typedef struct coder_t
{
	int				id;
	long long		last_proccess_time;
	usb_dongle_t* 	right_dongle;
	usb_dongle_t* 	left_dongle;
	pthread_t*		thread_id;
}	coder_t;

typedef struct data_t
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	char*			scheduler;
	int				error;
	volatile int	stop;
	usb_dongle_t*	dongles;
	coder_t*		coders;
	pthread_cond_t	stop_condation;
	pthread_mutex_t	stop_mutix;
}	data_t;

typedef struct proccess_args_t
{
	data_t*		data;
	coder_t*	coder;
}	proccess_args_t;

data_t*	parse_args(int argc, char **argv);

void proccess(data_t* data);

int compile(proccess_args_t* args, long my_time);

int debug(proccess_args_t* args, long my_time);

int refactor(proccess_args_t* args, long my_time);

long long ms_time(void);

#endif
