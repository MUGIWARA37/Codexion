/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhlou <rhlou@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 13:14:03 by rhlou             #+#    #+#             */
/*   Updated: 2026/07/10 13:50:44 by rhlou            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void *coder_routine(void *arg)
{
    t_dongle *first;
    t_dongle *second;   
    long long priority;              
    int      left;                  
    int      right; 
    t_coder *coder;
    t_sim *sim;
                                
    coder = (t_coder *)arg;
    sim   = coder->sim;
    left  = coder->id - 1;
    right = coder->id % sim->num_coders;
    if (coder->id % 2 == 0)
    {
        first  = &sim->dongles[left];
        second = &sim->dongles[right];
    }                               
    else
    {                         
        first  = &sim->dongles[right];
        second = &sim->dongles[left];  
    }  
    while (!is_sim_over(sim))
    {
        if (sim->use_edf)
            priority = coder->last_compile_start + sim->time_to_burnout;
        else
        {
            pthread_mutex_lock(&sim->fifo_mutex);
            priority = sim->fifo_counter++;
            pthread_mutex_unlock(&sim->fifo_mutex);
        }
        
        dongle_acquire(first, priority, coder->id, sim);
        dongle_acquire(second, priority, coder->id, sim);

        if (is_sim_over(sim))
        {
            dongle_release(second);
            dongle_release(first);
            break ;
        }

        coder->last_compile_start = get_time_ms();
        log_event(sim, coder->id, "is compiling");
        ft_msleep(sim->time_to_compile, sim);

        coder->compile_count++;
        dongle_release(second);
        dongle_release(first);

        if (sim->num_compiles_required != -1
        && coder->compile_count >= sim->num_compiles_required)
        break ;
  
        // debug
        log_event(sim, coder->id, "is debugging");
        ft_msleep(sim->time_to_debug, sim);
            
        // refactor
        log_event(sim, coder->id, "is refactoring");
        ft_msleep(sim->time_to_refactor, sim);

        
        
    }
    return (NULL);
}
