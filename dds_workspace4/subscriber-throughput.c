#include "dds/dds.h"
#include "myMessage.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#define MAX_SAMPLES 1


int64_t get_time_us(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)((ts.tv_sec * 1000000000ULL + ts.tv_nsec) / 1000.0);
}

int main(int argc, char **argv)
{ 
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t reader;
    dds_return_t rc;
    dds_qos_t *qos;
    myMessage_Msg *msg;
    uint64_t counter = 0;
    uint64_t pkg_counter = 0;
    uint64_t startTime;
    void *samples[MAX_SAMPLES]; // the free space created in RAM with myMessage_Msg__alloc()
    dds_sample_info_t infos[MAX_SAMPLES]; // for status control such as valid

    // Create a Participant
    participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0)
        DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

    //qos = dds_create_qos();
    //dds_qset_reliability(qos, DDS_RELIABILITY_BEST_EFFORT, 0);


    topic = dds_create_topic (participant, &myMessage_Msg_desc, "myMessage_Msg_", NULL, NULL);
    if (topic < 0)
            DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

    reader = dds_create_reader (participant, topic, NULL, NULL); //QoS
    if (reader < 0)
            DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader));

    printf("---SUBSCRIBER, TOPIC AND READER ARE CREATED---\n");
    fflush (stdout);
    
    // Allocate space equal to the message size and samples[0] is starting point
    samples[0] = myMessage_Msg__alloc ();

    while(1)
    {
        rc = dds_take(reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
        if (rc < 0)
            DDS_FATAL("dds_take: %s\n", dds_strretcode(-rc));
        else if (rc > 0)
        {
            for(int j = 0; j < rc; j++)
            {
                if(infos[j].valid_data)
                {
                    msg = (myMessage_Msg*) samples[j]; 

                    counter++;

                    if(counter == 10001)
                    {
                        startTime = get_time_us();
                    }
                           

                    if(counter > 10000 && counter <= 110000)
                    {
                        pkg_counter += sizeof(msg->data) / sizeof(msg->data[0]);

                        if(counter == 110000)
                        {
                            uint64_t endTime = get_time_us();

                            double elapsed_sec = (double)(endTime - startTime) / 1000000.0;

                            double mb_per_sec  = (double)pkg_counter / (elapsed_sec * 1024.0 * 1024.0);

                            printf("Throughput: %.3f \n", mb_per_sec);

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

