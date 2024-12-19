#include "UltraEngine.h"
#include "PixmapCircleProgressBar.h"
#include "Paint.h"

PixmapCircleProgressBar::PixmapCircleProgressBar()
{
	showDigitValue = false;
}

std::shared_ptr<PixmapCircleProgressBar> PixmapCircleProgressBar::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, boolean reverse)
{
	struct Struct : public PixmapCircleProgressBar {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", x, y, width, height, parent, PanelStyle::PANEL_DEFAULT);
	instance->SetColor(Rgba(0, 0, 0, 0));
	auto pixmap = CreatePixmap(width, height);
	instance->reverse = reverse;
	instance->SetPixmap(pixmap);
	return instance;
}


void PixmapCircleProgressBar::adjustPixmap(int width, int height)
{
	auto pixmap = GetPixmap();
	if (!pixmap)
	{
		SetPixmap(CreatePixmap(width, height));
	}
	else
	{
		SetPixmap(pixmap->Resize(width, height));
	}
}

void PixmapCircleProgressBar::paintPixmap()
{
	auto pixmap = GetPixmap();
	int radius = UltraEngine::Ceil(float(GetSize().height) / 2.0f) - 2;

	float width = GetSize().width;
	float height = GetSize().height;
	float fValue = value;
	float fMaxValue = maxValue;

	int centerY = height / 2;
	for (int i = 0; i < maxValue; i++) {
		unsigned int currentColor = (!reverse && i < value) || (reverse && i >= maxValue - value) ? getRgbaColor(WIDGETCOLOR_FOREGROUND) : getRgbaColor(WIDGETCOLOR_BACKGROUND);
		int centerX = ((i + 1) * width / fMaxValue) - (width / fMaxValue / 2);
		Paint::drawFilledCircle(pixmap, centerX, centerY, radius, currentColor);
	}
	blinkColor = color[WIDGETCOLOR_FOREGROUND];
}
