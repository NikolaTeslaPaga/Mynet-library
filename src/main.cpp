//#include "main.h"
//
//#include <conio.h>
//#include "server/serverGUI.h"
//
//int main() {
//
//	SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);
//	InitWindow(800, 450, "invisible server");
//
//	rlImGuiSetup(true);
//
//
//	mynet::ENetInitializer enetInit;
//
//	mynet::Server server;
//	mynet::ServerGUI gui(server);
//
//	while (!WindowShouldClose()) {
//
//		BeginDrawing();
//		ClearBackground(BLANK);
//		rlImGuiBegin();
//
//		ImGui::SetNextWindowPos({ 0, 0 });
//		ImGui::SetNextWindowSize({ 400, 400 });
//
//		gui.Render();
//
//		rlImGuiEnd();
//		EndDrawing();
//	}
//
//	rlImGuiShutdown();
//	CloseWindow();
//}


/*

CLIENT EXAMPLE

std::string code = "ABC123";
auto [ip, port] = mynet::ServerGUI::DecodeJoinCode(code);

mynet::Client client;
client.start();
client.Connect(ip, port);

*/


//int main() {
//	mynet::ENetInitializer enetInit;
//
//	mynet::Server server;
//
//	if (!server.Start())
//		return EXIT_FAILURE;
//	std::cout << "Server started on port: " << server.GetConfig().port << '\n';
//
//	//mynet::Client client;
//	//const char* msg = "Hellow!";
//
//	for (bool running = true; running;) {
//		server.Poll(1);
//
//		if (_getch() == 27)
//			running = false;
//	}
//}


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