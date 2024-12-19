#include "UltraEngine.h"
#include "Container.h"
#include "InnerContainer.h"

Container::Container() {
	//	classname = "Container";
}

Container::~Container() {

}

bool Container::KeyDown(const KeyCode key) {
	if (KEY_SHIFT == key) {
		isShift = true;
	}
	return CustomWidget::KeyDown(key);
}

void Container::KeyUp(const KeyCode key) {
	if (KEY_SHIFT == key) {
		isShift = false;
	}
	CustomWidget::KeyUp(key);
}

std::shared_ptr<Container> Container::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	struct Struct : public Container {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, style);
	return instance;
}

int getMoveValue(int scrollMovedValue, int innerLength, int viewLength, int maxSlider, int scrollButtonSize) {
	return -scrollMovedValue * Ceil((float)(innerLength - viewLength)
		/ (float)(maxSlider - scrollButtonSize));
}

bool Container::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	bool isInit = CustomWidget::Initialize("", x, y, width, height, parent, style);
	viewContainer = InnerContainer::create(Self()->As<Container>());
	innerContainer = InnerContainer::create(viewContainer);
	scrollSize = indent * 2;
	if (hasWidgetStyle(CONTAINER_SCROLL)) {
		verticalSlider = CustomSlider::create(width - scrollSize, 0, scrollSize, height, Self()->As<Container>(), CUSTOM_SLIDER_SCROLLBAR);
		verticalSlider->setMaxValue((float)verticalSlider->getScrollButtonMaxSize());
		verticalSlider->SetHidden(true);
		verticalSlider->doMouseScroll = false;
		verticalSlider->setListener([this](Event event) {
			moveContent(0, getMoveValue(event.data, innerContainer->getHeight(), viewContainer->getHeight(),
				verticalSlider->getScrollButtonMaxSize(), verticalSlider->getScrollButtonSize()));
			return true;
			});
		horizontalSlider = CustomSlider::create(0, height - scrollSize, width - scrollSize, scrollSize, Self()->As<Container>(), CUSTOM_SLIDER_HORIZONTAL | CUSTOM_SLIDER_SCROLLBAR);
		horizontalSlider->setMaxValue((float)horizontalSlider->getScrollButtonMaxSize());
		horizontalSlider->SetHidden(true);
		horizontalSlider->doMouseScroll = false;
		horizontalSlider->setListener([this](Event event) {
			moveContent(getMoveValue(event.data, innerContainer->getWidth(), viewContainer->getWidth(),
				horizontalSlider->getScrollButtonMaxSize(), horizontalSlider->getScrollButtonSize()), 0);
			return true;
			});

		auto controlsManager = ControlsManager::getInstance();
		controlsManager->addActionListener(ACTION_SCROLL_UP, Self(),
			[this](Event event) {
				if (this && !GetHidden() && isCursorInWidgetArea()) {
					if (!isShift) verticalSlider->previous();
					else horizontalSlider->previous();
				}
			});
		controlsManager->addActionListener(ACTION_SCROLL_DOWN, Self(),
			[this](Event event) {
				if (this && !GetHidden() && isCursorInWidgetArea()) {
					if (!isShift) verticalSlider->next();
					else horizontalSlider->next();
				}
			});
	}
	return isInit;
}

void Container::addWidget(shared_ptr<Widget> widget) {
	widget->SetParent(innerContainer);
	updateInnerContainerSize();
}

void Container::addWidgets(vector<shared_ptr<Widget>> widgets) {
	for (shared_ptr<Widget>& widget : widgets) {
		widget->SetParent(innerContainer);
	}
	updateInnerContainerSize();
}

void Container::removeWidget(shared_ptr<Widget> widget) {
	widget->SetParent(nullptr);
	widget = nullptr;
	updateInnerContainerSize();
}

void Container::clear() {
	while (innerContainer->kids.size() > 0) {
		removeWidget(innerContainer->kids[0]);
	}
	innerContainer->SetShape(iVec2(0, 0), iVec2(0, 0));
	updateInnerContainerSize();
}

vector<shared_ptr<Widget>> Container::getWidgets() {
	return innerContainer->kids;
}

int getScrollLength(int maxSlider, int viewLength, int innerLength, int scrollMoveValue) {
	int scrollButtonSize = Ceil((float)maxSlider
		* (float)viewLength / (float)innerLength);
	//int moveValue = -1 * getMoveValue(scrollMoveValue, innerLength, viewLength, maxSlider, scrollButtonSize);
	//return maxSlider - (float)(scrollMoveValue * (innerLength - viewLength)) / (float)moveValue;
	return scrollButtonSize;
}

