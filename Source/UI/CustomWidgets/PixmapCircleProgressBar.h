#pragma once
#include "UltraEngine.h"
#include "PixmapProgressBar.h"

class PixmapCircleProgressBar : public PixmapProgressBar
{
protected:
	PixmapCircleProgressBar();
	virtual void adjustPixmap(int width, int height);
	virtual void paintPixmap();
public:
	static std::shared_ptr<PixmapCircleProgressBar> create(const int x, const int y, const int width, const int height, shared_ptr<UltraEngine::Widget> parent, boolean reverse = false);
};
