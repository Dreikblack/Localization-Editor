#include "UltraEngine.h"
#include "CustomSlider.h"

CustomSlider::CustomSlider() {
	blocks.resize(5);//background, image, border, text, sliderButton
	doMouseScroll = true;
}

std::shared_ptr<CustomSlider> CustomSlider::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	struct Struct : public CustomSlider {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, WIDGET_BACKGROUND | WIDGET_BORDER | style);
	return instance;
}

void CustomSlider::setHeight(int height) {
	CustomWidget::setHeight(height);
	if (!hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL)) {
		secondTriangleButton->setPosition(getTrianglePosition(false, getWidth()));
	}
}

void CustomSlider::setWidth(int width) {
	CustomWidget::setWidth(width);
	if (hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL)) {
		secondTriangleButton->setPosition(getTrianglePosition(false, getHeight()));
	}
}

void CustomSlider::createTriangleButton(bool isFirst) {
	bool isVertical = !hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL);
	int width = GetSize().width;
	int height = GetSize().height;
	int buttonWidth = indent * 3;
	int buttonHeight = indent * 3;
	TriangleDirection direction = TRIANGLE_LEFT;
	if (isVertical) {
		if (isFirst) {
			direction = TRIANGLE_TOP;
		} else {
			direction = TRIANGLE_BOTTOM;
		}
		buttonWidth = width;
		buttonHeight = width;
	} else if (!isVertical && !isFirst) {
		direction = TRIANGLE_RIGHT;
		buttonWidth = height;
		buttonHeight = height;
	} else {
		buttonWidth = height;
		buttonHeight = height;
	}
	unsigned int triangleColor = getRgbaColor(WIDGETCOLOR_FOREGROUND);
	iVec2 triangleButtonPosition = getTrianglePosition(isFirst, isVertical ? buttonHeight : buttonWidth);
	auto button = TriangleButton::create(triangleButtonPosition.x, triangleButtonPosition.y, buttonWidth, buttonHeight, Self()->As<CustomSlider>(), direction, triangleColor, 0.8f);
	if (isFirst) {
		button->setListener(
			[this](Event event) {
				previous();
				return true;
			});
		firstTriangleButton = button;
	} else {
		button->setListener(
			[this](Event event) {
				next();
				return true;
			});
		secondTriangleButton = button;
	}
}

iVec2 CustomSlider::getTrianglePosition(bool isFirst, int arrowLength) {
	int x = 0, y = 0;
	bool isVertical = !hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL);
	if (isVertical) {
		if (!isFirst) {
			y = GetSize().height - arrowLength;
		}
	} else {
		if (!isFirst) {
			x = GetSize().width - arrowLength;
		}
	}
	return iVec2(x, y);
}

int CustomSlider::getValueByPosition(int x, int y, bool isHorizontal) {
	int value = currentValue;
	if (!hasWidgetStyle(CUSTOM_SLIDER_SCROLLBAR)) {
		float position = isHorizontal ? x : y;
		float triangleSize = isHorizontal ? firstTriangleButton->getWidth() : firstTriangleButton->getHeight();
		float widgetSize = isHorizontal ? getWidth() : getHeight();
		value = ceil((position - triangleSize - 1) / getScrollButtonMaxSize() * (float)maxValue);
	} else if (lastMousePosition.x != -1) {
		value = isHorizontal ? (currentValue + x - lastMousePosition.x) : (currentValue + y - lastMousePosition.y);
	}
	if (value < 1) {
		value = 1;
	}
	return value;
}

