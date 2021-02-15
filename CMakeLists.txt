cmake_minimum_required (VERSION 3.10)
project(AkaGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Aka
add_subdirectory(lib/Aka)

set(SRC
	"src/main.cpp"

	"src/System/AnimatorSystem.cpp"
	"src/System/CameraSystem.cpp"
	"src/System/CoinSystem.cpp"
	"src/System/CollisionSystem.cpp"
	"src/System/PhysicSystem.cpp"
	"src/System/PlayerSystem.cpp"
	"src/System/SoundSystem.cpp"
	"src/System/TextRenderSystem.cpp"
	"src/System/TileMapSystem.cpp"
	"src/System/TileRenderSystem.cpp"
	"src/System/LevelSystem.cpp"

	"src/GUI/EntityWidget.cpp"
	"src/GUI/GUINode.cpp"
	"src/GUI/InfoWidget.cpp"
	"src/GUI/Modal.cpp"
	"src/GUI/ResourcesWidget.cpp"
	"src/GUI/MenuWidget.cpp"

	"src/Game/Game.cpp"
	"src/Game/Level.cpp"
	"src/Game/Resources.cpp"
	"src/Game/OgmoWorld.cpp"

	"src/Component/Animator.cpp"
	"src/Component/Transform2D.cpp"
)
if(GRAPHIC_OPENGL)
	set(SRC
		${SRC}
		"lib/imgui/imgui.cpp"
		"lib/imgui/imgui_demo.cpp"
		"lib/imgui/imgui_draw.cpp"
		"lib/imgui/imgui_widgets.cpp"
		"lib/imgui/imgui_tables.cpp"
		"lib/imgui/backends/imgui_impl_opengl3.cpp"
		"lib/imgui/backends/imgui_impl_glfw.cpp"
	)
elseif(GRAPHIC_D3D11)
	set(SRC
		${SRC}
		"lib/imgui/imgui.cpp"
		"lib/imgui/imgui_demo.cpp"
		"lib/imgui/imgui_draw.cpp"
		"lib/imgui/imgui_widgets.cpp"
		"lib/imgui/imgui_tables.cpp"
		"lib/imgui/backends/imgui_impl_dx11.cpp"
		"lib/imgui/backends/imgui_impl_glfw.cpp"
	)
endif()
add_executable(AkaGame
	${SRC}
)

# Add third party libs
target_include_directories(AkaGame PUBLIC lib/imgui)
target_include_directories(AkaGame PUBLIC lib/Aka/lib/glfw/include)
target_include_directories(AkaGame PUBLIC lib/Aka/lib/glew/include)
target_include_directories(AkaGame PUBLIC lib/IconFontCppHeaders)
target_include_directories(AkaGame PUBLIC lib/json/include)

target_link_libraries(AkaGame Aka)

add_custom_command(
	TARGET AkaGame POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${PROJECT_SOURCE_DIR}/lib/Aka/lib/glew/bin/glew32.dll"
	$<TARGET_FILE_DIR:AkaGame>
)
add_custom_command(
	TARGET AkaGame POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/asset
	$<TARGET_FILE_DIR:AkaGame>/asset
)
