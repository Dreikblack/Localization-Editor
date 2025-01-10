#include "UltraEngine.h"
#include "ButtonPanel.h"
#include "TriangleButton.h"

ButtonPanel::ButtonPanel() {
	//classname = "ButtonPanel";
}

shared_ptr<ButtonPanel> ButtonPanel::create(const int x, const int y, const int maxLength, const int buttonWidth, const int buttonHeight, shared_ptr<Widget> parent, const int style, unsigned int color) {
	struct Struct : public ButtonPanel {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, maxLength, buttonWidth, buttonHeight, parent, color, style);
	return instance;
}

bool ButtonPanel::Initialize(const int x, const int y, const int maxLength, int buttonWidth, int buttonHeight, shared_ptr<Widget> parent, unsigned int color, const int style) {
	ButtonPanel::buttonWidth = buttonWidth;
	ButtonPanel::buttonHeight = buttonHeight;
	bool isVertical = hasWidgetStyle(style, BUTTON_PANEL_VERTICAL);
	bool isCenter = hasWidgetStyle(style, BUTTON_PANEL_CENTER);
	int buttonLength = getButtonLength();
	ButtonPanel::maxLength = maxLength >= buttonLength ? maxLength : buttonLength;
	CustomWidget::Initialize("", x, y, isVertical ? buttonWidth + 1 : maxLength, isVertical ? maxLength : buttonHeight + 1, parent, style);
	if (hasWidgetStyle(BUTTON_PANEL_NO_INDENT)) indent = 0;
	triangleColor = color;
	createTriangleButton();
	createTriangleButton(false);
	iVec2 pos = getTrianglePosition(true);
	int halfSize = Floor(float(isVertical ? buttonHeight : buttonWidth) / 2.0f);
	container = Container::create(isVertical ? 0 : pos.x + halfSize + indent, isVertical ? 0 : pos.y + halfSize + indent, 0, 0, Self()->As<Widget>());
	if (hasWidgetStyle(BUTTON_PANEL_AGILE_ARROWS)) {
		firstTriangleButton->SetHidden(true);
		secondTriangleButton->SetHidden(true);
	}
	return true;
}

int ButtonPanel::getContainerLength() {
	int length = 0;
	for (auto& button : buttonList) {
		if (button->GetHidden()) {
			continue;
		}
		length = length + getButtonLength(button) + indent;
	}
	length = length - (length != 0 ? indent : 0);
	//return length < (maxLength - getButtonLength() - 2 * indent) ? length : (maxLength - getButtonLength() - 2 * indent);
	return length < maxLength ? length : maxLength - getButtonLength() - 2 * indent;
}

iVec2 ButtonPanel::getTrianglePosition(bool isFirst) {
	int x = 0, y = 0;
	int width = GetSize().width;
	int height = GetSize().height;
	bool isVertical = hasWidgetStyle(BUTTON_PANEL_VERTICAL);
	bool isTopLeft = hasWidgetStyle(BUTTON_PANEL_LEFT_TOP);
	bool isCenter = hasWidgetStyle(BUTTON_PANEL_CENTER);
	int containerLength = getContainerLength();
	int arrowLength = Floor(float(isVertical ? buttonWidth : buttonHeight) / 2.0f);

	if (isVertical && isTopLeft && !isCenter) {
		if (!isFirst) {
			y = containerLength + arrowLength + 2 * indent - 1;
		}
	} else if (!isVertical && isTopLeft && !isCenter) {
		if (!isFirst) {
			x = containerLength + arrowLength + 2 * indent - 1;
			if ((x + arrowLength) > maxLength) {
				x = maxLength - arrowLength - 1;
			}
		}
	} else if (isVertical && !isTopLeft && !isCenter) {
		if (isFirst) {
			y = height - containerLength - arrowLength - 2 * indent - 1;
		} else {
			y = height - arrowLength - 1;
		}
	} else if (!isVertical && !isTopLeft && !isCenter) {
		if (isFirst) {
			x = width - containerLength - arrowLength - 2 * indent - 1;;
		} else {
			x = width - arrowLength - 1;
		}
	} else if (isVertical && isCenter) {
		if (isFirst) {
			y = maxLength / 2 - arrowLength / 2 - indent - containerLength / 2 - 1;
		} else {
			y = maxLength / 2 + containerLength / 2 + indent - 1;
		}
	} else if (!isVertical && isCenter) {
		if (isFirst) {
			x = maxLength / 2 - arrowLength - indent - containerLength / 2 - 1;
		} else {
			x = maxLength / 2 + containerLength / 2 + indent - 1;
		}
	}
	return iVec2(x, y);
}