void Container::updateInnerContainerSize() {
	int maxX = 0;
	int maxY = 0;
	for (shared_ptr<Widget> widget : innerContainer->kids) {;
		int newMaxX = widget->GetPosition().x + widget->GetSize().width;
		int newMaxY = widget->GetPosition().y + widget->GetSize().height;
		if (newMaxX > maxX) {
			maxX = newMaxX;
		}
		if (newMaxY > maxY) {
			maxY = newMaxY;
		}
	}
	maxX = Max(maxX, viewContainer->getWidth());
	maxY = Max(maxY, viewContainer->getHeight());
	innerContainer->setSize(maxX, maxY);
	if (hasWidgetStyle(CONTAINER_SCROLL)) {
		if (innerContainer->getHeight() > getHeight()) {
			viewContainer->setWidth(getWidth() - scrollSize);
			verticalSlider->setScrollLength(getScrollLength(verticalSlider->getScrollButtonMaxSize(),
				viewContainer->getHeight(), innerContainer->getHeight(), verticalSlider->getScrollMoveValue()));
			verticalSlider->SetHidden(false);
		} else {
			verticalSlider->SetHidden(true);
			verticalSlider->setCurrentValue(1, true);
			viewContainer->setWidth(getWidth());
			innerContainer->setPositionY(0);
		}
		if (innerContainer->getWidth() > getWidth()) {
			viewContainer->setHeight(getHeight() - scrollSize);
			horizontalSlider->setScrollLength(getScrollLength(horizontalSlider->getScrollButtonMaxSize(),
				viewContainer->getWidth(), innerContainer->getWidth(), verticalSlider->getScrollMoveValue()));
			horizontalSlider->SetHidden(false);
		} else {
			horizontalSlider->SetHidden(true);
			horizontalSlider->setCurrentValue(1, true);
			viewContainer->setHeight(getHeight());
			innerContainer->setPositionX(0);
		}
	}
}

void Container::moveContent(int deltaX, int deltaY) {
	int containerWidth = innerContainer->GetSize().width;
	int containerHeight = innerContainer->GetSize().height;
	int containerX = innerContainer->GetPosition().x;
	int containerY = innerContainer->GetPosition().y;
	int newPositionX = containerX + deltaX;
	int newPositionY = containerY + deltaY;
	if (newPositionX > 0) {
		newPositionX = 0;
	} else if ((newPositionX + containerWidth) < viewContainer->getWidth()) {
		newPositionX = viewContainer->getWidth() - containerWidth;
	}
	if (newPositionY >= 0) {
		newPositionY = 0;
	} else if (newPositionY < (viewContainer->getHeight() - containerHeight)) {
		newPositionY = viewContainer->getHeight() - containerHeight;
	}
	innerContainer->SetShape(newPositionX, newPositionY, containerWidth, containerHeight);
}

void Container::resetScrollPosition() {
	if (hasWidgetStyle(CONTAINER_SCROLL)) {
		verticalSlider->setCurrentValue(1, true);
		horizontalSlider->setCurrentValue(1, true);
	}
	innerContainer->setPosition(0, 0);
}

int Container::getElementCount() {
	return innerContainer->kids.size();
}

iVec2 Container::getInnerContainerPosition() {
	return innerContainer->GetPosition();
}

void Container::setInnerContainerSize(iVec2 size) {
	innerContainer->setSize(size);
}

void Container::SetShape(const iVec2& position, const iVec2& size) {
	SetShape(position.x, position.y, size.width, size.height);
}

void Container::SetShape(const int x, const int y, const int width, const int height) {
	bool wasSizeChanged = getHeight() != height || getWidth() != width;
	CustomWidget::SetShape(x, y, width, height);
	viewContainer->setSize(width, height);
	if (verticalSlider && wasSizeChanged) {
		verticalSlider->setHeight(height);
		verticalSlider->setMaxValue((float)verticalSlider->getScrollButtonMaxSize());
		verticalSlider->setCurrentValue(1);
		verticalSlider->setPositionX(width - scrollSize);
	}
	if (horizontalSlider && wasSizeChanged) {
		horizontalSlider->setWidth(height);
		horizontalSlider->setMaxValue((float)horizontalSlider->getScrollButtonMaxSize());
		horizontalSlider->setCurrentValue(1);
		horizontalSlider->setPositionY(height - scrollSize);
	}
}
