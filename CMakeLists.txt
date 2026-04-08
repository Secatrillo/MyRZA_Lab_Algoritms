cmake_minimum_required(VERSION 3.10)
project(RzaProject)

# 1. Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Подключаем папки с заголовочными файлами (аналог -I)
include_directories(
    .
    ./GlobalController
)

# 3. Собираем ВСЕ .cpp файлы рекурсивно (чтобы не перечислять 40+ файлов вручную)
# Это создаст список всех исходников в переменную SOURCES
file(GLOB_RECURSE SOURCES 
    "main.cpp"
    "ExLabLibs/ParserComtrade.cpp"
    "GlobalController/*.cpp"
)

# 4. Создаем исполняемый файл
add_executable(main_app ${SOURCES})

# 5. (Опционально) Название выходного файла как у вас — main.exe
set_target_properties(main_app PROPERTIES OUTPUT_NAME "main")

# Копируем папку со всеми конфигурациями в директорию сборки
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Start_line" 
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Копируем папку со всеми конфигурациями в директорию сборки
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/End_line" 
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
