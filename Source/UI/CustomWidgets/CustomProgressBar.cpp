#include "UltraEngine.h"
#include "CustomProgressBar.h"

CustomProgressBar::CustomProgressBar()
{
	reverse = false;
	showDigitValue = false;
	cornerradius = 6;
	blocks.resize(5);//Background, Border, Progress, PointsToUse, Text, 
}

std::shared_ptr<CustomProgressBar> CustomProgressBar::create(const int x, const int y, const int width, const int height, shared_ptr<UltraEngine::Widget> parent, const int style, bool reverse, bool showDigitValue)
{
	struct Struct : public CustomProgressBar {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", x, y, width, height, parent, style);
	instance->reverse = reverse;
	instance->showDigitValue = showDigitValue;
	return instance;
}

void CustomProgressBar::Draw(const int x, const int y, const int width, const int height)
{
	if (!doDraw)
	{
		return;
	}
	for (auto& block : blocks)
	{
		block.hidden = true;
	}
	// Background
	if (hasWidgetStyle(WIDGET_BACKGROUND)) {
		blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
		blocks[0].wireframe = false;
		blocks[0].position = iVec2(0);
		blocks[0].size = size;
		blocks[0].radius = (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0;
		blocks[0].hidden = false;
	}

	// Border
	if (hasWidgetStyle(WIDGET_BORDER)) {
		blocks[1].hidden = false;
		blocks[1].color = color[WIDGETCOLOR_BORDER];
		blocks[1].wireframe = true;
		blocks[1].position = iVec2(0);
		blocks[1].size = size;
		blocks[1].radius = (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0;
	}
	// Progress
	blocks[2].hidden = false;
	blocks[2].color = color[WIDGETCOLOR_FOREGROUND];
	blocks[2].wireframe = false;
	auto progressPosition = iVec2(0);
	auto progressSize = iVec2(0, size.height);
	if (reverse)
	{
		progressSize.width = Floor((float)size.width * (float)(value - pointsToUse) / (float)maxValue);
		progressPosition.x = size.width - progressSize.width;
	}
	else 
	{
		progressPosition.x = 0;
		progressSize.width = Floor((float)size.width * value / maxValue) - Floor((float)size.width * pointsToUse / maxValue);
	}
	blocks[2].position = progressPosition;
	blocks[2].size = progressSize;
	int radius = hasWidgetStyle(WIDGET_BORDER_ROUNDED) ? cornerradius : 0;
	radius = progressSize.width < progressSize.height ? radius / 1.5f : radius;
	blocks[2].radius = radius;
	blocks[2].gradient = 0.5f;
	// health/armor to waste
	if (pointsToUse > 0 && value > 0)
	{
		blocks[3].hidden = false;
		blocks[3].color = color[WIDGETCOLOR_FOREGROUND] * (0.4f + pointsToUseBlinkIter / (float)pointsToUseBlinkIterMax);
		blocks[3].wireframe = false;
		auto progressPosition = iVec2(0);
		auto progressSize = iVec2(0, size.height);
		if (reverse)
		{
			progressPosition.x = size.width - Floor((float)size.width * (float) value / (float)maxValue);
			progressSize.width = Floor((float)size.width * (float)pointsToUse / (float)maxValue);
		}
		else
		{
			progressSize.width = Floor((float)size.width * pointsToUse / maxValue);
			progressPosition.x = blocks[2].size.width;
		}
		blocks[3].position = progressPosition;
		blocks[3].size = progressSize;
		int radius = hasWidgetStyle(WIDGET_BORDER_ROUNDED) ? cornerradius : 0;
		radius = progressSize.width < progressSize.height ? radius / 1.5f : radius;
		blocks[3].radius = radius;
		blocks[3].gradient = 0.5f;
	}

	

	//Text
	blocks[4].hidden = false;
	blocks[4].position = iVec2(0, 0);
	blocks[4].size = size;
	blocks[4].SetText(text);
	blocks[4].textalignment = hasWidgetStyle(WIDGET_TEXT_LEFT) ? (textAlignment - TEXT_CENTER) : textAlignment;
	blocks[4].color = color[WIDGETCOLOR_SELECTEDTEXT];
	pointsToUseNext();
}

void CustomProgressBar::pointsToUseNext()
{
	if (pointsToUseBlinkIter >= pointsToUseBlinkIterMax)
	{
		pointsToUseBlinkIterForward = false;
	}
	else if (pointsToUseBlinkIter <= pointsToUseBlinkIterMin)
	{
		pointsToUseBlinkIterForward = true;
	}
	if (pointsToUse == 0)
	{
		doDraw = false;
	}
	else
	{
		pointsToUseBlinkIter = pointsToUseBlinkIterForward ? pointsToUseBlinkIter + 1 : pointsToUseBlinkIter - 1;
	}
}


void CustomProgressBar::setProgress(float value, float maxValue) 
{
	if (CustomProgressBar::value == value && CustomProgressBar::maxValue == maxValue)
	{
		return;
	}
	if (value < 0)
	{
		value = 0;
	} 
	else if (value > maxValue)
	{
		value = maxValue;
	}
	if (maxValue < 0)
	{
		maxValue = 0;
	}
	if (showDigitValue) 
	{
		//SetText(WString(value) + WString('/') + WString(maxValue));
		SetText(WString(value));
	}
	CustomProgressBar::value = value;
	CustomProgressBar::maxValue = maxValue;
	if (maxValue == 0)
	{
		SetHidden(true);
		return;
	}
	else if (GetHidden())
	{
		SetHidden(false);
	}
	doDraw = true;
	Redraw();
}

void CustomProgressBar::setProgress(float value)
{
	setProgress(value, maxValue);
}

void CustomProgressBar::setPointsToUse(int points)
{
	if (pointsToUse == points)
	{
		return;
	}
	if (points < 0)
	{
		pointsToUse = 0;
	}
	else if (points > value)
	{
		pointsToUse = value;
	}
	else
	{
		pointsToUse = points;
	}
	doDraw = true;
	Redraw();
}


void CustomProgressBar::SetShape(const iVec2& position, const iVec2& size)
{
	if (GetSize() != size)
	{
		doDraw = true;
		cornerradius = Ceil(0.17f * (float)size.height);
	}
	CustomWidget::SetShape(position, size);
}

void CustomProgressBar::SetShape(const int x, const int y, const int width, const int height)
{
	if (getWidth() != width || getHeight() != height)
	{
		doDraw = true;
		cornerradius = Ceil(0.4f * (float)height);
	}
	CustomWidget::SetShape(x, y, width, height);
}
