/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_functions.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:32 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/07 17:19:38 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	queue_has(t_queue *q, int id)
{
	int	i;

	i = 0;
	while (i < q->size)
	{
		if (q->buffer[i] == id)
			return (1);
		i++;
	}
	return (0);
}

int	push_back(t_queue *queue, int value)
{
	int	index;

	index = queue->rear;
	queue->buffer[index] = value;
	queue->rear++;
	queue->size++;
	return (index);
}

void	push_back_if_missing(t_queue *q, int id)
{
	if (!queue_has(q, id) && q->push_later != id)
		push_back(q, id);
}

int	pop_front(t_queue *queue, int coder_finished)
{
	int	i;
	int	value;

	if (queue->size == 0)
		return (-1);
	i = 0;
	value = queue->buffer[queue->front];
	while (i < queue->size - 1)
	{
		queue->buffer[i] = queue->buffer[i + 1];
		i++;
	}
	queue->size--;
	queue->rear--;
	if (coder_finished)
		queue->use_push_later = 0;
	if (!queue->use_push_later && queue->push_later != -1)
	{
		push_back(queue, queue->push_later);
		queue->push_later = -1;
	}
	return (value);
}

void	heap_deadline(t_queue *heap_queue, t_args *args)
{
	t_coder		*coder1;
	t_coder		*coder2;
	int			id;

	if (heap_queue->size < 2)
		return ;
	coder1 = &args->data->coders[heap_queue->buffer[0]];
	coder2 = &args->data->coders[heap_queue->buffer[1]];
	if (coder1->last_proccess_time > coder2->last_proccess_time)
	{
		id = heap_queue->buffer[0];
		heap_queue->buffer[0] = heap_queue->buffer[1];
		heap_queue->buffer[1] = id;
	}
}
