add_executable(snek WIN32 "snek.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET snek PROPERTY CXX_STANDARD 20)
endif()


add_dependencies(snek SDL2::SDL2main SDL2::SDL2-static)

target_include_directories(snek PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/SDL/include)

target_link_libraries(snek SDL2::SDL2main SDL2::SDL2-static)
target_link_libraries(snek Winmm Setupapi Version Imm32)


add_custom_target(textures DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/textures)
add_custom_command(
	TARGET textures
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/textures ${CMAKE_CURRENT_BINARY_DIR}/textures
)
add_dependencies(snek textures)
