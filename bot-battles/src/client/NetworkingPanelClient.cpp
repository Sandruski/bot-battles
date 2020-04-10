#include "NetworkingPanel.h"

#include "ClientComponent.h"
#include "GameClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool NetworkingPanel::RenderBody() const
{
    ImGui::Text("%s", GetName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (clientComponent.m_playerID < INVALID_PLAYER_ID) {
        ImGui::Text("Player %u %s", clientComponent.m_playerID, clientComponent.m_name.c_str());

        ImGui::Text("Last akd frame:");
        ImGui::SameLine();
        U32 lastAckdFrame = clientComponent.m_lastAckdFrame;
        ImGui::Text("%u", lastAckdFrame);

        ImGui::Text("Last packet time:");
        ImGui::SameLine();
        F32 lastPacketTime = clientComponent.m_lastPacketTime;
        ImGui::Text("%.f", lastPacketTime);

        ImGui::Text("Deliveries:");
        ImGui::SameLine();
        U32 deliveriesCount = clientComponent.m_deliveryManager.GetDeliveriesCount();
        ImGui::Text("%u", deliveriesCount);

        ImGui::Text("RTT:");
        ImGui::SameLine();
        F32 rtt = clientComponent.m_RTT;
        ImGui::Text("%.2f", rtt);

        ImGui::Spacing();
    }

    ImGui::Checkbox("Client Prediction", &clientComponent.m_isClientPrediction);
    ImGui::Checkbox("Server Reconciliation", &clientComponent.m_isServerReconciliation);
    ImGui::Checkbox("Entity Interpolation", &clientComponent.m_isEntityInterpolation);

    return true;
}
}
