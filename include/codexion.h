/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:32:43 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 16:33:30 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

// ================== includes ==================

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

// ================== Data Structurs ==================

typedef struct t_coder	t_coder;

typedef struct t_heap_entry
{
	long long			priority;
	int					compile_count;
	int					coder_id;
}						t_heap_entry;

typedef struct t_heap
{
	t_heap_entry		*data;
	int					size;
	int					capacity;
}						t_heap;

typedef struct t_dongle
{
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	int					is_available;
	long long			released_at;
	t_heap				wait_queue;
}						t_dongle;

typedef struct t_sim
{
	int					num_coders;
	long long			time_to_burnout;
	long long			time_to_compile;
	long long			time_to_debug;
	long long			time_to_refactor;
	int					num_compiles_required;
	long long			dongle_cooldown;
	int					use_edf;
	t_coder				*coders;
	t_dongle			*dongles;
	pthread_mutex_t		log_mutex;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		start_mutex;
	int					simulation_over;
	long long			start_time;
	long long			fifo_counter;
	pthread_mutex_t		fifo_mutex;
	pthread_t			monitor_thread;
}						t_sim;

typedef struct t_coder
{
	int					id;
	pthread_t			thread;
	long long			last_compile_start;
	int					compile_count;
	pthread_mutex_t		coder_mutex;
	t_sim				*sim;
}						t_coder;

// ================== utils ==================

long long				get_time_ms(void);
void					ft_msleep(long long ms, t_sim *sim);
int						is_sim_over(t_sim *sim);
int						ft_atoi(const char *str);
int						ft_strcmp(const char *s1, const char *s2);
int						is_valid_number(const char *str);

// ================== scheduler ==================

int						heap_init(t_heap *h, int capacity);
void					sift_up(t_heap *h, int i);
void					sift_down(t_heap *h, int i);
int						heap_push(t_heap *h, long long priority,
							int compile_count, int coder_id);
t_heap_entry			heap_pop(t_heap *h);
int						heap_peek_id(t_heap *h);

// ================== log ==================

void					log_event(t_sim *sim, int coder_id, char *event);

// ================== init ==================

int						init_sim(t_sim *sim, int argc, char **argv);

// ================== dongle ==================

int						dongle_acquire(t_dongle *dongle, long long priority,
							t_coder *coder);
void					dongle_release(t_dongle *dongle);

// ================== coder ==================

void					*coder_routine(void *arg);

// ================== monitor ==================

void					*monitor_routine(void *arg);

#endif