#include "zenoh.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

int main (int argc, char** argv)
{
    z_owned_session_t session;
    z_owned_config_t config; 
    z_owned_bytes_t payload;
    z_owned_keyexpr_t key_expr;
    z_owned_publisher_t pub;

    z_publisher_options_t opts;
    z_publisher_options_default(&opts);
    opts.is_express = true;

    uint8_t value[262144];

    for(size_t i = 0; i < sizeof(value) / sizeof(value[0]); i++)
    {
        value[i] = (uint8_t)i;
    }

    z_config_default(&config);
    
    if(z_open(&session, z_move(config), NULL) != 0 )
    {
        //printf("Failed to open Zenoh session!\n");
        exit(-1); 
    }

    if(z_keyexpr_from_str_autocanonize(&key_expr, "key/1") != 0)
    {
            //printf("Key is not created...\n");
        exit(-1);
    }

    if(z_declare_publisher(z_loan(session), &pub, z_loan(key_expr), &opts) != 0)
    {
            //printf("Publisher is not created...\n");
        exit(-1);
    }


    while(1) 
    { 
        if(z_bytes_from_static_buf(&payload, value, sizeof(value)) != 0)
        {            
            exit(-1);
        }  

        z_publisher_put(z_loan(pub), z_move(payload), NULL); 

    }

    z_drop(z_move(session));

    z_drop(z_move(key_expr));
    z_drop(z_move(pub));


}

