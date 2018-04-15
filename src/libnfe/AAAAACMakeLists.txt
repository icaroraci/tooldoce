cmake_minimum_required(VERSION 3.9)


find_package(LibXml2 REQUIRED)
message(STATUS LIBXML2_FOUND = ${LIBXML2_FOUND})

file(GLOB SRCS *.c *.h)
add_library(libtooldoce ${SRCS})
