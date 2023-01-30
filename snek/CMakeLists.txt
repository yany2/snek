add_executable(snek "snek.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET snek PROPERTY CXX_STANDARD 20)
endif()


target_include_directories(snek PRIVATE "${CMAKE_SOURCE_DIR}/extern/SDL/include")

target_link_directories(snek PRIVATE "${CMAKE_SOURCE_DIR}/out/build/x64-release/extern/SDL")

target_link_libraries(snek "SDL2main.lib" "SDL2-static.lib" "Winmm.lib" "Setupapi.lib" "Version.lib" "Imm32.lib")