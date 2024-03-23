include_directories(../include)

find_package(benchmark REQUIRED)

##------------------------------------------------------------------------------
## bench_demo.cc
add_executable(bench_demo ../benchmark/bench_demo.cc)
target_link_libraries(bench_demo benchmark::benchmark)