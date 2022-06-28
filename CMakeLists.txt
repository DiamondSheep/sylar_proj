cmake_minimum_required(VERSION 3.10) 
 
project(sylar) 
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 
set(CMAKE_CXX_FLAGS -g) 

find_package(Boost)
find_package(yaml-cpp)

include_directories(src/)
set(LIB_SRC
    src/log.cpp
    src/utils.cpp 
    src/config.cpp 
    )
add_library(sylar SHARED ${LIB_SRC})
target_include_directories(sylar PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(sylar PUBLIC ${Boost_LIBRARIES})
target_include_directories(sylar PUBLIC ${YAML_CPP_INCLUDE_DIRS})
target_link_libraries(sylar PUBLIC ${YAML_CPP_LIBRARIES})

set(TEST_SRC
    #test/logger_test.cpp # for logger 
    test/config_test.cpp # for config
    )

add_executable(test ${TEST_SRC}) 
target_link_libraries(test sylar ${YAML_CPP_LIBRARIES})
target_include_directories(test PUBLIC ${YAML_CPP_INCLUDE_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)