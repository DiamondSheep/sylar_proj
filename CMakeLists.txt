cmake_minimum_required(VERSION 3.10) 
 
project(sylar) 
include (cmake/utils.cmake) # __FILE__
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 
set(CMAKE_CXX_FLAGS "-g -Wno-builtin-macro-redefined") 

find_package(Boost)
find_package(yaml-cpp)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(src/)
set(LIB_SRC
    src/utils.cpp 
    src/config.cpp 
    src/threads.cpp
    src/log.cpp
    )
add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)  # __FILE__
target_include_directories(sylar PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(sylar PUBLIC ${Boost_LIBRARIES})
target_include_directories(sylar PUBLIC ${YAML_CPP_INCLUDE_DIRS})
target_link_libraries(sylar PUBLIC ${YAML_CPP_LIBRARIES})
target_link_libraries(sylar PUBLIC Threads::Threads)

set(TEST_SRC
    #test/logger_test.cpp # for logger 
    #test/config_test.cpp # for config
    #test/pthread_test.cpp # for thread
    test/utils_test.cpp # for utils
    )

add_executable(test ${TEST_SRC}) 
force_redefine_file_macro_for_sources(test)  # __FILE__
target_link_libraries(test sylar ${YAML_CPP_LIBRARIES})
target_include_directories(test PUBLIC ${YAML_CPP_INCLUDE_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)