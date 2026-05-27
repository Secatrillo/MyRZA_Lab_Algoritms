cmake_minimum_required(VERSION 3.10)
project(RzaProject)
include(CMakeLists.macro.cmake)

# 1. Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_TESTING OFF)

add_subdirectory(libs)

# 2. Подключаем папки с заголовочными файлами (аналог -I)
include_directories(
    .
    ./GlobalController
    ./utils
    ./ExLabLibs
)

# 3. Собираем ВСЕ .cpp файлы рекурсивно (чтобы не перечислять 40+ файлов вручную)
# Это создаст список всех исходников в переменную SOURCES
file(GLOB_RECURSE SOURCES 
    "main.cpp"
    "ExLabLibs/ParserComtrade.cpp"
    "GlobalController/*.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX ".*cppzmq/*")

# 4. Создаем исполняемый файл
add_executable(main_app ${SOURCES})

set(BIN_DIR "${CMAKE_SOURCE_DIR}/Output/")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/Output/")

# Executable
set_target_properties(main_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

# Internal libraries: Build to app's lib folder
foreach(lib IN LISTS ARG_INTERNAL_LIBS)
    set_target_properties(${lib} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}
    )
endforeach()

target_link_libraries(main_app cppzmq)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
target_include_directories(main_app PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(main_app ${Python3_LIBRARIES})

# 5. (Опционально) Название выходного файла как у вас — main.exe
set_target_properties(main_app PROPERTIES OUTPUT_NAME "main")

# Копируем папку со всеми конфигурациями в директорию сборки
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Start_line" 
     DESTINATION "${BIN_DIR}")

# Копируем папку со всеми конфигурациями в директорию сборки
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/End_line" 
     DESTINATION "${BIN_DIR}")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/DProtComt"
     DESTINATION "${BIN_DIR}")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/utils/Monitoring.py" 
     DESTINATION "${BIN_DIR}")
