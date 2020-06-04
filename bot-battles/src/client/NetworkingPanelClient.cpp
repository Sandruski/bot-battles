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

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (clientComponent.lock()->m_playerID < INVALID_PLAYER_ID) {
        ImGui::Text("Player %u %s", clientComponent.lock()->m_playerID, clientComponent.lock()->m_name.c_str());

        ImGui::Text("Last akd frame:");
        ImGui::SameLine();
        U32 lastAckdFrame = clientComponent.lock()->m_lastAckdFrame;
        ImGui::Text("%u", lastAckdFrame);

        ImGui::Text("Last packet time:");
        ImGui::SameLine();
        F32 lastPacketTime = clientComponent.lock()->m_lastPacketTime;
        ImGui::Text("%.f", lastPacketTime);

        ImGui::Text("Deliveries:");
        ImGui::SameLine();
        U32 deliveriesCount = clientComponent.lock()->m_deliveryManager.GetDeliveriesCount();
        ImGui::Text("%u", deliveriesCount);

        ImGui::Text("RTT:");
        ImGui::SameLine();
        F32 rtt = clientComponent.lock()->m_RTT;
        ImGui::Text("%.2f", rtt);

        ImGui::Spacing();
    }

    if (ImGui::Checkbox("Client Prediction & Server Reconciliation", &clientComponent.lock()->m_isClientPredictionServerReconciliation)) {
        clientComponent.lock()->m_isClientPrediction = clientComponent.lock()->m_isServerReconciliation = clientComponent.lock()->m_isClientPredictionServerReconciliation;
    }

    ImGui::Checkbox("Entity Interpolation", &clientComponent.lock()->m_isEntityInterpolation);

    return true;
}
}
