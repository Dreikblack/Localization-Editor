#include "UltraEngine.h"
#include "PixmapProgressBar.h"

PixmapProgressBar::PixmapProgressBar()
{
	reverse = false;
	showDigitValue = false;
}

std::shared_ptr<PixmapProgressBar> PixmapProgressBar::create(const int x, const int y, const int width, const int height, shared_ptr<UltraEngine::Widget> parent, bool reverse, bool showDigitValue)
{
	struct Struct : public PixmapProgressBar {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", x, y, width, height, parent, WIDGET_BORDER_ROUNDED);
	auto pixmap = CreatePixmap(width - 2, height - 2);
	instance->reverse = reverse;
	instance->showDigitValue = showDigitValue;
	instance->SetPixmap(pixmap);
	return instance;
}

void PixmapProgressBar::setProgress(int value, int maxValue) 
{
	if (PixmapProgressBar::value == value && PixmapProgressBar::maxValue == maxValue)
	{
		return;
	}
	pointsToUse = 0;
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
	PixmapProgressBar::value = value;
	PixmapProgressBar::maxValue = maxValue;
	if (maxValue == 0)
	{
		SetHidden(true);
		return;
	}
	else if (GetHidden())
	{
		SetHidden(false);
	}
	if (showDigitValue) {
		SetText(WString(value));
	}
	paintPixmap();
	Redraw();
}

void PixmapProgressBar::adjustPixmap(int width, int height)
{
	auto pixmap = GetPixmap();
	if (!pixmap)
	{
		SetPixmap(CreatePixmap(width - 2, height - 2));
	}
	else
	{
		SetPixmap(pixmap->Resize(width - 2, height - 2));
	}
}

void PixmapProgressBar::paintPixmap()
{
	int width = GetSize().width - 2;
	auto pixmap = GetPixmap();
	int height = GetSize().height - 2;
	int valueWidth;
	if (value < maxValue)
	{
		valueWidth = Floor((float) width * ((float) value / (float) maxValue));
	}
	else
	{
		valueWidth = width;
	}
	for (int column = !reverse ? 0 : width - valueWidth; column < (!reverse ? valueWidth : width); column++) {

		for (int row = 0; row < height; row++)
		{
			if ((column < 1 || column > (width - 2)) && (row < 3 || row > (height - 3)))
			{
				continue;
			}
			pixmap->WritePixel(column, row, getRgbaColor(WIDGETCOLOR_FOREGROUND));
		}
	}
	for (int column = reverse ? 0 : valueWidth; column < (reverse ? width - valueWidth : width); column++)
	{
		for (int row = 0; row < height; row++)
		{
			if ((column < 1 || column > (width - 2)) && (row < 2 || row > (height - 3)))
			{
				continue;
			}
			pixmap->WritePixel(column, row, getRgbaColor(WIDGETCOLOR_BACKGROUND));
		}
	}
	blinkColor = color[WIDGETCOLOR_FOREGROUND];
}

void PixmapProgressBar::Draw(const int x, const int y, const int width, const int height)
{
	if (pointsToUse > 0)
	{
		pointsToUse = pointsToUse <= value ? pointsToUse : value;

		int colorWidth = width;
		int colorX = 0;
		if (reverse)
		{
			colorX = size.width - Floor((float)size.width * (float)value / (float)maxValue);
			colorWidth = Floor((float)size.width * (float)pointsToUse / (float)maxValue);
		}
		else
		{
			colorX = Floor((float)size.width * (float)value / (float)maxValue) - Floor((float)size.width * (float)pointsToUse / (float)maxValue);
			colorWidth = Floor((float)size.width * (float)pointsToUse / (float)maxValue);
		}
		Vec4 newColor = GetColor(WIDGETCOLOR_FOREGROUND) * (pointsToUseBlinkIter / (float)pointsToUseBlinkIterMax);
		newColor.a = 1;
		unsigned int newBlinkColor = getRgbaColor(newColor);
		unsigned int oldBlinkColor = getRgbaColor(blinkColor);
		Paint::drawChangeColor(pixmap, oldBlinkColor, newBlinkColor, colorX, 0, colorWidth, height);
		blinkColor = newColor;
		pointsToUseNext();
	}
	CustomWidget::Draw(x, y, width, height);
}

void PixmapProgressBar::pointsToUseNext()
{
	if (pointsToUseBlinkIter >= pointsToUseBlinkIterMax)
	{
		pointsToUseBlinkIterForward = false;
	}
	else if (pointsToUseBlinkIter <= pointsToUseBlinkIterMin)
	{
		pointsToUseBlinkIterForward = true;
	}
	pointsToUseBlinkIter = pointsToUseBlinkIterForward ? pointsToUseBlinkIter + 1 : pointsToUseBlinkIter - 1;
}

void PixmapProgressBar::SetColor(const Vec4& newColor, const WidgetColor index, const int deltatime)
{
	CustomWidget::SetColor(newColor, index, deltatime);
	if (index == WIDGETCOLOR_FOREGROUND)
	{
		blinkColor = color[WIDGETCOLOR_FOREGROUND];
		Vec4 bgColor = newColor * 0.4f;
		bgColor.a = 1;
		CustomWidget::SetColor(bgColor, WIDGETCOLOR_BACKGROUND, deltatime);
	}
}

void PixmapProgressBar::SetShape(int x, int y, int width, int height)
{
	if (getWidth() != width || getHeight() != height)
	{
		adjustPixmap(width, height);
	}
	CustomWidget::SetShape(x, y, width, height);
}

void PixmapProgressBar::SetShape(const iVec2& position, const iVec2& size)
{
	if (GetSize() != size)
	{
		adjustPixmap(size.width, size.height);
	}
	CustomWidget::SetShape(position, size);
}

void PixmapProgressBar::setPointsToUse(int points)
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
	paintPixmap();
	Redraw();
}

void PixmapProgressBar::redrawPointsToUse()
{
	if (pointsToUse > 0) Redraw();
}