void ButtonPanel::createTriangleButton(bool isFirst) {
	bool isVertical = hasWidgetStyle(BUTTON_PANEL_VERTICAL);
	bool isTopLeft = hasWidgetStyle(BUTTON_PANEL_LEFT_TOP);
	bool isCenter = hasWidgetStyle(BUTTON_PANEL_CENTER);
	int halfSize = Floor(float(isVertical ? buttonWidth : buttonHeight) / 2.0f);
	TriangleDirection direction = TRIANGLE_LEFT;
	if (isVertical) {
		if (isFirst) {
			direction = TRIANGLE_TOP;
		} else {
			direction = TRIANGLE_BOTTOM;
		}
	} else if (!isVertical && !isFirst) {
		direction = TRIANGLE_RIGHT;
	}
	iVec2 triangleButtonPosition = getTrianglePosition(isFirst);
	auto button = TriangleButton::create(triangleButtonPosition.x, triangleButtonPosition.y, isVertical ? buttonWidth : halfSize, !isVertical ? buttonHeight : halfSize, Self()->As<ButtonPanel>(), direction, triangleColor, 0.8f);
	if (isFirst) {
		button->setListener(
			[this](Event event) {
				selectPrevious();
				return true;
			});
		firstTriangleButton = button;
	} else {
		button->setListener(
			[this](Event event) {
				selectNext();
				return true;
			});
		secondTriangleButton = button;
	}
}

int ButtonPanel::getButtonThickness() {
	return hasWidgetStyle(BUTTON_PANEL_VERTICAL) ? buttonWidth : buttonHeight;
}

int ButtonPanel::getButtonLength() {
	return hasWidgetStyle(BUTTON_PANEL_VERTICAL) ? buttonHeight : buttonWidth;
}

int ButtonPanel::getButtonLength(shared_ptr<CustomButton> button) {
	return hasWidgetStyle(BUTTON_PANEL_VERTICAL) ? button->getHeight() : button->getWidth();
}

void ButtonPanel::draw() {
	bool isVertical = hasWidgetStyle(BUTTON_PANEL_VERTICAL);
	bool isCenter = hasWidgetStyle(BUTTON_PANEL_CENTER);
	int count = 0;
	int totalButtonLength = 0;//count * (getButtonLength() + indent) - indent;
	for (auto& button : buttonList) {
		if (button->GetHidden(false)) {
			continue;
		}
		//iVec2 position(isVertical ? 0 : count * (getButtonLength() + indent), !isVertical ? 0 : count * (getButtonLength() + indent));
		iVec2 position(isVertical ? 0 : totalButtonLength, !isVertical ? 0 : totalButtonLength);
		iVec2 buttonSize = button->GetSize();
		button->SetShape(position, buttonSize);
		button->SetColor(0.7f, 0.7, 0, 1, WIDGETCOLOR_HIGHLIGHT);
		totalButtonLength = totalButtonLength + getButtonLength(button) + (hasWidgetStyle(BUTTON_PANEL_NO_INDENT) ? 0 : indent);
		count++;
	}
	totalButtonLength = totalButtonLength - (totalButtonLength != 0 ? indent : 0);
	int containerLength = getContainerLength();
	bool doHideArrows = hasWidgetStyle(BUTTON_PANEL_AGILE_ARROWS) && totalButtonLength <= maxLength;
	firstTriangleButton->SetShape(getTrianglePosition(true), firstTriangleButton->GetSize());
	secondTriangleButton->SetShape(getTrianglePosition(false), secondTriangleButton->GetSize());
	if (doHideArrows) {
		firstTriangleButton->SetHidden(true);
		secondTriangleButton->SetHidden(true);
	} else if (firstTriangleButton->GetHidden()) {
		firstTriangleButton->SetHidden(false);
		secondTriangleButton->SetHidden(false);
	}
	int arrowLength = isVertical ? secondTriangleButton->getHeight() : secondTriangleButton->getWidth();
	iVec2 containerPosition;
	iVec2 containerSize;
	if (isVertical) {
		containerPosition.x = 0;
		containerSize.width = buttonWidth + 1;
		if (doHideArrows) {
			containerPosition.y = firstTriangleButton->GetPosition().y;
			if (isCenter) {
				containerPosition.y = containerPosition.y + getButtonLength() / 2 + indent;
			}
			containerSize.height = totalButtonLength;
		} else {
			containerPosition.y = firstTriangleButton->GetPosition().y + arrowLength + indent;
			containerSize.height = containerLength;
		}
	} else {
		containerPosition.y = 0;
		containerSize.height = buttonHeight;
		if (doHideArrows) {
			if (firstTriangleButton->GetPosition().x != 0) {
				containerPosition.x = firstTriangleButton->GetPosition().x + arrowLength + indent;
			}
			if (isCenter) {
				//containerPosition.x = containerPosition.x + arrowLength + indent;
			}
			containerSize.width = totalButtonLength;
		} else {
			containerPosition.x = firstTriangleButton->GetPosition().x;
			containerSize.width = containerLength + 1;
		}

	}
	container->SetShape(containerPosition, containerSize);
	container->updateInnerContainerSize();
	Redraw();
}

