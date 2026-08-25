#include "zenoh.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define KEY_COUNTER 1
#define PUB_COUNTER 1


int main (int argc, char** argv)
{
    z_owned_session_t session;
    z_owned_config_t config; 
    z_owned_bytes_t payload;
    z_owned_keyexpr_t key_expr[KEY_COUNTER];
    z_owned_publisher_t pub[PUB_COUNTER];
    char keyName[32];

    z_publisher_options_t opts;
    z_publisher_options_default(&opts);
    opts.is_express = true;

    uint8_t value[8] = {1}; //Configurable based on payload size

    z_config_default(&config);
    
    if(z_open(&session, z_move(config), NULL) != 0 )
    {
        //printf("Failed to open Zenoh session!\n");
        exit(-1); 
    }
    

    for(int i = 0; i < KEY_COUNTER; i++)
    {
        snprintf(keyName, sizeof(keyName), "key/%d", i);

        if(z_keyexpr_from_str_autocanonize(&key_expr[i], keyName) != 0)
        {
            //printf("Key is not created...\n");
            exit(-1);
        }

        if(z_declare_publisher(z_loan(session), &pub[i], z_loan(key_expr[i]), &opts) != 0)
        {
            //printf("Publisher is not created...\n");
            exit(-1);
        }

    }


    while(1) 
    { 
        for(int i = 0; i < PUB_COUNTER; i++)
        {   

            if(z_bytes_from_static_buf(&payload, value, sizeof(value)) != 0)
            {            
                exit(-1);
            }

            z_publisher_put(z_loan(pub[i]), z_move(payload), NULL); 
        }
    }

    z_drop(z_move(session));

    for(int i = 0; i < KEY_COUNTER; i++)
    {
        z_drop(z_move(key_expr[i]));
        z_drop(z_move(pub[i]));
    }


}

