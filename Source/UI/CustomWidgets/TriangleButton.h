#pragma once

#include "UltraEngine.h"
#include "CustomButton.h"

enum TriangleDirection
{
	TRIANGLE_LEFT = 0,
	TRIANGLE_RIGHT = 1,
	TRIANGLE_TOP = 2,
	TRIANGLE_BOTTOM = 3
};
class TriangleButton : public CustomButton
{
protected:
	TriangleButton();
	unsigned int borderColor;
	unsigned int color;
public:
	static shared_ptr<TriangleButton> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, TriangleDirection direction, unsigned int color = 0, const float gradient = 1);
	void setColor(unsigned int color, const float gradient = 1);
};
