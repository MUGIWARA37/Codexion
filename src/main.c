/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 14:01:34 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 14:18:01 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int main(int argc, char **argv)
{
    t_sim  sim;
    int i;
    
    if (argc != 9)
    {
        fprintf(stderr, "Usage: ./codexion num_coders time_to_burnout time_to_compile ");
        fprintf(stderr, "time_to_debug time_to_refactor num_compiles dongle_cooldown ");
        fprintf(stderr, "scheduler(fifo|edf)\n");
        return (1);
    }

    if (init_sim(&sim, argc, argv) == -1)
    {
        fprintf(stderr, "Error: invalid arguments\n");
        return (1);
    }

    pthread_create(&sim.monitor_thread, NULL, monitor_routine, &sim);
    
    i = 0;
    while (i < sim.num_coders)
    {
        pthread_create(&sim.coders[i].thread, NULL, coder_routine, &sim.coders[i]);
        i++;
    }
    
    i = 0;
    while (i < sim.num_coders)
    {
        pthread_join(sim.coders[i].thread, NULL);
        i++;
    }

    pthread_join(sim.monitor_thread, NULL);
    free(sim.coders);
    free(sim.dongles);

    return (0);
}