if(NOT TARGET spidev-lib++)
    FetchContent_Declare(
            spidev
            GIT_REPOSITORY https://github.com/hirshi001/spidev-lib-forked.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(spidev)
endif()
