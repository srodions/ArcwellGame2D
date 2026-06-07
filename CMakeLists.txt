cmake_minimum_required(VERSION 3.19)

# Задаем базовый путь к SDK
set(PSN00BSDK_DIR "C:/PSn00bSDK-0.24-win32")

# Явно объявляем вашу переменную пути к библиотекам
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

# Копируем РЕАЛЬНЫЙ файл exe.ld прямо в папку сборки
configure_file("${PSN00BSDK_LIBS}/ldscripts/exe.ld" "${CMAKE_CURRENT_BINARY_DIR}/exe.ld" COPYONLY)

# Передаем линкеру локальное имя файла скрипта
set(CMAKE_EXE_LINKER_FLAGS "-T exe.ld" CACHE STRING "" FORCE)

# Указываем линкеру точный путь к релизам библиотек
link_directories("${PSN00BSDK_LIBS}/release")

# Создаем исполняемый файл из исходников (ПИШЕТСЯ ОДИН РАЗ)
add_executable(my_game main.c)

# Передаем подпапки с заголовочными файлами SDK
target_include_directories(my_game PRIVATE
    "${PSN00BSDK_DIR}/include"
    "${PSN00BSDK_DIR}/include/libpsn00b"
)

# Заворачиваем библиотеки в циклическую группу линковки
target_link_libraries(my_game PRIVATE 
    -Wl,--start-group
    psxapi_exe_nogprel 
    psxgpu_exe_nogprel 
    psxetc_exe_nogprel
    c_exe_nogprel
    -Wl,--end-group
)

# Конвертируем .elf в чистый .exe для PlayStation 1
add_custom_command(TARGET my_game POST_BUILD
    # Шаг А: Вырезаем отладочную информацию ПК и делаем чистый бинарник
    COMMAND "${PSN00BSDK_DIR}/bin/mipsel-none-elf-objcopy.exe" -O binary my_game.elf my_game.exe
    
    # Шаг Б: Собираем финальный образ диска, используя уже правильный my_game.exe
    COMMAND "${PSN00BSDK_DIR}/bin/mkpsxiso.exe" -y -o "MyFirstPS1Game.bin" -c "MyFirstPS1Game.cue" "${CMAKE_CURRENT_SOURCE_DIR}/isoconfig.xml"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Converting ELF to PSX EXE and building final CD Image..."
)
