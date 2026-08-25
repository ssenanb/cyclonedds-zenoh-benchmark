#include <zenoh-pico.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "inttypes.h"

#define COUNTER 5000

// 50 100 200 500 1000 1500 2000 2500 3000 3500 4000 4500 5000

uint64_t get_time_us(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

   return (uint64_t)((ts.tv_sec * 1000000000ULL + ts.tv_nsec) / 1000.0);
}

int main (int argc, char** argv)
{
    z_owned_config_t config; 
    z_config_default(&config);

    z_owned_session_t session;
    z_owned_bytes_t payload;
    z_owned_keyexpr_t key_expr[COUNTER];
    z_owned_publisher_t pub[COUNTER];
    char keyName[32];
    uint32_t connected = 0;

    z_publisher_options_t opts;
    z_publisher_options_default(&opts);
    opts.is_express = true;
    
    uint8_t value[8];

    zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, Z_CONFIG_MODE_PEER);
    zp_config_insert(z_loan_mut(config), Z_CONFIG_LISTEN_KEY, "udp/224.0.0.123:7447#iface=lo");

    if(z_open(&session, z_move(config), NULL) < 0)
        return 0;

    for(int i = 0; i < COUNTER; i++)
    {
        snprintf(keyName, sizeof(keyName), "key/%d", i);

        if(z_keyexpr_from_str_autocanonize(&key_expr[i], keyName) < 0)
            return 0;

        if(z_declare_publisher(z_loan(session), &pub[i], z_loan(key_expr[i]), NULL) != 0) //&OPTS
            return 0;
    }

    while(1) 
    { 
        for(int i = 0; i < COUNTER; i++)
        {   
            uint64_t timestamp = get_time_us();
            memcpy(&value, &timestamp, sizeof(value));

            if(z_bytes_from_static_buf(&payload, value, sizeof(value)) < 0)
                exit(-1);

            z_publisher_put(z_loan(pub[i]), z_move(payload), NULL);  
            
            //z_sleep_ms(1);
        }
    }

    z_drop(z_move(session));

    for(int i = 0; i < COUNTER; i++)
    {
        z_drop(z_move(key_expr[i]));
        z_drop(z_move(pub[i]));
    }

    return 0;
}

