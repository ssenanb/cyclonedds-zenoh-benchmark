# cyclonedds-zenoh-benchmark

# Description

This project compares the performance of Cyclone DDS, Zenoh and Zenoh-Pico middleware across two different scenarios.

* __Scenario 1:__ The Effect Of Increasing Topic Count On Performance
  Using default QoS settings, the payload is fixed at 8 B. The topic count is increased from 50 to 5,000.

* __Scenario 2:__ The Effect Of Increasing Payload Size On Performance
  Using default QoS settings, the topic count is fixed  at 1. Payload size is increased from 8 B to 256 KB.

# The Load of CPU Analyze 

GIPS, IPC, cache misses and context switches were measured using Linux perf stat. The publisher and subscriber were pinned to separate cores using taskset for stability and core isolation. After a 5-seconds warm-up, tests were repeated 5 times and the results were averaged.

# Latency Analyze

On the publisher side, a timestamp was added to each sent message. On the subscriber side, when the sent message was received, the difference was calculated with the arrival time. After a 10,000-messages warm-up, the accumulated latency time was summed across 100,000 messages and it was divided by the number of messages.The publisher and subscriber were pinned to separate cores using taskset for stability and core isolation. The tests were repeated 5 times and the results were averaged.

# Throughput Analyze

On the subscriber side, the payload size of received messages was accumulated in a variable across 100,000 messages after a 10,000-messages warm-up. The variable was divided by the completion time of the 100,000 messages. The tests were repeated 5 times and the results were averaged.












