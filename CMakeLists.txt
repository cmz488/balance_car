cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME balance_car)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Algorithm/Src/FusionAHRS.cpp
        Encoder/Src/Encoder.c
        LED/Src/LED.c
        key/Src/key.c
        MPU6050/Src/MPU6050.c
        MPU6050/Src/MyI2C.c
        PWM/Src/Motor.c
        PWM/Src/PWM.c
        Encoder/Inc/Encoder.h
        BlueSerial/Src/Serial.c
        PWM/Inc/Motor.h
        BlueSerial/Inc/Serial.h
       #gray_sensor/Src/GraySensor.cpp
       #gray_sensor/Inc/GraySensor.h
        Bsp/src/bsp_dwt.cpp
        #Task/Inc/Mpu6050Task.h
        #Task/Inc/KeyScan.h
        #Task/Inc/Debug.h
        #Task/Inc/ChassisRun.h
        #Task/Src/Mpu6050Task.cpp
        #Task/Src/KeyScan.cpp
        #Task/Src/ChassisRun.cpp
        ChassisCtrl/ChassisStrategy.h
        #Task/Src/GraySensorTask.cpp
        #Task/Inc/GraySensorTask.h
        Algorithm/Src/MahonyAHRS.c
       ChassisCtrl/Src/ChassisCtrl.cpp
        #ChassisCtrl/Src/BalanceCtrl.cpp
      #ChassisCtrl/Inc/BalanceCtrl.h
        BUZZ/Src/Buzz.cpp
        BUZZ/Inc/Buzz.h
       ChassisCtrl/Src/ChassisOnline.cpp
       ChassisCtrl/Inc/ChassisOnline.h
      ChassisCtrl/Src/ChassisOutline.cpp
       ChassisCtrl/Inc/ChassisOutline.h
        #Serial/Src/Serial.cpp
        ssb/control/control.c
        ssb/control/control.h
        PID/PID.c
        ChassisCtrl/Src/ChassisOutline.cpp
        ChassisCtrl/Src/ChassisOnline.cpp
        ChassisCtrl/Src/ChassisCtrl.cpp
        gray_sensor/Src/grayscale_tracker.c
        # Task/Src/Debug.cpp
)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths

)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        Algorithm/Inc
      #  BlueSerial/Inc
        Bsp/Inc
        Encoder/Inc
        key/Inc
        LED/Inc
        MPU6050/Inc
        PID/
        PWM/Inc
        ##Task/
        ##Task/Inc
        gray_sensor/Inc
       ChassisCtrl/Inc
       ChassisCtrl
        BUZZ/Inc
        ssb/control
        #Serial/inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
