
add_library(SpidevMatrix STATIC)

target_link_libraries(SpidevMatrix PUBLIC spidev-lib++)

target_include_directories(SpidevMatrix PUBLIC include)

target_sources(SpidevMatrix PRIVATE
        src/Max7219.cpp
)
