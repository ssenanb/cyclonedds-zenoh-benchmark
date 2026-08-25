#include <zenoh-pico.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

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
    z_owned_keyexpr_t key_expr;
    z_owned_publisher_t pub;
    uint32_t connected = 0;
    uint64_t counter = 0;

    z_publisher_options_t opts;
    z_publisher_options_default(&opts);
    opts.is_express = true;
    
    uint8_t value[8]; // Configurable based on payload size

    // UDP Multicast - P2P
    zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, Z_CONFIG_MODE_PEER);
    zp_config_insert(z_loan_mut(config), Z_CONFIG_LISTEN_KEY, "udp/224.0.0.123:7447#iface=lo");
    
    z_open(&session, z_move(config), NULL);

    if(z_keyexpr_from_str_autocanonize(&key_expr, "key/1") < 0)
        return 0;

    if(z_declare_publisher(z_loan(session), &pub, z_loan(key_expr), &opts) != 0) //&OPTS
        return 0;

    while(1) 
    { 
        uint64_t timestamp = get_time_us();
        memcpy(&value, &timestamp, sizeof(timestamp));

        if(z_bytes_from_static_buf(&payload, value, sizeof(value)) != 0)
        {            
            exit(-1);
        }

        z_publisher_put(z_loan(pub), z_move(payload), NULL); 


    }

    z_drop(z_move(session));
    z_drop(z_move(key_expr));
    z_drop(z_move(pub));

    return 0;
}

