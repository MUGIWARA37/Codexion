/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:32:43 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/05 12:41:36 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
#define CODEXION_H

// ================== includes ==================

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

// ================== Data Structurs ==================

typedef struct t_coder t_coder;

typedef struct t_heap_entry
{
    long long priority;
    int coder_id;
}             t_heap_entry;

typedef struct t_heap
{
    t_heap_entry* data;
    int           size;
    int           capacity;
}              t_heap;

typedef struct t_dongle
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int is_available;
    long long released_at;
    t_heap  wait_queue;
}              t_dongle;

typedef struct  t_sim
{
    int num_coders;
    long long  time_to_burnout;
    long long  time_to_compile;
    long long  time_to_debug;
    long long  time_to_refactor;
    int        num_compiles_required;
    long long  dongle_cooldown;
    int        use_edf;
    t_coder*   coders;
    t_dongle*  dongles;
    pthread_mutex_t log_mutex;
    pthread_mutex_t stop_mutex;
    int             simulation_over;
    long long  start_time;
    long long fifo_counter;
    pthread_mutex_t fifo_mutex;
    pthread_t       monitor_thread;
}               t_sim;



typedef struct t_coder
{
    int        id;
    pthread_t   thread;
    long long  last_compile_start;
    int compile_count;
    t_sim*  sim;
}              t_coder;

// ================== utils ==================

long long   get_time_ms(void);
void        ft_msleep(long long ms, t_sim *sim);
int         is_sim_over(t_sim *sim);


#endif