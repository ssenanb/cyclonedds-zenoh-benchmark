#include "zenoh-pico.h"
#include "stdio.h"
#include "inttypes.h"

void data_handler(z_loaned_sample_t *sample, void *arg)
{   
    z_owned_slice_t output_data;
    z_bytes_to_slice(z_sample_payload(sample), &output_data);

    z_drop(z_move(output_data));
}

int main (int argc, char** argv)
{
    z_owned_config_t config; 
    z_config_default(&config);

    // UDP Multicast - P2P
    zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, Z_CONFIG_MODE_PEER);
    zp_config_insert(z_loan_mut(config), Z_CONFIG_LISTEN_KEY, "udp/224.0.0.123:7447#iface=lo");
    
    z_owned_session_t session;  
    if(z_open(&session, z_move(config), NULL) < 0)
        return 0;

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
