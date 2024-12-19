#pragma once

#include "UltraEngine.h"

using namespace UltraEngine;
/*!
	\brief Class for drawing in provided **Pixmap**
	\author Dreikblack
	\date December of 2022
*/
class Paint : public Object {
protected:
	Paint();
public:
	Paint(Paint const&) = delete;
	Paint& operator=(Paint const&) = delete;
	static void drawPixel(shared_ptr<Pixmap> pixmap, const int x, const int y, unsigned int color);
	static unsigned int readPixel(shared_ptr<Pixmap> pixmap, const int x, const int y);
	static void fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, unsigned int borderColor, const int x, const int y, const int width, const int height, float gradient = 1);
	static void fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, unsigned int borderColor, float gradient = 1);
	static void fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, const int x, const int y, const int width, const int height, float gradient = 1);
	static void fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, float gradient = 1);
	static void fillRectangle(shared_ptr<Pixmap> pixmap, const int x, const int y, const int width, const int height, unsigned int fillColor);
	static void drawLine(shared_ptr<Pixmap> pixmap, int x1, int y1, int x2, int y2, unsigned int color);
	static void drawLine(shared_ptr<Pixmap> pixmap, iVec2 point1, iVec2 point2, unsigned int color);
	static void drawShape(shared_ptr<Pixmap> pixmap, vector<iVec2> points, unsigned int color);
	static void drawFilledShape(shared_ptr<Pixmap> pixmap, vector<iVec2> points, unsigned int color);
	static void drawFilledShape(shared_ptr<Pixmap> pixmap, vector<iVec2> points, unsigned int color, unsigned int borderColor, float gradient = 1);
	static void drawChangeColor(shared_ptr<Pixmap> pixmap, unsigned int oldColor, unsigned int newColor);
	static void drawChangeColor(shared_ptr<Pixmap> pixmap, unsigned int oldColor, unsigned int newColor, const int x, const int y, const int width, const int height);
	static void drawCircle(shared_ptr<Pixmap> pixmap, const int centerX, const int centerY, const int radius, unsigned int color);
	static void drawFilledCircle(shared_ptr<Pixmap> pixmap, const int centerX, const int centerY, const int radius, unsigned int color);
	static void drawFilledCircle(shared_ptr<Pixmap> pixmap, const int centerX, const int centerY, const int radius, unsigned int color, unsigned int borderColor);
	static void drawFilledEllipse(shared_ptr<Pixmap> pixmap, const int centerX, const int centerY, const int radiusX, const int radiusY, unsigned int color, unsigned int borderColor);
	static void drawFilledRoundedRectangle(shared_ptr<Pixmap> pixmap, const int x, const int y, const int width, const int height, int radius, unsigned int color, unsigned int borderColor);
	static void drawCheckMark(shared_ptr<Pixmap> pixmap, const int x, const int y, const int width, const int height, unsigned int color);
	static void drawCheckMark(shared_ptr<Pixmap> pixmap, unsigned int color);
};

