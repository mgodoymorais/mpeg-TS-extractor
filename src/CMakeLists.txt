include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

ExternalProject_Add(cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

set(SOURCES
    main.cpp
    PidWriter.cpp
    TSFile.cpp
    TSPacket.cpp
)

add_executable(ts_extractor ${SOURCES})
add_dependencies(ts_extractor cxxopts)
#target_link_libraries(ts_extractor)
