#include "codexion.h"

void proccess(data_t* data)
{

	// time to compile
	usleep(data->time_to_compile);

	//time to debug
	usleep(data->time_to_debug);

	// time to refactor
	usleep(data->time_to_refactor);
}
