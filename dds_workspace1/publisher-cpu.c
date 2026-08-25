#include "dds/dds.h"
#include "myMessage.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"


//Topic sayısını artırmak, işlemciye daha ağır bir hesaplatma yaptırmaz; 
// işlemciyi daha çok arama yapmaya ve RAM beklemeye zorlar.
//İşlemci işlem yaparken değil, veri beklerken yavaşladığı için ölçümlerinizde bekleme metrikleri 
// (Cache Miss) artarken, verimlilik metrikleri (IPC/IPS) düşer.


// 50 100 200 500 1000 1500 2000 2500 3000 3500 4000 4500 5000
#define TOPIC_COUNTER 5000
#define WRITER_COUNTER 5000

void fillMessage(myMessage_Msg *msg);
uint64_t get_time_us(void);

int main(int argc, char **argv)
{
    dds_entity_t participant;
    dds_entity_t topic[TOPIC_COUNTER];
    dds_entity_t writer[WRITER_COUNTER];
    dds_return_t rc;
    dds_qos_t *qos;
    char topicName[32];
    uint32_t status = 0;
    myMessage_Msg msg;


    // Create a Participant
    participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0)
        DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

    //Create the QoS
    //qos = dds_create_qos();
    //dds_qset_reliability(qos, DDS_RELIABILITY_BEST_EFFORT, 0);

    for(int i = 0; i < TOPIC_COUNTER; i++)
    {
        snprintf(topicName, sizeof(topicName), "myMessage_Msg_%d", i);

        topic[i] = dds_create_topic (participant, &myMessage_Msg_desc, topicName, NULL, NULL);
        if (topic[i] < 0)
            DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic[i]));

        writer[i] = dds_create_writer (participant, topic[i], NULL, NULL); //QoS
        if (writer[i] < 0)
            DDS_FATAL("dds_create_writer: %s\n", dds_strretcode(-writer[i]));
    }

     printf("---PUBLISHER, TOPIC AND WRITER ARE CREATED---\n");
     fflush (stdout);

    for (int j = 0; j < TOPIC_COUNTER; j++)
    {
        rc = dds_set_status_mask(writer[j], DDS_PUBLICATION_MATCHED_STATUS);
        if (rc != DDS_RETCODE_OK)
            DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-rc));

        while(!(status & DDS_PUBLICATION_MATCHED_STATUS))
        {
            rc = dds_get_status_changes (writer[j], &status);
            if (rc != DDS_RETCODE_OK)
                DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-rc));

            dds_sleepfor(DDS_MSECS (20));
        }
    }


    while(1)
    {

        for (int i = 0; i < TOPIC_COUNTER; i++)
        {
            fillMessage(&msg);

            rc = dds_write (writer[i], &msg);
            if (rc != DDS_RETCODE_OK)
                DDS_FATAL("dds_write: %s\n", dds_strretcode(-rc));
        }

    }

    // Relase the resources
    rc = dds_delete (participant);
    if (rc != DDS_RETCODE_OK)
        DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

    return EXIT_SUCCESS;

}

void fillMessage(myMessage_Msg *msg)
{
    for(int i = 0; i < 8; i++)
        msg->data[i] = i;
}

