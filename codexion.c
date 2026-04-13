
#include "codexion.h"

static void	free_data(data_t* data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		free(data->coders[i].thread_id);
		i++;
	}
	free(data->coders);
	free(data->dongles);
	free(data);	
}

static void	init_coders_and_dongles(data_t* data)
{
	int	i;
	
	i = 0;
	data->coders = malloc(sizeof(coder_t) * data->number_of_coders);
	data->dongles = malloc(sizeof(usb_dongle_t) * data->number_of_coders);
	while (i < data->number_of_coders)
	{
		data->coders[i].id = i;
		data->coders[i].last_proccess_time = 0;
		data->dongles[i].id = i;
		pthread_mutex_init(&data->dongles[i].dongle, NULL);
		i++;
	}
}

int	main(int argc, char **argv)
{
	data_t*	data;

	data = parse_args(argc, argv);
	init_coders_and_dongles(data);
	if (data->error)
		return (free_data(data), 1);

	proccess(data);

	printf("Back in main\n");

	return (free_data(data), 0);
}
