cmake_minimum_required(VERSION 3.19)

# Заставляем CMake собирать проект строго в режиме оптимизированного релиза
set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)

# Задаем базовый путь к SDK
set(PSN00BSDK_DIR "C:/PSn00bSDK-0.24-win32")
set(PSN00BSDK_LIBS "${PSN00BSDK_DIR}/lib/libpsn00b")

# Принудительно указываем компиляторы для PS1 до объявления проекта
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mipsel)
set(CMAKE_C_COMPILER "${PSN00BSDK_DIR}/bin/mipsel-none-elf-gcc.exe")
set(CMAKE_CXX_COMPILER "${PSN00BSDK_DIR}/bin/mipsel-none-elf-gcc.exe")
set(CMAKE_ASM_COMPILER "${PSN00BSDK_DIR}/bin/mipsel-none-elf-gcc.exe")

# Подключаем остальной тулчейн
set(CMAKE_TOOLCHAIN_FILE "${PSN00BSDK_LIBS}/cmake/sdk.cmake" CACHE PATH "Path to PS1 Toolchain" FORCE)
list(APPEND CMAKE_MODULE_PATH "${PSN00BSDK_LIBS}/cmake")

project(PS1_Game C)
set(CMAKE_C_STANDARD 99)

# Насильно заставляем CMake создавать файл с расширением .elf
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Копируем файл exe.ld прямо в папку сборки
configure_file("${PSN00BSDK_LIBS}/ldscripts/exe.ld" "${CMAKE_CURRENT_BINARY_DIR}/exe.ld" COPYONLY)

# Базовые рабочие флаги линкера
set(CMAKE_EXE_LINKER_FLAGS "-T exe.ld -static" CACHE STRING "" FORCE)

# Указываем линкеру точный путь к релизам библиотек
link_directories("${PSN00BSDK_LIBS}/release")

# Создаем исполняемый файл из исходников
add_executable(my_game main.c)

# Передаем подпапки с заголовочными файлами SDK
target_include_directories(my_game PRIVATE
    "${PSN00BSDK_DIR}/include"
    "${PSN00BSDK_DIR}/include/libpsn00b"
)

# Заворачиваем БАЗОВЫЕ библиотеки в циклическую группу линковки
target_link_libraries(my_game PRIVATE 
    -Wl,--start-group
    psxapi_exe_nogprel 
    psxgpu_exe_nogprel 
    psxetc_exe_nogprel
    c_exe_nogprel
    -Wl,--end-group
)

# Автоматизация сборки образа диска строго в корне проекта
add_custom_command(TARGET my_game POST_BUILD
    # 1. Конвертируем .elf в правильный бинарник my_game.exe внутри папки build
    COMMAND "${PSN00BSDK_DIR}/bin/elf2x.exe" my_game.elf my_game.exe
    
    # 2. Копируем получившийся my_game.exe из папки build прямо в корень проекта (где его ждет isoconfig)
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/my_game.exe" "${CMAKE_CURRENT_SOURCE_DIR}/my_game.exe"
    
    # 3. !!! ИСПРАВЛЕНО: Передаем АБСОЛЮТНЫЙ путь к файлу конфигурации isoconfig.xml
    COMMAND "${PSN00BSDK_DIR}/bin/mkpsxiso.exe" -y -o "${CMAKE_CURRENT_BINARY_DIR}/MyFirstPS1Game.bin" -c "${CMAKE_CURRENT_BINARY_DIR}/MyFirstPS1Game.cue" "${CMAKE_CURRENT_SOURCE_DIR}/isoconfig.xml"
    
    # 4. Применяем патч лицензии к готовому .bin файлу внутри папки build
    COMMAND "${PSN00BSDK_DIR}/bin/psxlicensepatch.exe" --region EUR MyFirstPS1Game.bin
    
    # 5. Чистим корень проекта от временного файла my_game.exe
    COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_CURRENT_SOURCE_DIR}/my_game.exe"
    
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Executing flat root CD-ROM image build process..."
)