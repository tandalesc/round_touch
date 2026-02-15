cmake_minimum_required(VERSION 3.16)
project(round_touch_sim C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Board define - must be set before LVGL so lv_conf.h can use it
add_compile_definitions(BOARD_SIMULATOR)

# Find SDL2
find_package(SDL2 REQUIRED)

# SDL2 parent include (LVGL needs <SDL2/SDL.h> which requires the parent dir)
get_filename_component(SDL2_INCLUDE_PARENT "${SDL2_INCLUDE_DIRS}" DIRECTORY)

# LVGL configuration
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib/lv_conf.h CACHE STRING "" FORCE)
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
set(LV_CONF_BUILD_DISABLE_DEMOS ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/lvgl ${CMAKE_BINARY_DIR}/lvgl)

# LVGL needs SDL2 headers for its built-in SDL driver
target_include_directories(lvgl PUBLIC ${SDL2_INCLUDE_PARENT})

# Include paths - ORDER MATTERS
# 1. Shims directory (overrides Arduino library headers like <Arduino.h>)
# 2. Simulator directory (for platform/ includes)
# 3. src/ directory (include root for application code)
# 4. LVGL conf (next to lvgl/ folder)
# 5. SDL2 headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/shims
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
    ${SDL2_INCLUDE_DIRS}
)

# Simulator-specific sources
set(SIM_SOURCES
    main.cpp
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
)

add_executable(round_touch_sim ${SIM_SOURCES} ${APP_SOURCES})
target_link_libraries(round_touch_sim lvgl ${SDL2_LIBRARIES})