int ButtonPanel::getLength() {
	return hasWidgetStyle(BUTTON_PANEL_VERTICAL) ? GetSize().height : GetSize().width;
}

void ButtonPanel::setButtonHeight(int height) {
	buttonHeight = height;
}

void ButtonPanel::addButtons(vector<shared_ptr<CustomButton>> buttons) {
	container->addWidgets(vector<shared_ptr<Widget>>(buttons.begin(), buttons.end()));
	buttonList.insert(buttonList.end(), buttons.begin(), buttons.end());
	draw();
}

void ButtonPanel::addButton(shared_ptr<CustomButton> button) {
	addButtons(vector<shared_ptr<CustomButton>>{button});
}

vector<shared_ptr<Widget>> ButtonPanel::getButtons() {
	return container->getWidgets();
}

void ButtonPanel::insertButton(shared_ptr<CustomButton> button, int index) {
	bool isInserted = false;
	if (!buttonList.empty()) {
		auto iter = buttonList.begin();
		for (int i = 0; i < index && i < buttonList.size() && iter != buttonList.end(); i++) {
			if ((*iter)->GetHidden()) {
				i--;
			}
			iter++;
		}
		if (iter != buttonList.end()) {
			buttonList.emplace(iter, button);
			isInserted = true;
		}
	}
	if (!isInserted) {
		buttonList.push_back(button);
	}
	container->addWidget(button);
	button->SetHidden(false);
	draw();
}

void ButtonPanel::removeButton(shared_ptr<CustomButton> button) {
	container->removeWidget(button);
	buttonList.remove(button);
	draw();
}

void ButtonPanel::clear() {
	for (auto& button : buttonList) {
		button->SetParent(nullptr);
	}
	buttonList.clear();
	container->clear();
	draw();
	selectedButton = NULL;
}

bool ButtonPanel::doSelectButton(list<shared_ptr<CustomButton>>::iterator iter) {
	selectedButton = *iter;
	if (!selectedButton->GetHidden()) {
		selectedButton->select(true);
		return true;
	}
	return false;
}

