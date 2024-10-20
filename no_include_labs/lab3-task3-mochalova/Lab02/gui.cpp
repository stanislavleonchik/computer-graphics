#include "gui.h"

AppState currentState = AppState::Lab03;

void drawMainMenu() {
    ImGui::Begin("Main Menu");

    if (ImGui::Button("Lab03")) {
        currentState = AppState::Lab03;
    }

    if (ImGui::Button("Lab04")) {
        currentState = AppState::Lab04;
    }

    ImGui::End();
}

