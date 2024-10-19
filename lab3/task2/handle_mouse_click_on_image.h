#ifndef COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H
#define COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H

#include "../includes.h"
#include"../lab4/edge_intersection.h"
using namespace std;

bool isDrawing = false;
size_t cur_polygon = 0;
int startX = -1, startY = -1, endX = -1, endY = -1;
vector<line> lines;
vector<Polygon> polygons;
extern int leftOrRight;
extern int isInside;

Tool tool = Tool::standby;
int thickness = 1;

ImVec2 IntersectionPoint;

void handle_mouse_click_on_image(ImVec2 imagePos, ImVec2 imageSize, int width, int height) {
    if (ImGui::IsItemClicked()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        if (tool == Tool::standby) return;

        float relativeX = (mousePos.x - imagePos.x) / imageSize.x;
        float relativeY = (mousePos.y - imagePos.y) / imageSize.y;

        int pixelX = static_cast<int>(relativeX * width);
        int pixelY = static_cast<int>(relativeY * height);

        switch (tool) {
            case Tool::polygon:
                if (!isDrawing) {
                    isDrawing = true;
                    cur_polygon = polygons.size();
                    polygons.emplace_back(vector<Point>{{(float)pixelX, (float)pixelY}});
                } else {
                    if (abs(pixelX - polygons[cur_polygon].v[0].x) < 10 && abs(pixelY - polygons[cur_polygon].v[0].y) < 10) {
                        isDrawing = false;
                        polygons[cur_polygon].completed = true;
                    } else
                        polygons[cur_polygon].v.emplace_back((float)pixelX, (float)pixelY);
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
                    leftOrRight = static_cast<int>(сheckPointPositionRelativeEdge(lines.back(), pixelX, pixelY));
                }
                break;
            }
            case Tool::point_inside_polygon: {
                if (polygons.size() > 0 && polygons.back().completed) {
                    isInside = static_cast<int>(isPointInsidePolygon(Point(pixelX, pixelY), polygons.back()));
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

                    lines.emplace_back(startX, startY, endX, endY, tool, thickness);

                    isDrawing = false;
                }
        }
    }
}

#endif
