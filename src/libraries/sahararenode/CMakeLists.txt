cmake_minimum_required (VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

project (libdevsrv      VERSION 0.9)
project (devsrv         VERSION 0.9)

include_directories( /usr/local/include )
add_library(    libdevsrv
                channel.cc
                config.cc
                dataset.cc
                device.cc
                packet.cc
                buffer.cc
                headers.cc
                payload.cc                
                log.cc
                sdp.cc
                socket_base.cc
                socket_raw.cc
                socket_udp.cc
                channel_ds2fpga.cc
                channel_ds2ds.cc
                sdp_packet.cc
                sdp_packet_handler.cc
                snp_packet_handler.cc
                snp.cc
                global.cc
                utils.cc
                )

add_executable( devsrv
                main.cc
                )       

add_executable( sdptest 
                tests/sdp_test.cc
                )
add_executable( snptest 
                tests/snp_test.cc
                )
# Add pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_include_directories(devsrv PRIVATE /usr/local/include) 
target_link_libraries(devsrv libdevsrv pthread)
target_link_libraries(sdptest libdevsrv pthread)
target_link_libraries(snptest libdevsrv pthread)


