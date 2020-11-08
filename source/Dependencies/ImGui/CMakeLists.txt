project(ImGui)

set(IMGUI_SOURCE
    imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_stdlib.cpp
	imgui_widgets.cpp
	backends/imgui_impl_opengl3.cpp
	tinyfiledialogs.c
)

set(IMGUI_HEADERS
    imconfig.h
	imgui.h
	imgui_internal.h
	imgui_stdlib.h
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
	backends/imgui_impl_opengl3.h
	tinyfiledialogs.h
)


add_library(ImGui STATIC ${IMGUI_SOURCE} ${IMGUI_HEADERS})
find_package(GLEW)
target_link_libraries(ImGui GLEW::GLEW)
target_include_directories(ImGui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
set_target_properties(ImGui PROPERTIES DEBUG_POSTFIX _d)
install(FILES ${IMGUI_HEADERS} DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/Dependencies/ImGui)

install(TARGETS ImGui EXPORT ImGuiTargets)
install(EXPORT ImGuiTargets
        FILE ImGuiTargets.cmake
        NAMESPACE ImGui::
        DESTINATION CMake)
