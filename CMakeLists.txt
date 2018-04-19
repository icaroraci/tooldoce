cmake_minimum_required(VERSION 2.8)


find_package(LibXml2 REQUIRED)
  message(STATUS LIBXML2_FOUND = ${LIBXML2_FOUND})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(LIBNFE_INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(LIBNFE_SRC ${PROJECT_SOURCE_DIR}/src)

include_directories(${LIBNFE_INCLUDE}
                    ${LIBNFE_SRC}
                    ${LIBXML2_INCLUDE_DIR}
                    
)

file(GLOB MYLIB ${LIBNFE_SRC}/libnfe/*.c)

foreach(SOURCE ${MYLIB})
  message(STATUS Arquivo alvo: ${SOURCE})
endforeach(SOURCE)

add_library(libtooldoce STATIC ${MYLIB})