bool CustomSlider::setCurrentValue(int value, bool isForced) {
	if (GetInterface()->GetHidden() || (!isForced && (GetHidden() || GetDisabled() || !GetInteractive()))) return false;
	if (value < 1) {
		value = 1;
	} else if (value > maxValue) {
		value = maxValue;
	}
	int oldValue = currentValue;
	currentValue = value;
	scrollButtonHidden = false;
	bool isVertical = !hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL);
	bool isScrollBar = hasWidgetStyle(CUSTOM_SLIDER_SCROLLBAR);
	if (isVertical) {
		scrollPosition.y = firstTriangleButton->getHeight() + (value - 1) * (!isScrollBar ? (scrollSize.height) : 1);
		if (scrollPosition.y > getHeight() - firstTriangleButton->getHeight() - scrollSize.height) {
			scrollPosition.y = (getHeight() - firstTriangleButton->getHeight() - scrollSize.height);
			currentValue = maxValue - (isScrollBar ? scrollSize.height : 0);
		}
	} else {
		scrollPosition.x = firstTriangleButton->getWidth() + (value - 1) * (!isScrollBar ? (scrollSize.width) : 1);
		if (scrollPosition.x > getWidth() - firstTriangleButton->getWidth() - scrollSize.width) {
			scrollPosition.x = (getWidth() - firstTriangleButton->getWidth() - scrollSize.width);
			currentValue = maxValue - (isScrollBar ? scrollSize.width : 0);
		}
	}
	if (moveScrollListener) {
		moveScrollListener(Event(EVENT_WIDGETACTION, Self(), currentValue - oldValue));
	}
	Redraw();
	return true;
}

int CustomSlider::getCurrentValue() {
	return currentValue;
}

bool CustomSlider::setMaxValue(int value) {
	if (value > 0) {
		if (currentValue == 0) {
			currentValue = 1;
		}
		maxValue = value;
		bool isScrollbar = hasWidgetStyle(CUSTOM_SLIDER_SCROLLBAR);
		if (!isScrollbar) {
			setScrollLength(Round((float)getScrollButtonMaxSize() / (float)maxValue));
		}
		setCurrentValue(currentValue);
		int pushDelay = 2000 / maxValue;
		firstTriangleButton->timerDelay = pushDelay;
		secondTriangleButton->timerDelay = pushDelay;
		Redraw();
		return true;
	}
	return false;
}

void CustomSlider::setScrollLength(int length) {
	if (length < indent) {
		length = indent;
	}
	bool isBigger = false;
	if (!hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL)) {
		isBigger = length > scrollSize.height;
		scrollSize.height = length;
	} else {
		isBigger = length > scrollSize.width;
		scrollSize.width = length;
	}
	if (isBigger) {
		int oldValue = currentValue;
		setCurrentValue(1);
		setCurrentValue(oldValue);
	}
}

int CustomSlider::getScrollButtonMaxSize() {
	if (!hasWidgetStyle(style, CUSTOM_SLIDER_HORIZONTAL)) {
		return getHeight() - firstTriangleButton->getHeight() * 2 + 2;
	} else {
		return getWidth() - firstTriangleButton->getWidth() * 2 + 2;
	}
}

int CustomSlider::getScrollButtonSize() {
	if (!hasWidgetStyle(style, CUSTOM_SLIDER_HORIZONTAL)) {
		return scrollSize.height;
	} else {
		return scrollSize.width;
	}
}

void CustomSlider::previous() {
	setCurrentValue(currentValue - scrollMoveValue);
}

void CustomSlider::next() {
	setCurrentValue(currentValue + scrollMoveValue);
}

void CustomSlider::setScrollMoveValue(int scrollMoveValue) {
	CustomSlider::scrollMoveValue = scrollMoveValue;
}

int CustomSlider::getScrollMoveValue() {
	return scrollMoveValue;
}

void CustomSlider::setListener(std::function<bool(Event)> listener) {
	moveScrollListener = listener;
}

bool CustomSlider::isPositionInScrollButton(int x, int y) {
	int scrollX = scrollPosition.x;
	int scrollY = scrollPosition.y;
	return x >= scrollX && x < scrollX + scrollSize.width && y >= scrollY && y < scrollY + scrollSize.height;
}

void CustomSlider::MouseMove(const int x, const int y) {
	bool positionInScrollButton = isPositionInScrollButton(x, y);
	if (!scrollHoverState && positionInScrollButton) {
		scrollHoverState = true;
		Redraw();
	} else if (scrollHoverState && !positionInScrollButton) {
		scrollHoverState = false;
		Redraw();
	}
	bool isHorizontal = hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL);
	if (scrollGrabbedState) {
		setCurrentValue(getValueByPosition(x, y, isHorizontal));
		lastMousePosition.x = x;
		lastMousePosition.y = y;
	}
}

