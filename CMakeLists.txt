cmake_minimum_required (VERSION 3.10)
project(AkaGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Aka
add_subdirectory(lib/Aka)

set(SRC
	"src/main.cpp"

	"src/System/SpriteAnimatorSystem.h"
	"src/System/SpriteAnimatorSystem.cpp"
	"src/System/CoinSystem.h"
	"src/System/CoinSystem.cpp"
	"src/System/CollisionSystem.h"
	"src/System/CollisionSystem.cpp"
	"src/System/PhysicSystem.h"
	"src/System/PhysicSystem.cpp"
	"src/System/SoundSystem.h"
	"src/System/SoundSystem.cpp"
	"src/System/TextRenderSystem.h"
	"src/System/TextRenderSystem.cpp"
	"src/System/TileMapRenderSystem.h"
	"src/System/TileMapRenderSystem.cpp"
	"src/System/SpriteRenderSystem.h"
	"src/System/SpriteRenderSystem.cpp"
	"src/System/ParticleSystem.h"
	"src/System/ParticleSystem.cpp"
	
	"src/GUI/EntityWidget.h"
	"src/GUI/EntityWidget.cpp"
	"src/GUI/EditorUI.h"
	"src/GUI/EditorUI.cpp"
	"src/GUI/InfoWidget.h"
	"src/GUI/InfoWidget.cpp"
	"src/GUI/Modal.h"
	"src/GUI/Modal.cpp"
	"src/GUI/ResourcesWidget.h"
	"src/GUI/ResourcesWidget.cpp"
	"src/GUI/MenuWidget.h"
	"src/GUI/MenuWidget.cpp"
	"src/GUI/ViewWidget.h"
	"src/GUI/ViewWidget.cpp"
	
	"src/Game/Views/GameView.h"
	"src/Game/Views/GameView.cpp"
	"src/Game/Views/EndView.h"
	"src/Game/Views/EndView.cpp"
	"src/Game/Views/MenuView.h"
	"src/Game/Views/MenuView.cpp"
	"src/Game/Views/IntroView.h"
	"src/Game/Views/IntroView.cpp"
	"src/Game/Resources.h"
	"src/Game/Resources.cpp"
	"src/Game/OgmoWorld.h"
	"src/Game/OgmoWorld.cpp"
	"src/Game/Game.h"
	"src/Game/Game.cpp"
	
	"src/Component/Camera2D.h"
	"src/Component/Coin.h"
	"src/Component/Collider2D.h"
	"src/Component/Player.h"
	"src/Component/SoundInstance.h"
	"src/Component/Text.h"
	"src/Component/TileLayer.h"
	"src/Component/TileMap.h"
	"src/Component/SpriteAnimator.h"
	"src/Component/SpriteAnimator.cpp"
	"src/Component/Transform2D.h"
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
