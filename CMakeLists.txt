cmake_minimum_required(VERSION 3.10)

project(AVR LANGUAGES CXX C ASM)

# Set Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# User Defined
# #################################################################

# System Info
# --------------------------------------- #
set(ARDUINO_PATH "/usr/share/arduino")
set(PORT "ttyACM0")
# --------------------------------------- #

# Target Info
# --------------------------------------- #
set(CHIP "atmega2560")
set(PROGRAMMER "stk500v2")
set(DEVICE "m2560")
# --------------------------------------- #

# Programs
# --------------------------------------- #
set(AVR_CPP "avr-g++")
set(AVR_C "avr-gcc")
set(AVR_OBJCOPY "avr-objcopy")
set(AVRDUDE "avrdude")
# --------------------------------------- #

# Add Arduino
# --------------------------------------- #
option(AVR_USE_ARDUINO_LIB OFF)
set(ARDUINO_VARIANT_DEVICE mega)
# --------------------------------------- #

# Generate flags
# --------------------------------------- #
set(OPTIMISATION "-Os")
set(CHIPF "-mmcu=${CHIP}")
set(DEFINES "-DF_CPU=16000000")

set(C_FLAGS "${OPTIMISATION} ${CHIPF} ${DEFINES}")
set(CXX_FLAGS "${OPTIMISATION} ${CHIPF} ${DEFINES}")
set(ASM_FLAGS "${OPTIMISATION} ${CHIPF} ${DEFINES}")
# --------------------------------------- #

# #################################################################

# Set compiler info
# --------------------------------------- #
set(CMAKE_CXX_COMPILER ${AVR_CPP})
set(CMAKE_C_COMPILER ${AVR_C})
set(CMAKE_ASM_COMPILER ${AVR_C})

set(CMAKE_C_FLAGS ${C_FLAGS})
set(CMAKE_CXX_FLAGS ${CXX_FLAGS})
set(CMAKE_ASM_FLAGS ${ASM_FLAGS})
# --------------------------------------- #

set(INCLUDE)
set(SOURCE)

# Get Sources and Includes
file(GLOB_RECURSE SOURCE
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/*.asm"
    "${CMAKE_SOURCE_DIR}/src/*.S"
)

# helper for arduino include functions
function(GetSubdirs ret in)
    set(dirs)

    file(GLOB_RECURSE children LIST_DIRECTORIES true ${in}/*)

    foreach(child ${children})
        if(IS_DIRECTORY ${child})
            list(APPEND dirs ${child})
        endif()
    endforeach()

    SET(${ret} ${dirs} PARENT_SCOPE)
endfunction()

# some paths to arduino folders
set(ARDUINO_AVR_PATH "${ARDUINO_PATH}/hardware/arduino/avr")
set(ARDUINO_LIB_PATH "${ARDUINO_AVR_PATH}/libraries")
set(ARDUINO_STANDARD_LIB_PATH "${ARDUINO_AVR_PATH}/cores/arduino")

# arduino include functions
function(IncludeArduinoStandardLib)
    set(INCLUDE ${INCLUDE} ${ARDUINO_STANDARD_LIB_PATH} PARENT_SCOPE)

    file(GLOB STD_LIB_SOURCE
        "${ARDUINO_STANDARD_LIB_PATH}/*.cpp"
        "${ARDUINO_STANDARD_LIB_PATH}/*.c"
        "${ARDUINO_STANDARD_LIB_PATH}/*.asm"
        "${ARDUINO_STANDARD_LIB_PATH}/*.S"
    )

    set(SOURCE ${SOURCE} ${STD_LIB_SOURCE} PARENT_SCOPE)
endfunction()

function(IncludeArduinoVariants variant)
    set(INCLUDE ${INCLUDE} "${ARDUINO_AVR_PATH}/variants/${variant}" PARENT_SCOPE)
endfunction()

function(IncludeArduinoLib library_name)
    set(LIBRARY_PATH "${ARDUINO_LIB_PATH}/${library_name}")
    set(LIB_SRC_FOLDER "${LIBRARY_PATH}/src")

    set(LIB_SRC)
    file(GLOB_RECURSE LIB_SRC
        "${LIB_SRC_FOLDER}/*.cpp"
        "${LIB_SRC_FOLDER}/*.c"
        "${LIB_SRC_FOLDER}/*.asm"
        "${LIB_SRC_FOLDER}/*.S"
    )

    set(SOURCE ${SOURCE} ${LIB_SRC} PARENT_SCOPE)

    set(LIB_INC)
    GetSubdirs(LIB_INC ${LIBRARY_PATH})

    set(INCLUDE ${INCLUDE} ${LIB_INC} PARENT_SCOPE)
endfunction()

# if using arduino include it
if(${AVR_USE_ARDUINO_LIB})
    IncludeArduinoStandardLib()
    IncludeArduinoVariants(${ARDUINO_VARIANT_DEVICE})
endif()

# Targets
# --------------------------------------- #
add_executable(${PROJECT_NAME}
    ${SOURCE}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE}
)

# Set output name
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

# Custom targets
add_custom_target(HEX ALL ${AVR_OBJCOPY} -O ihex -R .eeprom "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS ${PROJECT_NAME})

add_custom_target(FLASH ALL ${AVRDUDE} -c ${PROGRAMMER} -P "/dev/${PORT}" -p ${DEVICE} -D -U "flash:w:${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex" DEPENDS HEX)
add_custom_target(COMPILE DEPENDS ${PROJECT_NAME})
# --------------------------------------- #