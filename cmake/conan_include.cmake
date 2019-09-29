if(EXISTS "${CMAKE_SOURCE_DIR}/conan.cmake")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
endif()

