#pragma once
#ifndef _MYNET_SERVER_SERVERGUI_H_
#define _MYNET_SERVER_SERVERGUI_H_

#include "enetGlobal.h"
#include "server.h"
#include "joinCode.h"

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include <thread>
#include <string>

namespace mynet {

	class ServerGUI {
	public:

		explicit ServerGUI(Server& server)
			:server(server) {}

		~ServerGUI() {
			Stop();
		}

	public:

		bool Start() {
			if (running)
				return false;

			ApplyConfig();

			if (!server.Start())
				return false;



			running = true;
			joinCode = joinCode::EncodeJoinCode(ip, config.port, codeConfig);

			return true;
		}

		void Stop() {
			if (!running)
				return;

			running = false;
			joinCode.clear();

			server.Stop();
		}

		void Render() {
			ImGui::Begin("Server");

			ImGui::InputScalar("Port", ImGuiDataType_U16, &config.port);
			ImGui::InputScalar("Max Clients", ImGuiDataType_U64, &config.maxClients);
			ImGui::InputScalar("Channels", ImGuiDataType_U64, &config.channels);
			ImGui::InputScalar("Incoming Bandwidth", ImGuiDataType_U64, &config.incomingBandwidth);
			ImGui::InputScalar("Outgoing Bandwidth", ImGuiDataType_U64, &config.outgoingBandwidth);
			ImGui::InputScalar("Packet Size", ImGuiDataType_U64, &config.packetSize);
			ImGui::InputScalar("Buffer Size", ImGuiDataType_U64, &config.bufferSize);

			ImGui::Text("IP: %s", ip.c_str());

			if (!running) {
				if (ImGui::Button("Start Server"))
					Start();
			}
			else {
				if (ImGui::Button("Stop Server"))
					Stop();
			}


			ImGui::Separator();
			ImGui::Text("Server Status: %s", running ? "Running" : "Stopped");

			if (running) {
				ImGui::Text("Connected Clients: %zu", server.GetPeerAmount());
				ImGui::Text("Join Code: %s", joinCode.c_str());
				ImGui::SameLine();
				if (ImGui::Button("Copy Code"))
					ImGui::SetClipboardText(joinCode.c_str());


				static char broadcastBuffer[1024] = "";
				ImGui::InputTextMultiline("Broadcast Message", broadcastBuffer, sizeof(broadcastBuffer));

				if (ImGui::Button("Broadcast To All"))
					server.Broadcast(broadcastBuffer, strlen(broadcastBuffer) + 1);


				if (ImGui::Button("Disconnect All"))
					server.DisconnectAll();
			}

			ImGui::End();
		}

		void Render(
			int width,
			int height,
			bool& isDragging,
			Vector2 dragOffset
		) {

			ImGui::SetNextWindowPos({ 0,0 });
			ImGui::SetNextWindowSize({ float(width), float(height)});

			ImGuiWindowFlags flags = 
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoScrollbar;

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				if (ImGui::GetMousePos().y < ImGui::GetWindowPos().y + ImGui::GetFrameHeight()) {
					isDragging = true;
					dragOffset = GetMousePosition();
				}
			}


			ImGui::Begin("Server", nullptr, flags);

			ImGui::InputScalar("Port", ImGuiDataType_U16, &config.port);
			ImGui::InputScalar("Max Clients", ImGuiDataType_U64, &config.maxClients);
			ImGui::InputScalar("Channels", ImGuiDataType_U64, &config.channels);
			ImGui::InputScalar("Incoming Bandwidth", ImGuiDataType_U64, &config.incomingBandwidth);
			ImGui::InputScalar("Outgoing Bandwidth", ImGuiDataType_U64, &config.outgoingBandwidth);
			ImGui::InputScalar("Packet Size", ImGuiDataType_U64, &config.packetSize);
			ImGui::InputScalar("Buffer Size", ImGuiDataType_U64, &config.bufferSize);

			ImGui::Text("IP: %s", ip.c_str());

			if (!running) {
				if (ImGui::Button("Start Server"))
					Start();
			}
			else {
				if (ImGui::Button("Stop Server"))
					Stop();
			}

			ImGui::Separator();
			ImGui::Text("Server Status: %s", running ? "Running" : "Stopped");

			if (running) {
				ImGui::Text("Connected Clients: %zu", server.GetPeerAmount());
				ImGui::Text("Join Code: %s", joinCode.c_str());
				ImGui::SameLine();
				if (ImGui::Button("Copy code"))
					ImGui::SetClipboardText(joinCode.c_str());


				static char broadcastBuffer[1024] = "";
				ImGui::InputTextMultiline("Broadcast Message", broadcastBuffer, sizeof(broadcastBuffer));

				if (ImGui::Button("Broadcast To All"))
					server.Broadcast(broadcastBuffer, strlen(broadcastBuffer) + 1);

				if (ImGui::Button("Disconnect All"))
					server.DisconnectAll();
			}


			ImGui::SetCursorPos({ float(width) - 25, float(height) - 25 });
			ImGui::Button("###resize", ImVec2(20, 20));

			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				ImVec2 delta = ImGui::GetIO().MouseDelta;
				int newWidth = width + (int)delta.x;
				int newHeight = height + (int)delta.y;

				// Define clean boundaries so inputs remain readable
				if (newWidth < 350)  newWidth = 350;
				if (newHeight < 400) newHeight = 400;

				SetWindowSize(newWidth, newHeight);
			}

			ImGui::End();
		}

		void Poll(uint32_t timeOutMs = 0) {
			if (running)
				server.Poll(timeOutMs);
		}

	private:

		void ApplyConfig() {
			server.ReConfig(config);
		}


	private:
		Server& server;
		Server::Config config{};

		bool running = false;

		joinCode::Config codeConfig{};
		std::string joinCode = std::string(12, '*');

		std::string serverName = "My Server!!!";
		std::string ip = GetLocalIP();


	};


} //namespace mynet

#endif //_SERVER_SERVERGUI_