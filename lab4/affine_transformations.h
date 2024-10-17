#ifndef AFFINE_TRANSFORMATIONS_H
#define AFFINE_TRANSFORMATIONS_H

#include "../includes.h"
#include "../lab4/affine_tools.h"

using matrixf = vector<vector<float>>;

class AffineMatrix {

private:
	matrixf mtr;
	unsigned int cur_poly;
	ImVec2 point;

	void reset_mtr() {
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (i == j)
					mtr[i][j] = 1;
				else
					mtr[i][j] = 0;
			}
		}
	}

	void apply(vector<vector<float>> verts) {

		for (auto& v: verts)
		{
			for (size_t i = 0; i < 3; i++)
			{
				v[i] = v[i] * (mtr[i][0] + mtr[i][1] + mtr[i][2]);
			}
		}

		for (size_t i = 0; i < verts.size(); i++)
		{
			polygons[cur_poly].v[i].x = verts[i][0];
			polygons[cur_poly].v[i].y = verts[i][1];
		}
	}

	matrixf prod(matrixf m1, matrixf m2) {

		matrixf res(3);
		for (size_t i = 0; i < 3; i++)
		{
			res[i].resize(3);
			for (size_t j = 0; j < 3; j++)
			{
				res[i][j] = m1[i][j] * m2[i][j];
			}
		}
		return res;
	}

public:

	bool around_center;

	AffineMatrix() {
		mtr.resize(3);
		for (size_t i = 0; i < 3; i++)
		{
			mtr[i].resize(3);
			mtr[i][i] = 1;
		}
	}

	void make_offset(int x, int y) {

		if (!polygons.empty())
			cur_poly = polygons.size() - 1;
		else
			return;

		mtr[2][0] = x;
		mtr[2][1] = y;

		vector<vector<float>> verts;
		for(auto x: polygons[cur_poly].v)
		{
			verts.push_back({ x.x, x.y, 1.0f });
		}

		apply(verts);

		reset_mtr();

	}
	

	void make_turning(int turning_val) {
		if (!polygons.empty())
			cur_poly = polygons.size() - 1;
		else
			return;


		vector<vector<float>> verts;
		for (auto x : polygons[cur_poly].v)
		{
			verts.push_back({ x.x, x.y, 1.0f });
		}

		ImVec2 center;
		if (around_center) {
			for (auto v: verts)
			{
				center.x += v[0];
				center.y += v[1];
			}
			center.x = center.x / verts.size();
			center.y = center.y / verts.size();
		}
		else
			center = point;

		matrixf offset(3);
		for (size_t i = 0; i < 3; i++)
		{
			offset[i].resize(3);
			offset[i][i] = 1;
		}
		offset[2][0] = -center.x;
		offset[2][1] = -center.y;

		mtr[0][0] = cos(turning_val);
		mtr[0][1] = sin(turning_val);
		mtr[1][0] = -sin(turning_val);
		mtr[1][1] = cos(turning_val);

		mtr = prod(offset, mtr);

		offset[2][0] = center.x;
		offset[2][1] = center.y;

		mtr = prod(mtr, offset);

		apply(verts);

		reset_mtr();
	}



	void make_scaling(float scaling_val) {
		if (!polygons.empty())
			cur_poly = polygons.size() - 1;
		else
			return;

		vector<vector<float>> verts;
		for (auto x : polygons[cur_poly].v)
		{
			verts.push_back({ x.x, x.y, 1.0f });
		}
		
		ImVec2 center;
		if (around_center) {
			for (auto v : verts)
			{
				center.x += v[0];
				center.y += v[1];
			}
		}
		else
			center = point;

		matrixf offset(3);
		for (size_t i = 0; i < 3; i++)
		{
			offset.resize(3);
			offset[i][i] = 1;
		}
		offset[2][0] = -center.x;
		offset[2][1] = -center.y;

		mtr[0][0] = scaling_val;
		mtr[1][1] = scaling_val;

		mtr = prod(offset, mtr);

		offset[2][0] = center.x;
		offset[2][1] = center.y;

		mtr = prod(mtr, offset);

		apply(verts);

		reset_mtr();

	}

	void set_center_point(ImVec2 p) {
		point = p;
	}
};

#endif // !AFFINE_TRANSFORMATIONS_H
