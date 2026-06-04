cmake_minimum_required(VERSION 3.15)
project(Tetris VERSION 1.0.0 LANGUAGES CXX)

# Налаштування стандарту C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пошук бібліотеки SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Головний виконуваний файл
add_executable(Tetris main.cpp)

# Підключення бібліотек SFML
target_link_libraries(Tetris PRIVATE sfml-graphics sfml-window sfml-system)