void CustomSlider::MouseDown(const MouseButton button, const int x, const int y) {
	if (x < 0 || y < 0 || x > getWidth() || y > getHeight()) return;
	if (button == MOUSE_LEFT) {
		if (!scrollGrabbedState && scrollHoverState && isPositionInScrollButton(x, y)) {
			lastMousePosition.x = x;
			lastMousePosition.y = y;
			scrollGrabbedState = true;
		}
	}
	Redraw();
}

void CustomSlider::MouseUp(const MouseButton button, const int x, const int y) {
	if (x < 0 || y < 0 || x > getWidth() || y > getHeight()) {
		scrollGrabbedState = false;
		Redraw();
		return;
	}
	if (button == MOUSE_LEFT) {
		if (!scrollGrabbedState) {
			if (!hasWidgetStyle(CUSTOM_SLIDER_SCROLLBAR)) {
				setCurrentValue(getValueByPosition(x, y, hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL)));
			} else {
				if (hasWidgetStyle(CUSTOM_SLIDER_HORIZONTAL)) {
					setCurrentValue((currentValue + scrollMoveValue * 5 * (x > (currentValue + scrollSize.width) ? 1 : -1)));
				} else {
					setCurrentValue(currentValue + scrollMoveValue * 5 * (y > (currentValue + scrollSize.height) ? 1 : -1));
				}
			}
		}
		scrollGrabbedState = false;
		lastMousePosition.x = -1;
		lastMousePosition.y = -1;
	}
	Redraw();
}

void CustomSlider::MouseWheel(const int delta, const int x, const int y) {
	if (GetHidden() || !doMouseScroll) {
		return;
	}
	if (delta > 0) {
		next();
	} else {
		previous();
	}
}

void CustomSlider::MouseLeave(const int x, const int y) {
	if (scrollHoverState || scrollGrabbedState) {
		scrollHoverState = false;
		//scrollPushState = false;
	}
	Redraw();
}


bool CustomSlider::Initialize(const int x, const int y, const int width_, const int height_, shared_ptr<Widget> parent, const int style) {
	indent = indent / 2;
	bool isVertical = !hasWidgetStyle(style, CUSTOM_SLIDER_HORIZONTAL);
	int width = 10, height = 10;
	if (isVertical) {
		width = width_ > indent * 3 ? width_ : indent * 3;
		height = height_ > indent * 6 ? height_ : indent * 6;
	} else {
		width = width_ > indent * 6 ? width_ : indent * 6;
		height = height_ > indent * 3 ? height_ : indent * 3;
	}
	CustomWidget::Initialize("", x, y, width, height, parent, style);
	createTriangleButton(true);
	createTriangleButton(false);
	int scrollX = 1, scrollY = 1;
	if (isVertical) {
		scrollY = firstTriangleButton->GetSize().y + 1;
		scrollSize.width = getWidth() - 2;
	} else {
		scrollX = firstTriangleButton->GetSize().x + 1;
		scrollSize.height = getHeight() - 2;
	}
	scrollPosition.x = scrollX;
	scrollPosition.y = scrollY;
	setScrollMoveValue(hasWidgetStyle(CUSTOM_SLIDER_SCROLLBAR) ? indent / 4 : 1);
	return true;
}

void CustomSlider::LoseFocus() {
	scrollHoverState = false;
	//scrollPushState = false;
	Redraw();
}

void CustomSlider::Draw(const int x, const int y, const int width, const int height) {
	CustomWidget::Draw(x, y, width, height);
	int blockId = AddBlock(iVec2(0), size, color[WIDGETCOLOR_SLIDERTRACK]);
	blocks[blockId].gradient = 0.75f;
	if (!scrollButtonHidden) {
		//blocks[blockId].color = color[WIDGETCOLOR_FOREGROUND];
		blocks[blockId].position = scrollPosition;
		blocks[blockId].size = scrollSize;

		if (scrollGrabbedState) {
			blocks[blockId].color = color[WIDGETCOLOR_SLIDERTRACK] * 0.75f;
			blocks[blockId].gradient = 1.0f / 0.75f;
		} else if (scrollHoverState) {
			blocks[blockId].color += 0.05;
		}

		blocks[blockId].hidden = false;
	} else {
		blocks[blockId].hidden = true;
	}
}
