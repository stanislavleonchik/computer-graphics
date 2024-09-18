#ifndef COMPUTER_GRAPHICS_INCLUDES_H
#define COMPUTER_GRAPHICS_INCLUDES_H

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
#include <glut.h>
#include <queue>
#include <stack>
#include <deque>
#include <cmath>
#include <list>
#include <set>
#include <map>
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

using namespace std;

#endif //COMPUTER_GRAPHICS_INCLUDES_H
