
export module AffineMatrix;

import <vector>;
import Polygon;
#include "imgui.h"

using std::vector;
using matrixf = vector<vector<float>>;

export class AffineMatrix {
    matrixf mtr;
    unsigned int cur_poly{};
    ImVec2 point;

    void reset_mtr() {
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 3; j++) {
                if (i == j)
                    mtr[i][j] = 1;
                else
                    mtr[i][j] = 0;
            }
        }
    }

    auto apply(vector<vector<float>> verts, vector<Polygon>& polygons) -> void {
        vector<vector<float>> new_verts(3);
        for (size_t i = 0; i < verts.size(); i++) {
            new_verts[i].resize(3);
        }

        for (size_t k = 0; k < verts.size();k++) {
            for (size_t i = 0; i < 3; i++) {
                new_verts[k][i] = verts[k][0] * mtr[0][i] + verts[k][1] * mtr[1][i] + verts[k][2] * mtr[2][i];
            }
        }

        for (size_t i = 0; i < verts.size(); i++) {
            polygons[cur_poly].v[i].x = new_verts[i][0];
            polygons[cur_poly].v[i].y = new_verts[i][1];
        }
    }

    static matrixf prod(matrixf m1, matrixf m2) {
        matrixf res(3);
        for (size_t i = 0; i < 3; i++) {
            res[i].resize(3);
            for (size_t j = 0; j < 3; j++) {
                for (size_t k = 0; k < 3; k++)
                {
                    res[i][j] += m1[i][k] * m2[k][j];
                }
            }
        }
        return res;
    }

public:
    bool around_center{};

    AffineMatrix() {
        mtr.resize(3);
        for (size_t i = 0; i < 3; i++) {
            mtr[i].resize(3);
            mtr[i][i] = 1;
        }
    }

    void make_offset(int x, int y, vector<Polygon>& polygons) {
        if (!polygons.empty())
            cur_poly = polygons.size() - 1;
        else
            return;

        mtr[2][0] = (float)x;
        mtr[2][1] = (float)y;

        vector<vector<float>> verts;
        for(auto v: polygons[cur_poly].v) {
            verts.push_back({v.x, v.y, 1.0f });
        }

        apply(verts, polygons);
        reset_mtr();
    }

    void make_turning(int turning_val, vector<Polygon>& polygons) {
        if (!polygons.empty())
            cur_poly = polygons.size() - 1;
        else
            return;

        vector<vector<float>> verts;
        for (auto x : polygons[cur_poly].v) {
            verts.push_back({ x.x, x.y, 1.0f });
        }

        ImVec2 center;
        if (around_center) {
            for (auto v: verts) {
                center.x += v[0];
                center.y += v[1];
            }
            center.x = center.x / (float)verts.size();
            center.y = center.y / (float)verts.size();
        }
        else
            center = point;

        matrixf offset(3);
        for (size_t i = 0; i < 3; i++) {
            offset[i].resize(3);
            offset[i][i] = 1;
        }
        offset[2][0] = -center.x;
        offset[2][1] = -center.y;

        turning_val = static_cast<int>(turning_val * 3.14159 / 180);
        mtr[0][0] = (float)cos(turning_val);
        mtr[0][1] = (float)sin(turning_val);
        mtr[1][0] = (float)-sin(turning_val);
        mtr[1][1] = (float)cos(turning_val);

        mtr = prod(offset, mtr);

        offset[2][0] = center.x;
        offset[2][1] = center.y;

        mtr = prod(mtr, offset);

        apply(verts, polygons);

        reset_mtr();
    }



    void make_scaling(float scaling_val_x, float scaling_val_y, vector<Polygon>& polygons) {
        if (!polygons.empty())
            cur_poly = polygons.size() - 1;
        else
            return;

        vector<vector<float>> verts;
        for (auto x : polygons[cur_poly].v) {
            verts.push_back({ x.x, x.y, 1.0f });
        }

        ImVec2 center;
        if (around_center) {
            for (auto v : verts) {
                center.x += v[0];
                center.y += v[1];
            }
            center.x = center.x / (float)verts.size();
            center.y = center.y / (float)verts.size();
        }
        else
            center = point;

        matrixf offset(3);
        for (size_t i = 0; i < 3; i++) {
            offset[i].resize(3);
            offset[i][i] = 1;
        }
        offset[2][0] = -center.x;
        offset[2][1] = -center.y;

        mtr[0][0] = scaling_val_x;
        mtr[1][1] = scaling_val_y;

        mtr = prod(offset, mtr);

        offset[2][0] = center.x;
        offset[2][1] = center.y;

        mtr = prod(mtr, offset);

        apply(verts, polygons);

        reset_mtr();
    }

    void set_center_point(ImVec2 p) {
        point = p;
    }
};
