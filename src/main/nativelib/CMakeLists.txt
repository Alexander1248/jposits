cmake_minimum_required(VERSION 3.20)
project(PositsJNI LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)

# Пути к SoftPosit (пример, поправь под свои каталоги)
set(SOFTPOSIT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imports/softposits/source)
set(SOFTPOSIT_BUILD ${CMAKE_CURRENT_SOURCE_DIR}/imports/softposits/build)

# JNI (на Windows надо руками указать JAVA_HOME)
if (WIN32)
    set(JAVA_HOME "D:/JDKs/graalvm-ce-24.0.2")
    include_directories(
            "${JAVA_HOME}/include"
            "${JAVA_HOME}/include/win32"
    )
else()
    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})
endif()

include_directories(
        ${SOFTPOSIT_SRC}
        ${SOFTPOSIT_BUILD}
)

# Универсальная функция для сборки под разными ОС
function(add_jposit_target target_name)
    add_library(${target_name} SHARED PositsJNI.cpp)

    target_link_libraries(${target_name}
            ${SOFTPOSIT_BUILD}/Linux-x86_64-GCC/softposit.a
    )

    set_target_properties(${target_name} PROPERTIES
            OUTPUT_NAME "jposits"
    )
endfunction()

# Добавляем отдельные цели под разные ОС
if (WIN32)
    add_jposit_target(PositsJNI_win)
elseif(APPLE)
    add_jposit_target(PositsJNI_macos)
elseif(UNIX)
    add_jposit_target(PositsJNI_linux)
endif()
