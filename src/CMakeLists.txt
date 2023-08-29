add_library(OMPQ SHARED
    omp_q.c
    simulate_lrz.c
    simulate_qiskit.c
)

target_compile_options(OMPQ PRIVATE
    -g -O3 -fPIC
)

target_include_directories(OMPQ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${JANSSON_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})

# Link with imported targets
target_link_libraries(OMPQ PRIVATE
    rabbitmq::rabbitmq
    ${Python3_LIBRARIES}
    ${JANSSON_LIBRARIES}
)

