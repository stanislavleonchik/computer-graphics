#include "Line.h"

using namespace std;
Line::Line(int x1, int x2, short c1[3], short c2[3])
{
    if (x1 <= x2) {
        
        this->x1 = x1;
        this->x2 = x2;

        copy(c1, c1 + 3, this->c1);
        copy(c2, c2 + 3, this->c2);
        
    }
    else {
        this->x1 = x2;
        this->x2 = x1;

        copy(c1, c1 + 3, this->c2);
        copy(c2, c2 + 3, this->c1);
    }
}

Line::Line()
{
    x1 = 0;
    x2 = 0;
    c1[0] = 0; c1[1] = 0; c1[2] = 0;
    c2[0] = 0; c2[1] = 0; c2[2] = 0;
}

vector<pair<int, ImU32>> Line::GetGradienLine(int y)
{
    vector<pair<int, ImU32>> line;

    double dz = x2 - x1;
    int dR = c2[0] - c1[0];
    int dG = c2[1] - c1[1];
    int dB = c2[2] - c1[2];

    for (int i = x1; i < x2; i++) {
        double cur_dist = (x2 - i) / dz;

        auto cur_c = IM_COL32(c1[0] + round(dR * cur_dist),
            c1[1] + round(dG * cur_dist),
            c1[2] + round(dB * cur_dist),
            255);

            line.push_back(make_pair(i, cur_c));
    }

    return line;
}
