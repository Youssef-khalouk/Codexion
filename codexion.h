#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct usb_dongle_t
{
	pthread_mutex_t	dongle;
	int		dongle_id;
}	usb_dongle_t;

typedef struct coder_t
{
	int		id;
	long		last_proccess_time;
	usb_dongle_t* 	right_dongle;
	usb_dongle_t* 	left_dongle;
	pthread_t* 	thread_id;
}	coder_t;

typedef struct data_t
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	char*		scheduler;
	int		error;
	usb_dongle_t*	dongles;
	coder_t*	coders;
}	data_t;

data_t*	parse_args(int argc, char **argv);

void proccess(data_t* data);

#endif
