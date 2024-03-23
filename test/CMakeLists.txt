#
# add include for tests
#

include_directories(../include)

add_library(all_libraries
        ../src/sleipnir.c
)

#
# tests
#
link_libraries(gtest gmock_main)

##------------------------------------------------------------------------------
## sleipnir_test.cc
add_executable(sleipnir_test sleipnir_test.cc)
target_link_libraries(sleipnir_test all_libraries)
add_test(NAME sleipnir_test COMMAND sleipnir_test)