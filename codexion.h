#define CODEXION_H
#ifndef CODEXION_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct coders_paramters_t
{
	int time_to_burnout;
	int time_to_compile;
	int time_to_debug;
	int time_to_refactor;
}	coders_paramters_t;

struct data_t
{
	int number_of_coders;
	int time_to_burnout;
	int time_to_compile;
	int time_to_debug;
	int time_to_refactor;
	int number_of_compiles_required;
	int dongle_cooldown;
	int scheduler;
	int error;
}	data_t;

data_t*	parse_args(char **argv);

#endif
