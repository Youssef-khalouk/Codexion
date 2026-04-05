
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
			return (0);
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
	if (!to_int(&num, argv[0][1]))
	{
	       	printf("the argument '%s' is not valid.", argv[0][1]);
		data->error = 1;
	}
	
}
