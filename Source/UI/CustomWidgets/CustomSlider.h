#pragma once
#include <UltraEngine.h>
#include "CustomWidget.h"
#include "TriangleButton.h"

using namespace UltraEngine;

enum CustomSliderStyle {
	CUSTOM_SLIDER_HORIZONTAL = 16,
	CUSTOM_SLIDER_SCROLLBAR = 32,
};

/*!
\brief Slider
\author Dreikblack
\date April of 2023
Slider for scrolling a content or selecting something in a list
*/
class CustomSlider : public CustomWidget {
protected:
	CustomSlider();

	shared_ptr <TriangleButton> firstTriangleButton;
	shared_ptr <TriangleButton> secondTriangleButton;
	std::function<bool(Event)> moveScrollListener;
	void createTriangleButton(bool isFirst);
	iVec2 getTrianglePosition(bool isFirst, int arrowLength);
	int getValueByPosition(int x, int y, bool isHorizontal);
	bool isPositionInScrollButton(int x, int y);
	iVec2 lastMousePosition = iVec2(-1, -1);

	iVec2 scrollPosition = iVec2(0, 0);
	iVec2 scrollSize = iVec2(5, 5);

	bool scrollButtonHidden = true;
	bool scrollGrabbedState = false;
	bool scrollHoverState = false;
	int currentValue = 0;
	int maxValue = 0;
	int scrollMoveValue = 1;

	virtual void MouseMove(const int x, const int y);
	virtual void MouseDown(const MouseButton button, const int x, const int y);
	virtual void MouseUp(const MouseButton button, const int x, const int y);
	virtual void MouseWheel(const int delta, const int x, const int y);
	virtual void MouseLeave(const int x, const int y);
	virtual void LoseFocus();
	virtual void Draw(const int x, const int y, const int width, const int height);
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style);
public:
	bool doMouseScroll;
	static std::shared_ptr<CustomSlider> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0);
	virtual void setHeight(int height);
	virtual void setWidth(int width);
	bool setCurrentValue(int value, bool isForced = false);
	int getCurrentValue();
	bool setMaxValue(int value);
	void setScrollLength(int breadth);
	int getScrollButtonMaxSize();
	int getScrollButtonSize();
	void previous();
	void next();
	void setScrollMoveValue(int scrollMoveValue);
	int getScrollMoveValue();
	void setListener(std::function<bool(Event)> listener);
};

