#include "zenoh.h"
#include "stdio.h"
#include "string.h"


void data_handler(z_loaned_sample_t *sample, void *arg)
{   
    z_owned_slice_t output_data;
    z_bytes_to_slice(z_sample_payload(sample), &output_data);

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
