#include "NetworkingPanel.h"

#include "ClientProxy.h"
#include "GameServer.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool NetworkingPanel::RenderBody() const
{
    ImGui::Text("%s", GetName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        ImGui::Text("Player %u %s", playerID, clientProxy->GetName());

        ImGui::Text("Last akd frame:");
        ImGui::SameLine();
        U32 lastAckdFrame = clientProxy->m_lastAckdFrame;
        ImGui::Text("%u", lastAckdFrame);

        ImGui::Text("Last packet time:");
        ImGui::SameLine();
        F32 lastPacketTime = clientProxy->m_lastPacketTime;
        ImGui::Text("%.f", lastPacketTime);

        ImGui::Text("Deliveries:");
        ImGui::SameLine();
        U32 deliveriesCount = clientProxy->m_deliveryManager.GetDeliveriesCount();
        ImGui::Text("%u", deliveriesCount);

        ImGui::Text("Successful deliveries:");
        ImGui::SameLine();
        U32 successfulDeliveriesCount = clientProxy->m_deliveryManager.GetSuccessfulDeliveriesCount();
        ImGui::Text("%u", successfulDeliveriesCount);

        ImGui::Text("Failed deliveries:");
        ImGui::SameLine();
        U32 failedDeliveriesCount = clientProxy->m_deliveryManager.GetFailedDeliveriesCount();
        ImGui::Text("%u", failedDeliveriesCount);
    }

    ImGui::Checkbox("Server Rewind", &serverComponent.m_isServerRewind);

    return true;
}
}