void ButtonPanel::selectButton(bool isBackward) {
	if (buttonList.empty()) {
		return;
	}
	list<shared_ptr<CustomButton>>::iterator iter;
	list<shared_ptr<CustomButton>>::iterator lastIter = buttonList.end();
	lastIter--;
	if (selectedButton) {
		selectedButton->select(false);
		if (doCycle && isBackward && selectedButton == buttonList.front()) {
			iter = buttonList.end();
		} else if (doCycle && !isBackward && selectedButton == *lastIter) {
			iter = buttonList.begin();
			if (doSelectButton(iter)) {
				updateContentPosition();
				return;
			}
		} else {
			iter = find(buttonList.begin(), buttonList.end(), selectedButton);
		}
		selectedButton = NULL;
	} else {
		if (isBackward) {
			iter = buttonList.end();
		} else {
			iter = buttonList.begin();
			if (doSelectButton(iter)) {
				updateContentPosition();
				return;
			}
		}
	}

	if (!isBackward) {
		if (iter != buttonList.end()) {
			for (iter++; iter != buttonList.end(); ++iter) {
				if (doSelectButton(iter)) {
					break;
				}
			}
		} else {
			for (iter--; iter != buttonList.begin(); --iter) {
				if (doSelectButton(iter)) {
					break;
				}
			}
			if (iter == buttonList.begin()) {
				doSelectButton(iter);
			}
		}

	} else {
		if (iter != buttonList.begin()) {
			for (iter--; iter != buttonList.begin(); --iter) {
				if (doSelectButton(iter)) {
					break;
				}
			}
			if (iter == buttonList.begin()) {
				doSelectButton(iter);
			}
		} else {
			for (; iter != buttonList.end(); ++iter) {
				if (doSelectButton(iter)) {
					break;
				}
			}
			if (iter == buttonList.end()) {
				doSelectButton(iter);
			}
		}
	}
	updateContentPosition();
}

void ButtonPanel::selectPrevious() {
	//if (buttonList.empty() || (!doCycle && selectedButton == buttonList.front())) {
	if (buttonList.empty() || (!doCycle && selectedButton == buttonList.front())) {
		deselect();
		return;
	}
	bool doOpposite = doCycle && selectedButton == buttonList.front();
	selectButton(true);
}

void ButtonPanel::selectNext() {
	//if (buttonList.empty() || (!doCycle && selectedButton == buttonList.back())) {
	if (buttonList.empty() || (!doCycle && selectedButton == buttonList.back())) {
		deselect();
		return;
	}
	bool doOpposite = doCycle && selectedButton == buttonList.back();
	selectButton(false);
}

void ButtonPanel::selectFirst() {
	if (buttonList.begin() == buttonList.end()) {
		deselect();
		return;
	}
	doSelectButton(buttonList.begin());
	updateContentPosition();
}

void ButtonPanel::updateContentPosition() {
	if (selectedButton) {
		bool isVertical = hasWidgetStyle(BUTTON_PANEL_VERTICAL);
		if (isVertical && (selectedButton->GetPosition().y + container->getInnerContainerPosition().y) < 0) {
			container->moveContent(0, buttonHeight);
		} else if (!isVertical && (selectedButton->GetPosition().x + container->getInnerContainerPosition().x) < 0) {
			container->moveContent(buttonWidth, 0);
		} else if (isVertical && (selectedButton->GetPosition().y + buttonHeight) > container->GetSize().height) {
			container->moveContent(0, -buttonHeight);
		} else if (!isVertical && (selectedButton->GetPosition().x + buttonWidth) > container->GetSize().width) {
			container->moveContent(-buttonWidth, 0);
		}
	}
}

void ButtonPanel::select(shared_ptr<CustomButton> button, bool doSwitchInteractive) {
	if (selectedButton == button) return;
	if (selectedButton) {
		selectedButton->select(false);
		if (doSwitchInteractive) {
			selectedButton->SetInteractive(true);
		}
	}
	selectedButton = button;
	if (selectedButton && !selectedButton->GetHidden()) {
		selectedButton->select(true);
		if (doSwitchInteractive) {
			selectedButton->SetInteractive(false);
		}
	}
}

void ButtonPanel::deselect() {
	if (selectedButton) {
		selectedButton->select(false);
		selectedButton = NULL;
	}
}

void ButtonPanel::setFirstArrowKey(KeyCode key) {
	firstArrowKey = key;
}

void ButtonPanel::setSecondArrowKey(KeyCode key) {
	secondArrowKey = key;
}

void ButtonPanel::useButtton() {
	if (selectedButton) {
		selectedButton->useListener(Event());
	}
}

bool ButtonPanel::KeyDown(const KeyCode key) {
	if (key == firstArrowKey) {
		selectPrevious();
	} else if (key == secondArrowKey) {
		selectNext();
	}
	return CustomWidget::KeyDown(key);
}

shared_ptr<CustomButton> ButtonPanel::getSelectedButton() {
	return selectedButton;
}

void ButtonPanel::updateHotkeyLabels() {
	for (auto& button : buttonList) {
		button->updateHotkeyLabel();
	}
}
