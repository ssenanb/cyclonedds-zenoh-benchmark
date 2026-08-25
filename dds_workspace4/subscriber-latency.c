#include "dds/dds.h"
#include "myMessage.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#include "inttypes.h"

#define MAX_SAMPLES 1
#define TOPIC_COUNTER 1
#define READER_COUNTER 1

uint64_t get_time_us(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)((ts.tv_sec * 1000000000ULL + ts.tv_nsec) / 1000.0);
}

int main(int argc, char **argv)
{ 
    dds_entity_t participant;
    dds_entity_t topic[TOPIC_COUNTER];
    dds_entity_t reader[READER_COUNTER];
    dds_return_t rc;
    dds_qos_t *qos;
    char topicName[32];
    myMessage_Msg *msg;
    
    uint64_t latency = 0;
    uint64_t total_latency = 0;
    uint64_t counter = 0;
    uint64_t average_latency = 0;

    void *samples[MAX_SAMPLES]; // the free space created in RAM with myMessage_Msg__alloc()
    dds_sample_info_t infos[MAX_SAMPLES]; // for status control such as valid

    // Create a Participant
    participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0)
        DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

    //qos = dds_create_qos();
    //dds_qset_reliability(qos, DDS_RELIABILITY_BEST_EFFORT, 0);

    for(int i = 0; i < TOPIC_COUNTER; i++)
    {
        snprintf(topicName, sizeof(topicName), "myMessage_Msg_%d", i);

        topic[i] = dds_create_topic (participant, &myMessage_Msg_desc, topicName, NULL, NULL);
        if (topic[i] < 0)
            DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic[i]));

        reader[i] = dds_create_reader (participant, topic[i], NULL, NULL); //QoS
        if (reader[i] < 0)
            DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader[i]));
    }

    printf("---SUBSCRIBER, TOPIC AND READER ARE CREATED---\n");
    fflush (stdout);
    
    // Allocate space equal to the message size and samples[0] is starting point
    samples[0] = myMessage_Msg__alloc ();


    while(1)
    {
       for(int i = 0; i < TOPIC_COUNTER; i++)
        {
            rc = dds_take(reader[i], samples, infos, MAX_SAMPLES, MAX_SAMPLES);
            if (rc < 0)
                DDS_FATAL("dds_take: %s\n", dds_strretcode(-rc));
            else if (rc > 0)
            {
               for(int j = 0; j < rc; j++)
                {
                    if(infos[j].valid_data)
                        msg = (myMessage_Msg*) samples[j]; 

                        counter++;

                        if(counter > 1000)
                        {
                            uint64_t sent_time;
                            memcpy(&sent_time, msg->data, sizeof(uint64_t));

                            uint64_t now = get_time_us();

                            latency = now - sent_time;

                            total_latency += latency;

                            if(counter == 11000)
                            {
                                average_latency = (uint64_t)(total_latency / 10000);
	                            printf("Latency: %" PRIu64 "us\n", average_latency);
                                return 0;
                            }
			            }
    
                } 
            }
        }
    }

    // Release the resources
    myMessage_Msg_free (samples[0], DDS_FREE_ALL);

    rc = dds_delete (participant);
    if (rc != DDS_RETCODE_OK)
        DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

    return EXIT_SUCCESS;

}

