#include "codexion.h"

void compile(proccess_args_t* args, long my_time)
{
    printf("%ld %d is compiling\n",
        my_time, args->coder->id);
	usleep(args->data->time_to_compile * 1000);
}

void debug(proccess_args_t* args, long my_time)
{
    printf("%ld %d is debugging\n",
        my_time, args->coder->id);
	usleep(args->data->time_to_debug * 1000);
}

void refactor(proccess_args_t* args, long my_time)
{
    printf("%ld %d is refactoring\n",
        my_time, args->coder->id);
	usleep(args->data->time_to_refactor * 1000);
}