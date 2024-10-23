#ifndef COMPUTER_GRAPHICS_INCLUDES_H
#define COMPUTER_GRAPHICS_INCLUDES_H

const int display_w = 1280, display_h = 720;

#define USE_MATH_DEFINES

#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <climits>
#include <iomanip>
#include <chrono>
#include <vector>
#include <bitset>
#include <limits>
#include <string>
#include <random>
#include <queue>
#include <stack>
#include <deque>
#include <cmath>
#include <list>
#include <set>
#include <map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

using namespace std;

#include "models/Tool.h"
#include "models/Color.h"
#include "models/Line.h"
#include "models/Point.h"
#include "models/Polygon.h"
#include "utils/converters/converters.h"
#include "utils/load_image.h"
#include "utils/converters/rgb_to_hsv.h"
#include "views/create_sliders.h"
#include "views/create_line_controls.h"
#include "utils/midpoint_displacement/draw_midpoint_displacement.h"
#include "views/midpoint_displacement_tools.h"


#include "utils/polygon_transform/check_point_position_relative_edge.h"
#include "utils/polygon_transform/Is_point_inside_polygon.h"
#include "utils/polygon_transform/edge_intersection.h"
#include "views/message_box.h"
#include "utils/line_draw/draw_bresenham_line.h"
#include "utils/polygon_transform/affine_transformations.h"
#include "utils/handle_mouse_click_on_image.h"
#include "views/create_tools.h"
#include "utils/line_draw/wu.h"
#include "utils/polygon_transform/check_point_position_relative_edge.h"
#include "utils/polygon_transform/Is_point_inside_polygon.h"
#include "utils/line_draw/draw_bresenham_line.h"
#include "utils/handle_mouse_click_on_image.h"
#include "views/create_tools.h"
#include "utils/line_draw/wu.h"
#include "views/affine_tools.h"
#include "utils/polygon_transform/affine_transformations.h"
#include "utils/polygon_transform/edge_intersection.h"
#include "views/message_box.h"
#include "views/editor/editor.h"

#define watch(x) cerr << "\n" << (#x) << " is " << (x) << endl
#define print(x) cerr << endl; for (auto a: x) cerr << (a) << " "; cerr << endl
#define alle(x) (x).begin(), (x).end()
#define ralle(x) (x).rbegin(), (x).rend()
#define lb(x, t) lower_bound(alle((x)), t)
#define ub(x, t) upper_bound(alle((x)), t)
#define pb push_back
#define el '\n'

using ull = unsigned long long;
using ll = long long;
using uc = unsigned char;

#endif
