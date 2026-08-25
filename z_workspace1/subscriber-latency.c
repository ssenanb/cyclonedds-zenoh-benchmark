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

void data_handler(z_loaned_sample_t *sample, void *arg)
{   
    static uint64_t counter = 0;
    static uint64_t latency = 0;
    static uint64_t total_latency = 0;
    uint64_t average_latency = 0;

    z_owned_slice_t output_data;
    z_bytes_to_slice(z_sample_payload(sample), &output_data);

    counter++;

    uint64_t sent_time;
    memcpy(&sent_time, (z_slice_data(z_loan(output_data))), sizeof(uint64_t));

    if(counter > 10000)
    {
        uint64_t now = get_time_us();

        latency = now - sent_time;

        total_latency += latency;

        if(counter == 110000)
        {
            average_latency = (uint64_t)(total_latency / 100000);
            printf("Latency: %" PRIu64 "us\n", average_latency);
        }
    }

    z_drop(z_move(output_data));
}

int main(int argc, char** argv)
{ 
    z_owned_session_t session;
    z_owned_config_t config;
    z_config_default(&config);    
    
    if(z_open(&session, z_move(config), NULL) != 0)
    {
       // printf("Failed to open Zenoh session!\n");
        exit(-1);
    }

    z_owned_keyexpr_t key_expr;
    if(z_keyexpr_from_str_autocanonize(&key_expr, "key/*") != 0)
    {
       // printf("Key is not read...\n");
        exit(-1);
    }

    z_owned_closure_sample_t callback;
    z_closure_sample(&callback, data_handler, NULL, NULL);
    
    z_owned_subscriber_t sub;
    if(z_declare_subscriber(z_loan(session), &sub, z_loan(key_expr), z_move(callback), NULL) != 0)
    {
        //printf("Subscriber is not created...\n");
        exit(-1);
    }
    
    char c = 0;
    while (c != 'q') {
        c = fgetc(stdin);
    }

    z_drop(z_move(key_expr));
    z_drop(z_move(sub));
    z_drop(z_move(session));    
    return 0; 
}
