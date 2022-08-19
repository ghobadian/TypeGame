cmake_minimum_required()
project(TypeGame)
set(CMAKE_C_STANDARD 11)
add_executable(project Game.c Constants.h)

find_package(Curses REQUIRED)
target_link_libraries(project ${CURSES_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(project Threads::Threads)
