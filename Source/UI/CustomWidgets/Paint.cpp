#include "UltraEngine.h"
#include "Paint.h"

Paint::Paint()
{
}

struct StructPaint : public Paint {
};

//std::shared_ptr<Paint> Paint::getInstance() {
//	static std::shared_ptr<Paint> instance = std::make_shared<StructPaint>();
//	return instance;
//}

void Paint::drawPixel(shared_ptr<Pixmap> pixmap, const int x, const int y, unsigned int color)
{
	if (pixmap && x >= 0 && x < pixmap->size.width && y >= 0 && y < pixmap->size.height) {
		pixmap->WritePixel(x, y, color);
	}
}

unsigned int Paint::readPixel(shared_ptr<Pixmap> pixmap, const int x, const int y)
{
	if (pixmap && x >= 0 && x < pixmap->size.width && y >= 0 && y < pixmap->size.height) {
		return pixmap->ReadPixel(x, y);
	}
	return 0;
}


void Paint::fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, unsigned int borderColor, const int x, const int y, const int width, const int height, float gradient)
{
	if (!pixmap) {
		return;
	}
	int expectedMaxBorderThickness = 5;
	for (int h = y; h < y + height; h++) 
	{
		bool isFilling = false;
		bool hasPrevPixelColor = false;
		bool lastPixel = false;
		int thicknessCounter = 0;
		for (int w = x; w < x + width; w++) {
			bool hasCurrentPixelColor = readPixel(pixmap, w, h) == borderColor;
			if (isFilling && hasCurrentPixelColor) 
			{
				isFilling = false;
				int lastBorderThickness = 0;
				for (int w1 = w + 1; w1 < x + width; w1++) 
				{
					if (readPixel(pixmap, w1, h) == borderColor) 
					{
						if (!lastPixel && lastBorderThickness < expectedMaxBorderThickness)
						{
							lastPixel = true;
							lastBorderThickness++;
						}
						else 
						{
							lastPixel = false;
							break;
						}
					}
					else
					{
						lastBorderThickness = 0;
					}
				}
				continue;
			}
			else if (!isFilling && !hasCurrentPixelColor && (hasPrevPixelColor || lastPixel) && thicknessCounter < expectedMaxBorderThickness) 
			{
				for (int w1 = w; w1 < x + width; w1++) 
				{
					if (readPixel(pixmap, w1, h) == borderColor) 
					{
						isFilling = true;
						break;
					}
				}
			}
			if (isFilling) 
			{
				char r = Red(fillColor);
				char g = Green(fillColor);
				char b = Blue(fillColor);
				char a = Alpha(fillColor);
				r = float(r) * gradient + (r - float(r) * gradient) * (float(w) / (x + width));
				g = float(g) * gradient + (g - float(g) * gradient) * (float(w) / (x + width));
				b = float(b) * gradient + (b - float(b) * gradient) * (float(w) / (x + width));
				drawPixel(pixmap, w, h, Rgba(r,g,b,a));
			}
			if (isFilling)
			{
				hasPrevPixelColor = false;
				thicknessCounter = 0;
				continue;
			}
			if (!hasPrevPixelColor && hasCurrentPixelColor)
			{
				thicknessCounter = 1;
				hasPrevPixelColor = true;
			}
			else if (hasPrevPixelColor && hasCurrentPixelColor)
			{
				thicknessCounter++;
			}
			else {
				hasPrevPixelColor = false;
			}
		}
	}
}

void Paint::fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, unsigned int borderColor, float gradient)
{
	int width = pixmap->size.width;
	int height = pixmap->size.height;
	fillShape(pixmap, fillColor, borderColor, 0, 0, width, height, gradient);
}

void Paint::fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, const int x, const int y, const int width, const int height, float gradient)
{
	fillShape(pixmap, fillColor, fillColor, x, y, width, height, gradient);
}

void Paint::fillShape(shared_ptr<Pixmap> pixmap, unsigned int fillColor, float gradient)
{
	int width = pixmap->size.width;
	int height = pixmap->size.height;
	fillShape(pixmap, fillColor, fillColor, 0, 0, width, height, gradient);
}

