#include "zenoh-pico.h"
#include "stdio.h"
#include "inttypes.h"
#include "time.h"

uint64_t get_time_us(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)((ts.tv_sec * 1000000000ULL + ts.tv_nsec) / 1000.0);
}



 void data_handler(z_loaned_sample_t *sample, void *arg)
{

    static uint64_t counter = 0;
    static uint64_t startTime;
    static uint64_t pkg_counter = 0;

    counter++;

    if(counter == 10001)
    {
        startTime = get_time_us();
    }


    if(counter > 10000 && counter <= 110000)
    {
        pkg_counter += z_bytes_len(z_sample_payload(sample));


        if(counter == 110000)
        {
            uint64_t endTime = get_time_us();
            double elapsed_sec = (double)(endTime - startTime) / 1000000.0;
            double mb_per_sec = (double)pkg_counter / (elapsed_sec * 1024.0 * 1024.0);

            printf("Throughput: %.3f \n", mb_per_sec);

        }

    }
}




int main (int argc, char** argv)
{
    z_owned_config_t config; 
    z_config_default(&config);

    zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, Z_CONFIG_MODE_PEER);
    zp_config_insert(z_loan_mut(config), Z_CONFIG_LISTEN_KEY, "udp/224.0.0.123:7447#iface=lo");

    z_owned_session_t session;  
    z_open(&session, z_move(config), NULL);

    z_owned_keyexpr_t key_expr;
    if(z_keyexpr_from_str_autocanonize(&key_expr, "key/*") < 0)
        return 0;

    z_owned_closure_sample_t callback;
    z_closure_sample(&callback, data_handler, NULL, NULL);
    
    z_owned_subscriber_t sub;
    if(z_declare_subscriber(z_loan(session), &sub, z_loan(key_expr), z_move(callback), NULL) < 0)
        return 0;
    
    char c = 0;
    while (c != 'q') {
        c = fgetc(stdin);
    }

    z_drop(z_move(session));
    z_drop(z_move(key_expr));
    z_drop(z_move(sub));

}