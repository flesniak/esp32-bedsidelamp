if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include_directories("libartnet/artnet")
    add_compile_definitions(LWIP)
    add_library(libartnet $<LIST:TRANSFORM,artnet.c misc.c network.c receive.c tod.c transmit.c,PREPEND,libartnet/artnet/>)
endif()

idf_component_register(SRCS "wifi.c" "main.c"
                       INCLUDE_DIRS ".")