void Paint::fillRectangle(shared_ptr<Pixmap> pixmap, const int x, const int y, const int width, const int height, unsigned int fillColor)
{
	for (int i = x; i < width; i++)
		for (int j = y; j < height; j++)
			drawPixel(pixmap, i, j, fillColor);
}

void Paint::drawLine(shared_ptr<Pixmap> pixmap, int x1, int y1, int x2, int y2, unsigned int color)
{
	const int deltaX = abs(x2 - x1);
	const int deltaY = abs(y2 - y1);
	const int signX = x1 < x2 ? 1 : -1;
	const int signY = y1 < y2 ? 1 : -1;
	int error = deltaX - deltaY;
	drawPixel(pixmap, x2, y2, color);
	while (x1 != x2 || y1 != y2)
	{
		drawPixel(pixmap, x1, y1, color);
		int error2 = error * 2;
		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}
		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}
}

void Paint::drawLine(shared_ptr<Pixmap> pixmap, iVec2 point1, iVec2 point2, unsigned int color)
{
	drawLine(pixmap, point1.x, point1.y, point2.x, point2.y, color);
}

void Paint::drawShape(shared_ptr<Pixmap> pixmap, vector<iVec2> points, unsigned int color) {
	if (!pixmap || points.size() < 3) {
		return;
	}
	for (int i = 1; i < points.size(); i++) {
		int prevPointIndex = i - 1;
		drawLine(pixmap, points[i].x, points[i].y, points[prevPointIndex].x, points[prevPointIndex].y, color);
	}
	int finalPointIndex = points.size() - 1;
	drawLine(pixmap, points[0].x, points[0].y, points[finalPointIndex].x, points[finalPointIndex].y, color);
}

void Paint::drawFilledShape(shared_ptr<Pixmap> pixmap, vector<iVec2> points, unsigned int color) {
	drawShape(pixmap, points, color);
	fillShape(pixmap, color);
}

void Paint::drawFilledShape(shared_ptr<Pixmap> pixmap, vector<iVec2> points, unsigned int color, unsigned int borderColor, float gradient) {
	drawShape(pixmap, points, borderColor);
	fillShape(pixmap, color, borderColor, gradient);
}

void Paint::drawChangeColor(shared_ptr<Pixmap> pixmap, unsigned int oldColor, unsigned int newColor)
{
	if (!pixmap) {
		return;
	}
	drawChangeColor(pixmap, oldColor, newColor, 0, 0, pixmap->size.width, pixmap->size.width);
}

void Paint::drawChangeColor(shared_ptr<Pixmap> pixmap, unsigned int oldColor, unsigned int newColor, const int x, const int y, const int width, const int height)
{
	if (!pixmap) return;
	for (int h = y; h < y + height; h++)
	{
		for (int w = x; w < x + width; w++) 
		{
			if (readPixel(pixmap, w, h) == oldColor) 
			{
				drawPixel(pixmap, w, h, newColor);
			}
		}
	}
}

