#include "codexion.h"

void compile(proccess_args_t* args, long my_time)
{
    printf("time:%ld coder:%d is compiling\n",
        my_time, args->coder->id);
	usleep(args->data->time_to_compile * 1000);
}

void debug(proccess_args_t* args, long my_time)
{
    printf("time:%ld coder:%d is debugging\n",
        my_time, args->coder->id);
	usleep(args->data->time_to_debug * 1000);
}

void refactor(proccess_args_t* args, long my_time)
{
    printf("time:%ld coder:%d is refactoring\n\n",
        my_time, args->coder->id);
	usleep(args->data->time_to_refactor * 1000);
}