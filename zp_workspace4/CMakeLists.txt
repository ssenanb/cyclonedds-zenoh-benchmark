cmake_minimum_required(VERSION 3.16)

project(zp_workspace4 C)

find_package(zenohpico REQUIRED)

add_executable(zPublisherCPU publisher-cpu.c)
target_link_libraries(zPublisherCPU zenohpico::lib)

add_executable(zPublisherLatency publisher-latency.c)
target_link_libraries(zPublisherLatency zenohpico::lib)

add_executable(zPublisherThroughput publisher-throughput.c)
target_link_libraries(zPublisherThroughput zenohpico::lib)

add_executable(zSubscriberCPU subscriber-cpu.c)
target_link_libraries(zSubscriberCPU zenohpico::lib)

add_executable(zSubscriberLatency subscriber-latency.c)
target_link_libraries(zSubscriberLatency zenohpico::lib)

add_executable(zSubscriberThroughput subscriber-throughput.c)
target_link_libraries(zSubscriberThroughput zenohpico::lib)
