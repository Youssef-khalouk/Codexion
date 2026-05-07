/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:10:15 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/07 17:17:55 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static char	to_int(int *num, char *str)
{
	long	i;
	int	count;
	
	i = 0;
	count = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i + count] >= '0' && str[i + count] <= '9')
		count++;
	i = atol(str);
	if (!count || count > 10 || 
		i > 2147483647 || i < -2147483648)
	{
		printf("the argument '%s' is not valid.\n", str);
		*num = 0;
		return (0);
	}
	*num = (int)i;
	return (1);
}

static int	check_args(int argc)
{
	if (argc > 9)
	{
		fprintf(stderr, "there is more arguments!\n");
		return (1);
	}
	if (argc < 9)
	{
		fprintf(stderr, "the arguments is not enugh!\n");
		return (1);
	}
	return (0);
}

t_data		*parse_args(int argc, char **argv)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	data->error = 0;
	if (check_args(argc))
	{
		data->error = 1;
		data->number_of_coders = 0;
		return (data);
	}
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
	printf("the schedular value '%s' is not valid!.\n", argv[8]);
	data->error = 1;
	return (data);
}
