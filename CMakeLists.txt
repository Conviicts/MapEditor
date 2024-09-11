cmake_minimum_required(VERSION 3.29)
project(MapEditor)

set(CMAKE_CXX_STANDARD 20)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

include_directories(incs)

set(IMGUI_DIR "external/imgui")
file(GLOB IMGUI_SOURCES
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_tables.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

set(LODEPNG_DIR "external/lodepng")
file(GLOB LODEPNG_SOURCES
        "${LODEPNG_DIR}/lodepng.cpp"
        "${LODEPNG_DIR}/lodepng_util.cpp"
)

file(GLOB SOURCES "srcs/*.cpp")
file(GLOB HEADERS "incs/*.h")

add_executable(MapEditor ${SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${LODEPNG_SOURCES})

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/*")

foreach(ASSET_FILE ${ASSET_FILES})
    file(RELATIVE_PATH REL_PATH "${ASSETS_DIR}" "${ASSET_FILE}")
    set(DEST_FILE "${CMAKE_BINARY_DIR}/assets/${REL_PATH}")
    get_filename_component(DEST_DIR "${DEST_FILE}" DIRECTORY)
    add_custom_command(
            TARGET MapEditor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSET_FILE}" "${DEST_FILE}"
    )
endforeach()

target_link_libraries(MapEditor PRIVATE glfw)
target_link_libraries(MapEditor PRIVATE GLEW::GLEW)
target_include_directories(MapEditor PRIVATE ${IMGUI_DIR})
target_include_directories(MapEditor PRIVATE ${IMGUI_DIR}/backends)
target_link_libraries(MapEditor
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_include_directories(MapEditor PRIVATE ${LODEPNG_DIR})
