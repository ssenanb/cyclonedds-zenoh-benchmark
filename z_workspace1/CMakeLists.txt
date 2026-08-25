cmake_minimum_required(VERSION 3.16)

project(z_workspace1 C)

find_package(zenohc REQUIRED)

add_executable(zPublisherCPU publisher-cpu.c)
target_link_libraries(zPublisherCPU zenohc::lib)

add_executable(zPublisherLatency publisher-latency.c)
target_link_libraries(zPublisherLatency zenohc::lib)

add_executable(zPublisherThroughput publisher-throughput.c)
target_link_libraries(zPublisherThroughput zenohc::lib)

add_executable(zSubscriberCPU subscriber-cpu.c)
target_link_libraries(zSubscriberCPU zenohc::lib)

add_executable(zSubscriberLatency subscriber-latency.c)
target_link_libraries(zSubscriberLatency zenohc::lib)

add_executable(zSubscriberThroughput subscriber-throughput.c)
target_link_libraries(zSubscriberThroughput zenohc::lib)

add_executable(zSubscriberThroughputv1 subscriber-throughput-v1.c)
target_link_libraries(zSubscriberThroughputv1 zenohc::lib)
