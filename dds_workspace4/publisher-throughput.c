#include "dds/dds.h"
#include "myMessage.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

void fillMessage(myMessage_Msg *msg);

int main(int argc, char **argv)
{
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t writer;
    dds_return_t rc;
    uint32_t status = 0;
    uint64_t counter = 0;
    myMessage_Msg msg;


    participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0)
        DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

    topic = dds_create_topic (participant, &myMessage_Msg_desc, "myMessage_Msg_", NULL, NULL);
    if (topic < 0)
        DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

    writer = dds_create_writer (participant, topic, NULL, NULL); 
    if (writer < 0)
        DDS_FATAL("dds_create_writer: %s\n", dds_strretcode(-writer));

    printf("---PUBLISHER, TOPIC AND WRITER ARE CREATED---\n");
    fflush (stdout);

    rc = dds_set_status_mask(writer, DDS_PUBLICATION_MATCHED_STATUS);
    if (rc != DDS_RETCODE_OK)
        DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-rc));

    while(!(status & DDS_PUBLICATION_MATCHED_STATUS))
    {
        rc = dds_get_status_changes (writer, &status);
        if (rc != DDS_RETCODE_OK)
            DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-rc));

        dds_sleepfor(DDS_MSECS (20));
    }


    while(1)
    {
     
        fillMessage(&msg);

        rc = dds_write (writer, &msg);
        if (rc != DDS_RETCODE_OK)
            DDS_FATAL("dds_write: %s\n", dds_strretcode(-rc));
            
    }

    rc = dds_delete (participant);
    if (rc != DDS_RETCODE_OK)
        DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

    return EXIT_SUCCESS;

}

void fillMessage(myMessage_Msg *msg)
{
   for (size_t i = 0; i < sizeof(msg->data) / sizeof(msg->data[0]); i++)
    {
        msg->data[i] = (uint8_t)i;
    }
}

