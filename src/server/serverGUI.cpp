#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NOUSER

#include "enet_template.h"
#include "server/serverGUI.h"
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

int main() {

	SetConfigFlags(FLAG_WINDOW_TRANSPARENT);

	int x = 400, y = 400;
	InitWindow(x, y, "Server");
	SetTargetFPS(60);

	rlImGuiSetup(true);

	bool isDragging = false;
	Vector2 dragOffset = { 0, 0 };

	mynet::ENetInitializer enetInit;

	mynet::Server server;
	mynet::ServerGUI gui(server);

	while (!WindowShouldClose()) {

		gui.Poll();

		x = GetScreenWidth();
		y = GetScreenHeight();

		if (isDragging) {
			Vector2 mousePos = GetMousePosition();
			Vector2 screenMouse = { GetWindowPosition().x + mousePos.x, GetWindowPosition().y + mousePos.y };
			SetWindowPosition(int(screenMouse.x - dragOffset.x), int(screenMouse.y - dragOffset.y));
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				isDragging = false;
			}
		}


		BeginDrawing();
		ClearBackground(BLANK);
		rlImGuiBegin();



		gui.Render(x, y, isDragging, dragOffset);


		rlImGuiEnd();
		EndDrawing();
	}

	rlImGuiShutdown();
	CloseWindow();
}