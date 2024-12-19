#pragma once
#include "UltraEngine.h"
#include "CustomWidget.h"
#include "Paint.h"

class PixmapProgressBar : public CustomWidget
{
protected:
	bool reverse;
	bool showDigitValue;
	int value = 0;
	int maxValue = 0;
	bool pointsToUseBlinkIterForward = true;
	const int pointsToUseBlinkIterMax = GAME_FREQUENCY * 2.0f;
	const int pointsToUseBlinkIterMin = GAME_FREQUENCY;
	float pointsToUseBlinkIter = pointsToUseBlinkIterMin;
	int pointsToUse = 0;
	Vec4 blinkColor;
	virtual void adjustPixmap(int width, int height);
	virtual void paintPixmap();
	void pointsToUseNext();

	PixmapProgressBar();
	virtual void Draw(const int x, const int y, const int width, const int height);
public:
	static std::shared_ptr<PixmapProgressBar> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, bool reverse, bool showDigitValue = false);
	virtual void setProgress(int value, int maxValue);
	virtual void SetShape(int x, int y, int width, int height);
	virtual void SetShape(const iVec2& position, const iVec2& size);
	virtual void SetColor(const Vec4& color, const WidgetColor index = WIDGETCOLOR_BACKGROUND, const int deltatime = 0);
	void setPointsToUse(int points);
	void redrawPointsToUse();
};