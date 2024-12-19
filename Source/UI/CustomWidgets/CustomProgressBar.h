#pragma once
#include "UltraEngine.h"
#include "CustomWidget.h"
#include "../../Interface/AllConstants.h"

class CustomProgressBar : public CustomWidget
{
protected:
	CustomProgressBar();
	bool reverse;
	bool showDigitValue;
	int value = 0;
	int maxValue = 0;
	bool doDraw = true;
	int pointsToUse = 0;
	bool pointsToUseBlinkIterForward = true;
	const int pointsToUseBlinkIterMax = GAME_FREQUENCY * 3.0f;
	const int pointsToUseBlinkIterMin = GAME_FREQUENCY;
	float pointsToUseBlinkIter = pointsToUseBlinkIterMin;
	void pointsToUseNext();
	virtual void Draw(const int x, const int y, const int width, const int height);
public:
	static std::shared_ptr<CustomProgressBar> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = WIDGET_BACKGROUND | WIDGET_BORDER | WIDGET_BORDER_ROUNDED, bool reverse = false, bool showDigitValue = false);
	virtual void SetShape(const iVec2& position, const iVec2& size);
	virtual void SetShape(const int x, const int y, const int width, const int height);
	virtual void setProgress(float value, float maxValue);
	virtual void setProgress(float value);
	void setPointsToUse(int points);
};
