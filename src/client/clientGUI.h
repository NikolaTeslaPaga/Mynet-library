#pragma once
#ifndef _MYNET_CLIENT_CLIENTGUI_H_
#define _MYNET_CLIENT_CLIENTGUI_H_

#include "client.h"

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include <thread>
#include <string>

namespace mynet {

    class ClientGUI {
    public:

        explicit ClientGUI(Client& client)
            : client(client) {
            client.onReceive = [&](const Client::PacketView& packet) {

                receivedMessages.push_back(
                    std::string(
                        (char*)packet.data,
                        packet.size
                    )
                );
                };
        }

        ~ClientGUI() {
            Disconnect();
        }

    public:

        bool Connect() {

            if (connected)
                return false;

            ApplyConfig();

            if (!client.Start())
                return false;

            receivedMessages.clear();

            bool success = client.Connect(ipBuffer, port);

            if (!success) {
                client.Stop();
                return false;
            }

            connected = true;

            return true;
        }

        bool ConnectWithCode(const std::string& code) {

            auto [ip, decodedPort] = ServerGUI::DecodeJoinCode(code);

            strcpy_s(ipBuffer, ip.c_str());
            port = decodedPort;

            return Connect();
        }

        void Disconnect() {

            if (!connected)
                return;

            connected = false;

            client.Disconnect();

            client.Stop();
        }

    public:

        bool Poll(uint32_t timeOutMs = 0) {
            if (!connected)
                return false;
            client.Poll(timeOutMs);
            return true;
        }

        void Render() {

            if (connected != client.IsConnected()) {
                connected = client.IsConnected();
                if (!connected)
                    client.Stop();
            }

            ImGui::Begin("Client");

            ImGui::InputText("IP", ipBuffer, sizeof(ipBuffer));
            ImGui::InputScalar("Port", ImGuiDataType_U16, &port);

            ImGui::Separator();

            ImGui::InputText("Join Code", joinCodeBuffer, sizeof(joinCodeBuffer));

            if (!connected) {

                if (ImGui::Button("Connect"))
                    Connect();

                ImGui::SameLine();

                if (ImGui::Button("Connect With Code"))
                    ConnectWithCode(joinCodeBuffer);
            }
            else {

                if (ImGui::Button("Disconnect"))
                    Disconnect();
            }

            ImGui::Separator();

            ImGui::Text("Connection Status: %s",
                connected ? "Connected" : "Disconnected");

            if (connected) {

                static char sendBuffer[1024] = "";

                ImGui::InputTextMultiline(
                    "Message",
                    sendBuffer,
                    sizeof(sendBuffer)
                );

                if (ImGui::Button("Send"))
                    client.Send(sendBuffer, strlen(sendBuffer) + 1);

                ImGui::Separator();

                ImGui::Text("Received:");

                ImGui::BeginChild("ReceiveBox", ImVec2(0, 200), true);

                for (const auto& msg : receivedMessages)
                    ImGui::TextWrapped("%s", msg.c_str());

                ImGui::EndChild();
            }

            ImGui::End();
        }

        void Render(
            int width,
            int height,
            bool& isDragging,
            Vector2 dragOffset
        ) {

            if (connected != client.IsConnected()) {
                connected = client.IsConnected();
                if (!connected)
                    client.Stop();
            }

            ImGui::SetNextWindowPos({ 0,0 });
            ImGui::SetNextWindowSize({ float(width), float(height) });

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoScrollbar;

            if (ImGui::IsWindowHovered() &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {

                if (ImGui::GetMousePos().y <
                    ImGui::GetWindowPos().y +
                    ImGui::GetFrameHeight()) {

                    isDragging = true;
                    dragOffset = GetMousePosition();
                }
            }

            ImGui::Begin("Client", nullptr, flags);

            ImGui::InputText("IP", ipBuffer, sizeof(ipBuffer));
            ImGui::InputScalar("Port", ImGuiDataType_U16, &port);

            ImGui::Separator();

            ImGui::InputText("Join Code", joinCodeBuffer, sizeof(joinCodeBuffer));

            if (!connected) {

                if (ImGui::Button("Connect"))
                    Connect();

                ImGui::SameLine();

                if (ImGui::Button("Connect With Code"))
                    ConnectWithCode(joinCodeBuffer);
            }
            else {

                if (ImGui::Button("Disconnect"))
                    Disconnect();
            }

            ImGui::Separator();

            ImGui::Text("Connection Status: %s",
                connected ? "Connected" : "Disconnected");

            if (connected) {

                static char sendBuffer[1024] = "";

                ImGui::InputTextMultiline(
                    "Message",
                    sendBuffer,
                    sizeof(sendBuffer)
                );

                if (ImGui::Button("Send"))
                    client.Send(sendBuffer, strlen(sendBuffer) + 1);


                ImGui::Separator();

                ImGui::Text("Received:");

                ImGui::BeginChild("ReceiveBox", ImVec2(0, 200), true);

                for (const auto& msg : receivedMessages)
                    ImGui::TextWrapped("%s", msg.c_str());

                ImGui::EndChild();
            }

            ImGui::SetCursorPos({
                float(width) - 25,
                float(height) - 25
                });

            ImGui::Button("///", ImVec2(20, 20));

            if (ImGui::IsItemActive() &&
                ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {

                ImVec2 delta = ImGui::GetIO().MouseDelta;

                int newWidth = width + (int)delta.x;
                int newHeight = height + (int)delta.y;

                if (newWidth < 350)
                    newWidth = 350;

                if (newHeight < 400)
                    newHeight = 400;

                SetWindowSize(newWidth, newHeight);
            }

            ImGui::End();
        }

    private:

        void ApplyConfig() {
            client.ReConfig(config);
        }

    private:

        Client& client;
        Client::Config config{};

        bool connected = false;

        std::vector<std::string> receivedMessages;

        uint16_t port = 1234;

        char ipBuffer[64] = "127.0.0.1";

        char joinCodeBuffer[128] = "";

    };

} // namespace mynet

#endif //_MYNET_CLIENT_CLIENTGUI_H_