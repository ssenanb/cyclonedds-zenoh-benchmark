cmake_minimum_required(VERSION 3.5)
project(dds_workspace4 LANGUAGES C)

if(NOT TARGET CycloneDDS::ddsc)
  # Find the CycloneDDS package.
  find_package(CycloneDDS REQUIRED)
endif()

idlc_generate(TARGET myMessage_lib FILES "myMessage.idl")

add_executable(ddsPublisherCPU publisher-cpu.c)
add_executable(ddsSubscriberCPU subscriber-cpu.c)

add_executable(ddsPublisherLatency publisher-latency.c)
add_executable(ddsSubscriberLatency subscriber-latency.c)
add_executable(ddsPublisherThroughput publisher-throughput.c)
add_executable(ddsSubscriberThroughput subscriber-throughput.c)


target_link_libraries(ddsPublisherCPU myMessage_lib CycloneDDS::ddsc)
target_link_libraries(ddsSubscriberCPU myMessage_lib CycloneDDS::ddsc)

target_link_libraries(ddsPublisherLatency myMessage_lib CycloneDDS::ddsc)
target_link_libraries(ddsSubscriberLatency myMessage_lib CycloneDDS::ddsc)
target_link_libraries(ddsPublisherThroughput myMessage_lib CycloneDDS::ddsc)
target_link_libraries(ddsSubscriberThroughput myMessage_lib CycloneDDS::ddsc)
