#ifndef COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H
#define COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H

#include "../includes.h"
using namespace std;

bool isDrawing = false;
size_t cur_polygon = 0;
int startX = -1, startY = -1, endX = -1, endY = -1;
vector<Line> lines;
vector<Polygon> polygons;
extern int leftOrRight;
extern int isInside;

Tool tool = Tool::standby;
int thickness = 1;
Color currentColor = {0, 0, 0};

ImVec2 IntersectionPoint;

void handle_mouse_click_on_image(ImVec2 imagePos, ImVec2 imageSize, int width, int height, float zoomLevel, ImVec2 offset) {
    if (ImGui::IsItemClicked()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 adjustedImagePos = ImVec2(imagePos.x + offset.x, imagePos.y + offset.y);
        if (tool == Tool::standby) return;

        float relativeX = (mousePos.x - adjustedImagePos.x) / imageSize.x;
        float relativeY = (mousePos.y - adjustedImagePos.y) / imageSize.y;

        if (relativeX >= 0.0f && relativeX <= 1.0f && relativeY >= 0.0f && relativeY <= 1.0f) {
            int pixelX = static_cast<int>(relativeX * width);
            int pixelY = static_cast<int>(relativeY * height);
            switch (tool) {
                case Tool::polygon:
                    if (!isDrawing) {
                        isDrawing = true;
                        cur_polygon = polygons.size();
                        polygons.emplace_back(
                                vector<Point>{
                                    {
                                        static_cast<float>(pixelX),
                                        static_cast<float>(pixelY)
                                    }
                                    },
                                false,
                                thickness,
                                currentColor
                                );
                    } else {
                        if (abs(pixelX - polygons[cur_polygon].v[0].x) < 10 &&
                            abs(pixelY - polygons[cur_polygon].v[0].y) < 10) {
                            isDrawing = false;
                            polygons[cur_polygon].completed = true;
                            polygons[cur_polygon].v.push_back(polygons[cur_polygon].v[0]);
                        } else
                            polygons[cur_polygon].v.emplace_back(pixelX, pixelY);
                    }
                    break;
                case Tool::find_intersection_point:
                    if (!isDrawing) {
                        startX = pixelX;
                        startY = pixelY;
                        isDrawing = true;
                    }
                    else {
                        endX = pixelX;
                        endY = pixelY;

                        lines.emplace_back(startX, startY, endX, endY, Tool::wu, thickness);
                        if (lines.size() >= 2)
                            IntersectionPoint = find_intersection(lines[lines.size() - 1], lines[lines.size() - 2]);

                        isDrawing = false;
                    }
                    break;
                case Tool::point_orientation_to_edge_check: {
                    if (lines.size() > 0) {
                        leftOrRight = static_cast<int>(check_point_position_relative_edge(lines.back(), pixelX, pixelY));
                    }
                    break;
                }
                case Tool::point_inside_polygon: {
                    if (polygons.size() > 0 && polygons.back().completed) {
                        isInside = static_cast<int>(is_point_inside_polygon(Point(pixelX, pixelY), polygons.back()));
                    }
                    break;
                }
                default:
                    if (!isDrawing) {
                        startX = pixelX;
                        startY = pixelY;
                        isDrawing = true;
                    } else {
                        endX = pixelX;
                        endY = pixelY;

                        lines.emplace_back(startX, startY, endX, endY, tool, thickness, currentColor);

                        isDrawing = false;
                    }
            }
        }
    }
}

#endif
