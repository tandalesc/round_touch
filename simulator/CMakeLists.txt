cmake_minimum_required(VERSION 3.16)
project(round_touch_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include paths - ORDER MATTERS
# 1. Shims directory (overrides Arduino library headers like <Arduino.h>)
# 2. Project root (resolves "src/..." include paths)
# 3. SDL2 headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/shims
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${SDL2_INCLUDE_DIRS}
)

# Simulator-specific sources
set(SIM_SOURCES
    main.cpp
    platform/SimDisplay.cpp
    platform/SimTouch.cpp
)

# Shared application sources from src/ (compile against shims unchanged)
set(APP_SOURCES
    ../src/application/Application.cpp
    ../src/application/interface/Interface.cpp
    ../src/application/interface/components/ComponentManager.cpp
    ../src/application/interface/components/types/Component.cpp
    ../src/application/interface/components/types/ComponentWithChildren.cpp
    ../src/application/interface/components/types/Layout.cpp
    ../src/application/interface/components/layout/FlexLayout.cpp
    ../src/application/workflow/Workflow.cpp
    ../src/application/workflow/ComponentLibrary.cpp
    ../src/device/Device.cpp
    ../src/device/hw/Touch.cpp
)

add_executable(round_touch_sim ${SIM_SOURCES} ${APP_SOURCES})
target_link_libraries(round_touch_sim ${SDL2_LIBRARIES})
