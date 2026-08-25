#include "zenoh.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
#include "inttypes.h"
#include "stdlib.h"


uint64_t get_time_us(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

   return (uint64_t)((ts.tv_sec * 1000000000ULL + ts.tv_nsec) / 1000.0);
}

int data_handler(z_loaned_sample_t *sample)
{
    static uint64_t counter = 0;
    static uint64_t startTime;
    static uint64_t pkg_counter = 0;

    z_owned_slice_t output_data;
    z_bytes_to_slice(z_sample_payload(sample), &output_data);


    counter++;

    if(counter == 10001)
    {
        startTime = get_time_us();
    }
                           

    if(counter > 10000 && counter <= 110000)
    {
        pkg_counter += 8;

        if(counter == 110000)
        {

            uint64_t endTime = get_time_us();

            double elapsed_sec = (double)(endTime - startTime) / 1000000.0;

            double mb_per_sec  = (double)pkg_counter / (elapsed_sec * 1024.0 * 1024.0);

            printf("Throughput: %.3f \n", mb_per_sec);

        }
    }

    z_drop(z_move(output_data));
}

int main(int argc, char** argv)
{ 
    z_owned_session_t session;
    z_owned_config_t config;
    z_owned_keyexpr_t key_expr;
    z_owned_closure_sample_t closure;
    z_owned_fifo_handler_sample_t handler;
    z_owned_subscriber_t sub;
    z_owned_sample_t sample; 
    
    z_config_default(&config);    
    
    if(z_open(&session, z_move(config), NULL) != 0)
    {
        //printf("Failed to open Zenoh session!\n");
        exit(-1);
    }

    z_fifo_channel_sample_new(&closure, &handler, 1); //?

    if(z_keyexpr_from_str_autocanonize(&key_expr, "key/*") != 0)
    {
            //printf("Key is not read...\n");
            exit(-1);
    }

     if(z_declare_subscriber(z_loan(session), &sub, z_loan(key_expr), z_move(closure), NULL) != 0)
    {
            //printf("Subscriber is not created...\n");
            exit(-1);
    }

    
    while(1)
    {
        if(z_fifo_handler_sample_recv(z_loan(handler), &sample) == Z_OK)
        {
            data_handler(z_loan(sample));
            z_drop(z_move(sample));
        }
    }


    z_drop(z_move(session));
    z_drop(z_move(key_expr));
    z_drop(z_move(sub));

    return 0; 
}

