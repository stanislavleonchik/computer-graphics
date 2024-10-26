#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include"../src/includes.h"

class BezierCurve {
private:
	vector<ImVec2> points; //опорные точки
	vector<array<ImVec2, 4>> curves;
	int focused_point;
	bool dragging;
	int n;
	int point_radius;
	ImColor common_color;
	ImColor focused_color; 
	ImVec2 size_menu_bar;
	float menu_h;

	int conversation(unsigned i) {
		if (i > n - i) {
			i = n - i;
		}

		unsigned long long result = 1;
		for (int k = 0; k < i; ++k) {
			result *= (n - k);
			result /= (k + 1);
		}
		return result;
	}

	double berzanstain_poly(unsigned i, double t) {
		return conversation(i) * pow(t, i) * pow(1 - t, n - i);
	}

	//версия для кривой старшей степени
	ImVec2 eval_curve_point(double t) {
		ImVec2 B(0, 0);
		
		for (size_t i = 0; i < points.size(); i++)
		{
			double b_poly = berzanstain_poly(i, t);
			B.x += points[i].x * b_poly;
			B.y += points[i].y * b_poly;
		}
		return B;
	}

	//функция собирает массив кривых по заданным пользователем точкам
	void points_correct() {

		curves.clear();
		if (points.size() == 1)
			return;
		if (points.size() == 4) {
			curves.push_back({ points[0], points[1], points[2], points[3] });
			return;
		}

		ImVec2 term_point_prev = points[0];
		int i;
		for (i = 1; i < (int)points.size() - 3; i += 2)
		{
			ImVec2 term_point_next((points[i + 1].x + points[i + 2].x) / 2, (points[i + 1].y + points[i + 2].y) / 2);
			curves.push_back({ term_point_prev, points[i], points[i + 1], term_point_next });
			term_point_prev = term_point_next;
		}
		i -= 1; // индекс последней прочитанной

		int count_not_readed_point = points.size() - i - 1;

		if (count_not_readed_point == 2) { //остались две точки
			i += 1;
			ImVec2 term_point((points[i].x + points[i + 1].x) / 2, (points[i].y + points[i + 1].y) / 2);
			curves.push_back({ term_point_prev, points[i], term_point, points[i + 1] });
		}
		else if (count_not_readed_point == 1) { //ситуация возможна только если points.size() == 2
			//ImVec2 term_point1((points[i].x + points[i + 1].x) / 3, (points[i].y + points[i + 1].y) / 3);
			//ImVec2 term_point2(2 * (points[i].x + points[i + 1].x) / 3, 2 * (points[i].y + points[i + 1].y) / 3);
			ImVec2 term_point1(
				points[i].x + (points[i + 1].x - points[i].x) / 3,
				points[i].y + (points[i + 1].y - points[i].y) / 3
			);
			ImVec2 term_point2(
				points[i].x + 2 * (points[i + 1].x - points[i].x) / 3,
				points[i].y + 2 * (points[i + 1].y - points[i].y) / 3
			);
			curves.push_back({ points[i], term_point1, term_point2, points[i + 1] }); //должна получится прямая линия
		}
		else if (count_not_readed_point == 3) {
			if (points.size() == 3) { //на самом деле как будто это должно попадать цикл
				ImVec2 term_point((points[1].x + points[2].x) / 2, (points[1].y + points[2].y) / 2);
				curves.push_back({ points[0], points[1], term_point, points[2] });
			}
			else {
				i += 1;
				curves.push_back({ term_point_prev, points[i], points[i + 1], points[i + 2] });
			}
		}

	}

public:

	BezierCurve() {
		points = vector<ImVec2>();
		focused_point = -1;
		dragging = false;
		n = 0;
		point_radius = 8;
		common_color = ImColor(0, 0, 255);
		focused_color = ImColor(255, 0, 0);
		menu_h = 23; //оно потом переопределяется если что
	}

	//версия для кривой старшей степени
	void draw_curve(float menu_h) {
		this->menu_h = menu_h;
		if (points.size() < 2)
			return;

		ImVec2 previous_point = eval_curve_point(0.0);
		ImDrawList* drawlist = ImGui::GetWindowDrawList();
		for (float i = 0.001; i <= 1; i += 0.001)
		{
			ImVec2 current_point = eval_curve_point(i);
			drawlist->AddLine(previous_point, current_point, ImColor(0, 0, 0), 2.0f);
			previous_point = current_point;
		}
	}

	//кубическая составная
	void draw_curves(float menu_h) {
		this->menu_h = menu_h;
		if (points.size() == 1)
			return;

		ImDrawList* drawlist = ImGui::GetWindowDrawList();
		
		for(auto curve: curves)
		{
			//t = 0
			ImVec2 previous_point = curve[0];
			for(float t = 0.001; t <= 1; t += 0.001)
			{
				ImVec2 current_point;
				current_point.x = curve[0].x * pow(1 - t, 3) + 3 * curve[1].x * pow(1 - t, 2) * t + 3 * curve[2].x * (1 - t) * t * t + curve[3].x * t * t * t;
				current_point.y = curve[0].y * pow(1 - t, 3) + 3 * curve[1].y * pow(1 - t, 2) * t + 3 * curve[2].y * (1 - t) * t * t + curve[3].y * t * t * t;
				drawlist->AddLine(previous_point, current_point, ImColor(0, 0, 0), 2.0f);
				previous_point = current_point;
			}
		}
	}

	void draw_points() {
		ImDrawList* drawlist = ImGui::GetWindowDrawList();
		

		for (size_t i = 0; i < points.size(); i++)
		{
			if (i != focused_point)
				drawlist->AddCircleFilled(points[i], point_radius, common_color);
			else
				drawlist->AddCircleFilled(points[i], point_radius, focused_color);
		}
	}

	void clear() {
		points.clear();
		curves.clear();
		focused_point = -1;
		dragging = false;
		n = 0;
	}

	//для отладки
	void draw_all_points() {
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		for (auto curve: curves)
		{
			drawList->AddCircleFilled(curve[0], point_radius, ImColor(0, 255, 0));
			drawList->AddCircleFilled(curve[1], point_radius, ImColor(0, 255, 0));
			drawList->AddCircleFilled(curve[2], point_radius, ImColor(0, 255, 0));
			drawList->AddCircleFilled(curve[3], point_radius, ImColor(0, 255, 0));
		}
	}

	void append_point(ImVec2 p) {
		p.y += menu_h + point_radius;
		p.x += point_radius;
		points.push_back(p);
		n = points.size() - 1;//для кривой старшей степени
		
		points_correct();
	}

	bool choosen_point(ImVec2 p) {
		p.y += menu_h + point_radius;
		p.x += point_radius;
		for (size_t i = 0; i < points.size(); i++)
		{
			if (!dragging && sqrt((p.x - points[i].x) * (p.x - points[i].x) +
				(p.y - points[i].y) * (p.y - points[i].y)) < point_radius) {
				focused_point = i;
				return true;
			}
		}
		return false;
	}

	void delete_point(ImVec2 p) {
		points.erase(points.begin() + focused_point);
		focused_point = -1;
		n = points.size()-1;//для кривой старшей степени
		points_correct();
	}

	void drag_point(ImVec2 new_pos) {
		new_pos.y += menu_h + point_radius;
		new_pos.x += point_radius;
		points[focused_point] = new_pos;
		focused_point = -1;
		points_correct();
	}
};

#endif // !BEZIERCURVE_H
