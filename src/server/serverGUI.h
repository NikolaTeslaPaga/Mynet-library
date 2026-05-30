#pragma once
#ifndef _MYNET_SERVER_SERVERGUI_H_
#define _MYNET_SERVER_SERVERGUI_H_

#include "server.h"

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
			joinCode = EncodeJoinCode(ipBuffer, config.port);

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


				static char broadcastBuffer[1024] = "";
				ImGui::InputTextMultiline("Broadcast Message", broadcastBuffer, sizeof(broadcastBuffer));

				if (ImGui::Button("Broadcast To All"))
					server.Broadcast(broadcastBuffer, strlen(broadcastBuffer) + 1);

				if (ImGui::Button("Disconnect All"))
					server.DisconnectAll();
			}


			ImGui::SetCursorPos({ float(width) - 25, float(height) - 25 });
			ImGui::Button("///", ImVec2(20, 20));

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

	public:

		static std::string IntToCode(uint64_t v) {
			if (v == 0)
				return "0";
			std::string s;
			while (v > 0) {
				s.push_back(digits[v % base]);
				v /= base;
			}
			std::reverse(s.begin(), s.end());
			return s;
		}
		static uint64_t CodeToInt(const std::string& code) {
			uint64_t v = 0;

			for (char c : code) {
				size_t pos = digits.find(c);
				if (pos == std::string::npos)
					throw std::runtime_error("Invalid join code character");

				v = v * base + static_cast<uint64_t>(pos);
			}

			return v;
		}

		static std::string EncodeJoinCode(const std::string& ip, uint16_t port) {
			ENetAddress addr;
			enet_address_set_host(&addr, ip.c_str());
			uint32_t ipInt = ntohl(addr.host);
			uint64_t codeInt = (uint64_t(ipInt) << 16) | port;
			return IntToCode(codeInt);
		}

		static std::pair<std::string, uint16_t> DecodeJoinCode(const std::string& code) {
			uint64_t packed = CodeToInt(code);

			uint16_t port = static_cast<uint16_t>(packed & 0xFFFF);
			uint32_t ipInt = static_cast<uint32_t>(packed >> 16);

			ENetAddress addr{};
			addr.host = htonl(ipInt);

			char ip[64];
			enet_address_get_host_ip(&addr, ip, sizeof(ip));

			return { std::string(ip), port };
		}

	private:

		void ApplyConfig() {
			server.ReConfig(config);
		}

		static inline const std::string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		static inline const size_t base = digits.size();

	private:
		Server& server;
		Server::Config config{};

		bool running = false;

		std::string joinCode;

		char serverName[128] = "My Server!!!";
		char ipBuffer[64] = "127.0.0.1";


	};


} //namespace mynet

#endif //_SERVER_SERVERGUI_