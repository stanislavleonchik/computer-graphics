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

#include "lab2/rgb2hsv/converters.h"
#include "lab2/rgb2hsv/load_image.h"
#include "lab2/rgb2hsv/rgb2hsv.h"
#include "lab2/rgb2hsv/create_sliders.h"
#include "lab3/task2/create_line_controls.h"

#include "lab4/tools.h"
#include "lab3/task2/line.h"
#include "lab4/Point.h"
#include "lab4/Polygon.h"
#include "lab4/CheckPointPositionRelativeEdge.h"
#include "lab4/IsPointInsidePolygon.h"
#include "lab3/task2/draw_bresenham_line.h"
#include "lab3/task2/handle_mouse_click_on_image.h"
#include "lab3/task2/create_tools.h"
#include "lab3/task2/wu.h"
#include "lab4/affine_tools.h"
#include "lab4/affine_transformations.h"
#include "lab4/edge_intersection.h"
#include "lab4/message_box.h"


#include "editor.h"

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




#endif //COMPUTER_GRAPHICS_INCLUDES_H
