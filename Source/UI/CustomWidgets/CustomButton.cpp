#include "UltraEngine.h"
#include "CustomButton.h"
#include "../../Managers/ControlsManager.h"
#include "../../Managers/ResourceManager.h"

CustomButton::CustomButton() {
	hoverState = false;
	pushState = false;
	selectedState = false;
	disableState = false;
	timerDelay = Round(1000.0f / (float)GAME_FREQUENCY) - 1;
	buttonClickSound = "";
	buttonHoverSound = "";
	actionType = ACTION_NONE;
}

CustomButton::~CustomButton() = default;


shared_ptr<CustomButton> CustomButton::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	struct Struct : public CustomButton {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", x, y, width, height, parent, style);
	instance->SetColor(instance->color[WIDGETCOLOR_SLIDERTRACK], WIDGETCOLOR_BORDER);
	instance->setAutoFontScale();
	return instance;
}

void CustomButton::MouseMove(const int x, const int y) {
	if (x > 0 and y > 0 and x < size.x and y < size.y) {
		//MouseEnter(x, y);
	}
}

void CustomButton::MouseEnter(const int x, const int y) {
	if (GetInterface() && !GetInterface()->MouseDown(MOUSE_LEFT)) {
		if (!hoverState) {
			ResourceManager::getInstance()->playSound(buttonHoverSound);
		}
	}
	CustomWidget::MouseEnter(x, y);
}

void CustomButton::MouseLeave(const int x, const int y) {
	if (hoverState || pushState) {
		hoverState = false;
		pushState = false;
		Redraw();
	}
	CustomWidget::MouseLeave(x, y);
}

bool CustomButton::eventActionCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra) {
	auto button = extra->As<CustomButton>();
	if (!button->isPushed()) {
		button->timer->Stop();
		button->timer = nullptr;
		return true;
	}
	button->useListener(EVENT_WIDGETACTION, button, 0, iVec2(), iVec2());
	button->Redraw();
	return false;
}

void CustomButton::MouseDown(const MouseButton button, const int x, const int y) {
	if (button != MOUSE_LEFT) return;
	if (hoverState == 0) return;
	pushState = true;
	if (allowedPush) {
		timer = UltraEngine::CreateTimer(timerDelay);
		ListenEvent(EVENT_TIMERTICK, timer, eventActionCallback, Self());
	}
	CustomWidget::MouseDown(button, x, y);
	Redraw();
}

void CustomButton::MouseUp(const MouseButton button, const int x, const int y) {
	if (button != MOUSE_LEFT) return;
	if (hoverState) {
		pushState = false;
		Redraw();
		if (!allowedPush) ResourceManager::getInstance()->playSound(buttonClickSound);
		useListener(EVENT_WIDGETACTION, Self(), 0, iVec2(), iVec2());
	}
	MouseMove(x, y);
	CustomWidget::MouseUp(button, x, y);
}

void CustomButton::DoubleClick(const MouseButton button, const int x, const int y) {
	CustomWidget::DoubleClick(button, x, y);
	if (doubleClickListener) {
		doubleClickListener(Event(EVENT_WIDGETACTION, Self(), 0, iVec2(), iVec2()));
	}
}

void CustomButton::LoseFocus() {
	hoverState = false;
	pushState = false;
	Redraw();
}

void CustomButton::Draw(const int x, const int y, const int width, const int height) {
	//in 2D width/height are not widget size for unkown reason
	int buttonWidth = getWidth();
	int buttonHeight = getHeight();
	CustomWidget::Draw(x, y, buttonWidth, !hotkeyLabel ? buttonHeight : buttonHeight - (getWidth() / 2) - indent);
	if (!disableState) {
		//Background
		blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
		if (hoverState) {
			blocks[0].color += 0.05;
		}
		blocks[0].gradient = 0.75f;

		if (pushState) {
			blocks[0].color = color[WIDGETCOLOR_BACKGROUND] * 0.75f;
			blocks[0].gradient = 1.0f / 0.75f;
			blocks[1].position.x++;
			blocks[1].position.y++;
			blocks[3].position.x++;
			blocks[3].position.y++;
		}

		//Border
		if (hoverState && !selectedState) {	
			blocks[2].color = color[WIDGETCOLOR_HIGHLIGHT];
		} else if (selectedState) {
			blocks[2].color = color[WIDGETCOLOR_SELECTION];
		} else {
			blocks[2].color = color[WIDGETCOLOR_BORDER];
		}
	} else {
		blocks[0].color = color[WIDGETCOLOR_RAISED];
		blocks[0].gradient = 1;
		blocks[2].color = color[WIDGETCOLOR_BORDER];
	}
	// text color
	if (hasWidgetStyle(WIDGET_BACKGROUND)) {
		//blocks[3].color = !selectedState ? color[WIDGETCOLOR_SLIDERARROW] : color[WIDGETCOLOR_SLIDERTRACK];
	} else {
		if (!disableState && hoverState) {
			blocks[3].color = color[WIDGETCOLOR_FOREGROUND] * 1.2f;
		} else {
			blocks[3].color = !disableState ? color[WIDGETCOLOR_FOREGROUND] : color[WIDGETCOLOR_FOREGROUND] * 0.8f;
		}
		blocks[3].gradient = 0.75f;
	}
}