void Paint::drawCircle(shared_ptr<Pixmap> pixmap, const int centerX, const int centerY, const int radius, unsigned int color) {
	int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while (y >= 0) {
		drawPixel(pixmap, centerX + x, centerY + y, color);
		drawPixel(pixmap, centerX + x, centerY - y, color);
		drawPixel(pixmap, centerX - x, centerY + y, color);
		drawPixel(pixmap, centerX - x, centerY - y, color);
		error = 2 * (delta + y) - 1;
		if (delta < 0 && error <= 0) {
			++x;
			delta += 2 * x + 1;
			continue;
		}
		error = 2 * (delta - x) - 1;
		if (delta > 0 && error > 0) {
			--y;
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
	}
}

void Paint::drawFilledCircle(shared_ptr<Pixmap> pixmap, const int centerX, const int centerY, const int radius, unsigned int color)
{
	drawFilledCircle(pixmap, centerX, centerY, radius, color, color);
}

void Paint::drawFilledCircle(shared_ptr<Pixmap> pixmap, const int centerX, const int centerY, const int radius, unsigned int color, unsigned int borderColor)
{
	drawCircle(pixmap, centerX, centerY, radius, borderColor);
	Paint::fillShape(pixmap, color, borderColor, centerX - radius - 1, centerY - radius - 1, (radius + 2) * 2, (radius + 2) * 2);
}

void Paint::drawFilledEllipse(shared_ptr<Pixmap> pixmap, const int cx, const int cy, const int a, const int b, unsigned int color, unsigned int borderColor)
{
	int x0 = cx;
	int y0 = cy + b;
	int a2 = a * a;
	int b2 = b * b;
	int d = 4 * b2 - 4 * a2 * b + a2;

	drawPixel(pixmap, x0, y0, borderColor);

	while (2 * b2 * (x0 + 1) < 2 * a2 * (y0 - 0.5))
	{
		if (d < 0)
		{
			d += 4 * b2 * (2 * x0 + 3);
		}
		else
		{
			d += 4 * b2 * (2 * x0 + 3) - 8 * a2 * (y0 - 1);
			y0--;
		}
		x0++;
		drawPixel(pixmap, x0, y0, borderColor);
		drawPixel(pixmap, x0, 2 * cy - y0, borderColor);
		drawPixel(pixmap, 2 * cx - x0, y0, borderColor);
		drawPixel(pixmap, 2 * cx - x0, 2 * cy - y0, borderColor);
	}

	d = b2 * (2 * x0 + 1) * (2 * x0 + 1) + 4 * a2 * (y0 - 1) * (y0 - 1) - 4 * a2 * b2;

	while (y0 > cy)
	{
		if (d < 0)
		{
			d += 4 * b2 * (2 * x0 + 2) - 8 * a2 * (y0 - 1);
			x0++;
		}
		else
		{
			d += -8 * a2 * (y0 - 1);
		}
		y0--;
		drawPixel(pixmap, x0, y0, borderColor);
		drawPixel(pixmap, x0, 2 * cy - y0, borderColor);
		drawPixel(pixmap, 2 * cx - x0, y0, borderColor);
		drawPixel(pixmap, 2 * cx - x0, 2 * cy - y0, borderColor);
	}
	fillShape(pixmap, color, borderColor);
}

//TODO this function
void Paint::drawFilledRoundedRectangle(shared_ptr<Pixmap> pixmap, const int x, const int y, const int width, const int height, int radius, unsigned int color, unsigned int borderColor)
{
	//int radius = width * 0.1;
	//drawArc(pixmap, radius, 0, radius, height - 1, 0, height / 2 - 1, radius, borderColor);
	//drawArc(pixmap, width - radius, 0, width - radius, height - 1, width - 1, height / 2 - 1, radius, borderColor);
	//drawLine(pixmap, radius, 0, width - radius, 0, borderColor);
	//drawLine(pixmap, radius, height - 1, width - radius, height - 1, borderColor);	
	//fillShape(pixmap, color, borderColor);
}

void Paint::drawCheckMark(shared_ptr<Pixmap> pixmap, const int x, const int y, const int width, const int height, unsigned int color)
{
	int halfY = Round((float)height / 2.0f) + y;
	int quarterY = Round((float)height / 4.0f) + y ;
	int thirdX = Round((float)width / 3.0f) + x;
	int quarterWidth = Round((float)width / 4.0f);
	auto point0 = iVec2(x, halfY);
	auto point1 = iVec2(x + quarterWidth, halfY);
	auto point2 = iVec2(thirdX, height - quarterY - 1);
	auto point3 = iVec2(width + x - quarterWidth - 1, y + 1);
	auto point4 = iVec2(width + x - 1, y + 1);
	auto point5 = iVec2(thirdX, y + height - 2);
	//drawFilledShape(pixmap, { point0, point1, point2, point3, point4, point5 },  color);
	drawFilledShape(pixmap, { point0, point1, point2, point3, point4, point5 }, color);
	if (!pixmap) {
		return;
	}
}

void Paint::drawCheckMark(shared_ptr<Pixmap> pixmap, unsigned int color)
{
	if (!pixmap) {
		return;
	}
	drawCheckMark(pixmap, 0, 0, pixmap->size.width, pixmap->size.height, color);
}
