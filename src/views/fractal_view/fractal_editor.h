#pragma once

#include <random>

ImColor interpolate_color(const ImColor& start, const ImColor& end, float t) {
    float r = start.Value.x + t * (end.Value.x - start.Value.x);
    float g = start.Value.y + t * (end.Value.y - start.Value.y);
    float b = start.Value.z + t * (end.Value.z - start.Value.z);
    float a = start.Value.w + t * (end.Value.w - start.Value.w);
    return ImColor(r, g, b, a);
}

auto create_fractal_editor(
        const int DISPLAY_WIDTH,
        const int DISPLAY_HEIGHT,
        unsigned char *EDITOR_IMAGE,
        int IMAGE_EDITOR_WIDTH,
        int IMAGE_EDITOR_HEIGHT,
        int IMAGE_EDITOR_CHANNELS,
        GLuint textureID,
        Fractal& fractal
) -> void {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoResize
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoTitleBar
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus
                                    | ImGuiWindowFlags_NoScrollbar;

    float menuHeight = ImGui::GetFrameHeightWithSpacing();
    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(DISPLAY_WIDTH), static_cast<float>(DISPLAY_HEIGHT) - menuHeight));
    ImGui::Begin("Fractal Editor", nullptr, window_flags);
    ImVec2 image_size(static_cast<float>(IMAGE_EDITOR_WIDTH), static_cast<float>(IMAGE_EDITOR_HEIGHT));
    ImGui::Image((void *)(intptr_t)textureID, image_size);

    ImVec2 image_pos = ImGui::GetItemRectMin();
    ImVec2 start_pos = ImVec2(image_pos.x + image_size.x * fractal.start_pos.x,image_pos.y + image_size.y * fractal.start_pos.y);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angle_dist(-fractal.fractal_params.angle_variation, fractal.fractal_params.angle_variation);
    std::uniform_real_distribution<float> random_angle_dist(-45.0f, 45.0f);




    string lsystem_result_string;
    string init = fractal.lsystem.axiom;
    auto rules = fractal.lsystem.rules;

    FractalParams fp = fractal.fractal_params;
    fp.angle = -90.0f;

    for (int i = 0; i < fractal.fractal_params.iterations; ++i) {
        string temp;
        for (auto c : init) {
            temp += rules.contains(c) ? rules[c] : string(1, c);
        }
        init = temp;
    }
    lsystem_result_string = init;

    float t = 0.0f;
    int total_commands = std::count(lsystem_result_string.begin(), lsystem_result_string.end(), 'F');

    ImVec2 next_pos;
    std::stack<std::pair<ImVec2, float>> state_stack;
    for (char command : lsystem_result_string) {
        next_pos = start_pos;

        if (command == 'F') {
            next_pos.x = start_pos.x + cos(fp.angle * M_PI / 180.0f) * fp.initial_length;
            next_pos.y = start_pos.y + sin(fp.angle * M_PI / 180.0f) * fp.initial_length;

            ImColor interpolated_color = interpolate_color(fp.begin_color, fp.end_color, t);
            ImU32 line_color = IM_COL32(interpolated_color.Value.x * 255, interpolated_color.Value.y * 255, interpolated_color.Value.z * 255, interpolated_color.Value.w * 255);

            draw_list->AddLine(
                    ImVec2(start_pos.x, start_pos.y),
                    ImVec2(next_pos.x, next_pos.y),
                    line_color, fp.initial_width
            );
            start_pos = next_pos;
            fp.initial_length *= fractal.fractal_params.length_reduce;
            fp.initial_width *= fractal.fractal_params.width_reduce;
            t += 1.0f / total_commands;
        } else if (command == '+') {
            float random_angle = fractal.fractal_params.angle + angle_dist(gen);
            fp.angle += random_angle;
        } else if (command == '-') {
            float random_angle = fractal.fractal_params.angle + angle_dist(gen);
            fp.angle -= random_angle;
        } else if (command == '@') {
        } else if (command == '[') {
        state_stack.push({start_pos, fp.angle});
        } else if (command == ']') {
            if (!state_stack.empty()) {
                auto [pos, angle] = state_stack.top();
                state_stack.pop();
                start_pos = pos;
                fp.angle = angle;
            }
        }
    }


    update_texture(
            textureID,
            EDITOR_IMAGE,
            IMAGE_EDITOR_WIDTH,
            IMAGE_EDITOR_HEIGHT,
            IMAGE_EDITOR_CHANNELS
    );

    ImGui::End();
}
