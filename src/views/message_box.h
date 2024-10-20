#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include"../includes.h"

extern bool show_message_box = false;

void ShowInfoWindow(string text = "")
{
    if (show_message_box)
    {
        // Начало создания окна
        ImGui::Begin("Information", &show_message_box, ImGuiWindowFlags_AlwaysAutoResize);

        // Текст сообщения
        ImGui::Text(text.c_str());

        // Кнопка OK
        if (ImGui::Button("OK"))
        {
            // Закрываем окно при нажатии
            show_message_box = false;
        }

        // Завершение создания окна
        ImGui::End();
    }
}

#endif // !MESSAGE_BOX_H
