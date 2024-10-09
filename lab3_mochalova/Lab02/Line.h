#ifndef LINE_H
#define LINE_H

#include <vector>
#include<algorithm>
#include"includes.h"

using std::pair;
using std::vector;


class Line
{
public:
	int x1, x2;
	short c1[3], c2[3];
	Line(int x1, int x2, short c1[3], short c2[3]);
	Line();
	vector<pair<int, ImU32>> GetGradienLine(int y);
};

#endif // !LINE_H


