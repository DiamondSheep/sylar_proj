cmake_minimum_required(VERSION 3.10) 
 
project(sylar) 
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 
set(CMAKE_CXX_FLAGS -g) 
 
aux_source_directory(src/ SOURCE_DIR) 
 
list(APPEND EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/include) 
 
#list(APPEND EXTRA_LIBS ${PROJECT_SOURCE_DIR}/lib) 
 
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_DIR}) 
 
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR} ${EXTRA_INCLUDES}) 
#target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC ${EXTRA_LIBS})