bool CustomButton::KeyDown(const KeyCode key) {
	return CustomWidget::KeyDown(key);
}

void CustomButton::SetText(const WString& text) {
	if (CustomButton::text != text) {
		CustomWidget::SetText(text);
		if (!isAutoFontScale) {
			setAutoFontScale();
		}
	}
}

void CustomButton::setListener(std::function<bool(Event)> listener) {
	CustomButton::listener = listener;
}

void CustomButton::setDoubleListener(std::function<bool(Event)> listener) {
	CustomButton::doubleClickListener = listener;
}

bool CustomButton::useListener(Event event) {
	if (!listener) {
		return false;
	}
	if (!allowedPush) ResourceManager::getInstance()->playSound(buttonClickSound);
	return listener(event);
}

bool CustomButton::useListener() {
	return useListener(EVENT_WIDGETACTION, Self(), 0, iVec2(), iVec2());
}


bool CustomButton::useListener(const EventId id, shared_ptr<Object> source, const int data = 0, iVec2 position = 0, iVec2 size = 0, shared_ptr<Object> extra) {
	if (!listener) {
		return false;
	}
	if (!allowedPush) ResourceManager::getInstance()->playSound(buttonClickSound);
	return listener(Event(id, source, data, position, size, extra));
}

void CustomButton::clearHotkey() {
	auto controlsManager = ControlsManager::getInstance();
	controlsManager->removeActionListener(actionType);
}

bool CustomButton::eventHotKeyButtonCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra) {
	auto button = extra->As<CustomButton>();
	button->enable(true);
	if (button->hotKeyTimer) {
		button->hotKeyTimer->Stop();
		button->hotKeyTimer = nullptr;
	}
	return false;
}

void CustomButton::setHotkey(ACTION_TYPE newActionType, bool doClearPrev, bool doForceHotkeyLabel) {
	ACTION_TYPE oldActionType = CustomButton::actionType;
	if (newActionType == ACTION_NONE) {
		return;
	}
	if (doForceHotkeyLabel || oldActionType == ACTION_NONE) {
		setHotkeyLabel(newActionType);
		CustomButton::actionType = newActionType;
	}
	auto controlsManager = ControlsManager::getInstance();

	if (doClearPrev && newActionType != ACTION_NONE) {
		controlsManager->removeActionListener(newActionType);
	}
	controlsManager->addActionListener(newActionType, Self(), [this](Event event) {
		if (GetInterface() && !GetInterface()->GetHidden() && !GetHidden() && !GetDisabled() && GetInteractive()) {
			useListener();
			if (!GetHidden() && !GetDisabled()) {
				hotKeyTimer = UltraEngine::CreateTimer(50);
				ListenEvent(EVENT_TIMERTICK, hotKeyTimer, eventHotKeyButtonCallback, Self());
				enable(false);
			}
		}
		});
	Redraw();
}

void CustomButton::setHotkeyLabel(ACTION_TYPE actionType) {
	CustomButton::actionType = actionType;
	auto controlsManager = ControlsManager::getInstance();
	auto key = controlsManager->getFirstActionKey(actionType);
	if (hotkeyLabel) {
		hotkeyLabel->SetText(key);
	} else {
		int labelSize = getWidth() / 2;
		setHeight(getHeight() + indent + labelSize);
		hotkeyLabel = CustomLabel::create(key, labelSize / 2, getHeight() - labelSize, labelSize, labelSize, Self()->As<CustomButton>(), TEXT_MIDDLE | TEXT_CENTER);	
		hotkeyLabel->SetColor(Vec4(0.1f, 0.1f, 0.1f, 0.5f), WIDGETCOLOR_BACKGROUND);
		hotkeyLabel->SetColor(Vec4(1), WIDGETCOLOR_SLIDERTRACK);
		hotkeyLabel->setBackground(true);
	}
	int labelWidth = GetInterface()->GetTextWidth(GetInterface()->font, hotkeyLabel->getFontScale(), key, 1) + indent;
	labelWidth = labelWidth < getWidth() ? labelWidth : getWidth();
	hotkeyLabel->setWidth(labelWidth);
	hotkeyLabel->setPositionX((getWidth() - labelWidth) / 2);
}

void CustomButton::updateHotkeyLabel() {
	setHotkeyLabel(actionType);
}

bool CustomButton::isPushed() {
	return pushState;
}

void CustomButton::select(bool selected) {
	selectedState = selected;
	Redraw();
}

void CustomButton::enable(bool isEnable) {
	if (isEnable) {
		CustomWidget::Enable();
		disableState = false;
	} else {
		disableState = true;
		Redraw();
		CustomWidget::Disable();
	}
}

bool CustomButton::isEnable() {
	return !disableState;
}

void CustomButton::Enable() {
	enable(true);
}

void CustomButton::Disable() {
	enable(false);
}

void CustomButton::enablePushing(bool push) {
	allowedPush = push;
}

void CustomButton::setHover(bool hover) {
	hoverState = hover;
}
