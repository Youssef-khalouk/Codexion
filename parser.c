#include "codexion.h"

static char	to_int(int *num, char *str)
{
	int	i;
	
	i = 0;
	while (str[i])
	{
		if ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
			continue;
		if (str[i] < '0' || str[i] > '9')
		{
			printf("the argument '%s' is not valid.\n", str);
			return (0);
		}
		break;
		i++;
	}
	*num = atoi(str);
	return (1);
}


data_t*	parse_args(char **argv)
{
	int num;

	data_t* data = malloc(sizeof(data_t));
	data->error = 0;
	if (!to_int(&num, argv[1]))
		data->error = 1;
	if (!to_int(&data->number_of_coders, argv[1]))
		data->error = 1;
	if (!to_int(&data->time_to_burnout, argv[2]))
		data->error = 1;
	if (!to_int(&data->time_to_compile, argv[3]))
		data->error = 1;
	if (!to_int(&data->time_to_debug, argv[4]))
		data->error = 1;
	if (!to_int(&data->time_to_refactor, argv[5]))
		data->error = 1;
	if (!to_int(&data->number_of_compiles_required, argv[6]))
		data->error = 1;
	if (!to_int(&data->dongle_cooldown, argv[7]))
		data->error = 1;
	data->scheduler = argv[8];
	if (strcmp(argv[8], "edf") == 0)
		return (data);
	if (strcmp(argv[8], "fifo") == 0)
		return (data);
	printf("the schedular value '%s' is not valid it should be 'fifo' or 'edf'!.\n", argv[8]);
	data->error = 1;
	return (data);
}
