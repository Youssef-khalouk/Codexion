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

typedef struct s_queue
{
	int buffer[256];
	int front;
	int rear;
	int size;
	int push_later;
	int	use_push_later;
} t_queue;

typedef struct usb_dongle_t
{
	int				id;
	pthread_mutex_t	mutix_dongle;
	pthread_cond_t	scheduler_cond;
	long long		set_down_time;
	pthread_mutex_t	mutix_queue;
	t_queue			queue;
}	usb_dongle_t;

typedef struct coder_t
{
	int				id;
	long long		last_proccess_time;
	int				r_d_id;
	int				l_d_id;
	usb_dongle_t* 	right_dongle;
	usb_dongle_t* 	left_dongle;
	pthread_t		thread_id;
	int				working;
	int				finish;
	pthread_mutex_t	working_mutix;
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
	long long		start_time;
}	data_t;

typedef struct proccess_args_t
{
	data_t*		data;
	coder_t*	coder;
	long long	start_time;
}	proccess_args_t;

data_t*	parse_args(int argc, char **argv);

void proccess(data_t* data, long long start_time);

int compile(proccess_args_t* args);

int debug(proccess_args_t* args);

int refactor(proccess_args_t* args);

long long ms_time(void);

#endif
