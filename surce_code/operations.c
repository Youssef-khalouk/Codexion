/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operations.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhalouk <ykhalouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:09:35 by ykhalouk          #+#    #+#             */
/*   Updated: 2026/05/07 20:01:10 by ykhalouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void wait_time(struct timespec *ts, long ms)
{
    clock_gettime(CLOCK_REALTIME, ts);

    ts->tv_sec  += ms / 1000;
    ts->tv_nsec += (ms % 1000) * 1000000;

    if (ts->tv_nsec >= 1000000000)
    {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000;
    }
}

int compile(t_args* args)
{
    long long   curent_time;
    int         stopped;
    int         ret;

    curent_time = ms_time();
    printf("%-6lld %d is compiling\n", curent_time - args->start_time, args->coder->id + 1);
    fflush(stdout);
    
    struct timespec ts;
    wait_time(&ts, args->data->time_to_compile);
    pthread_mutex_lock(&args->data->stop_mutix);
    while (!args->data->stop && (ms_time() - curent_time) < args->data->time_to_compile)
    {
        ret = pthread_cond_timedwait(&args->data->stop_condation, &args->data->stop_mutix, &ts);
        if (ret == 110) // 110 is a timeout -> ETIMEDOUT
            break;
    }
    stopped = args->data->stop;
    pthread_mutex_unlock(&args->data->stop_mutix);
    if (stopped)
        return (0);
    return (1);
}

int debug(t_args* args)
{
    long long   curent_time;
    int         stopped;
    int         ret;

    curent_time = ms_time();
    printf("%-6lld %d is debugging\n", curent_time - args->start_time, args->coder->id + 1);
    fflush(stdout);
    
    struct timespec ts;
    wait_time(&ts, args->data->time_to_debug);
    pthread_mutex_lock(&args->data->stop_mutix);
    while (!args->data->stop && (ms_time() - curent_time) < args->data->time_to_debug)
    {
        ret = pthread_cond_timedwait(&args->data->stop_condation, &args->data->stop_mutix, &ts);
        if (ret == 110) // 110 is a timeout -> ETIMEDOUT
            break;
    }
    stopped = args->data->stop;
    pthread_mutex_unlock(&args->data->stop_mutix);
    if (stopped)
        return (0);
    return (1);
}

int refactor(t_args* args)
{
    long long   curent_time;
    int         stopped;
    int         ret;

    curent_time = ms_time();
    printf("%-6lld %d is refactoring\n", curent_time - args->start_time, args->coder->id + 1);
    fflush(stdout);
    
    struct timespec ts;
    wait_time(&ts, args->data->time_to_refactor);
    pthread_mutex_lock(&args->data->stop_mutix);
    while (!args->data->stop && (ms_time() - curent_time) < args->data->time_to_refactor)
    {
        ret = pthread_cond_timedwait(&args->data->stop_condation, &args->data->stop_mutix, &ts);
        if (ret == 110) // 110 is a timeout -> ETIMEDOUT
            break;
    }
    stopped = args->data->stop;
    pthread_mutex_unlock(&args->data->stop_mutix);
    if (stopped)
        return (0);
    return (1);
}