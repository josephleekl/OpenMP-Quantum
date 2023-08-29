file(GLOB EXAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    
    target_include_directories(${EXAMPLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include ${NLOPT_INCLUDE_DIRS} ${JANSSON_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})

    # Link with imported targets and other necessary libraries
    target_link_libraries(${EXAMPLE_NAME} PRIVATE
        OMPQ
        pthread
        dl
        util
        rabbitmq::rabbitmq
        ${NLOPT_LINK_LIBRARIES}
        ${Python3_LIBRARIES}
        ${JANSSON_LIBRARIES}
    )
    set_target_properties(${EXAMPLE_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES)

    target_compile_options(${EXAMPLE_NAME} PRIVATE
        -g -O3 -fopenmp -fopenmp-offload-mandatory -fopenmp-targets=x86_64 -Xopenmp-target=x86_64 --offload-arch=x86_64
    )
endforeach()
