#pragma once
#include "UltraEngine.h"
#include "CustomWidget.h"
#include "../../Managers/ControlsManager.h"
#include "CustomLabel.h"

class CustomButton : public CustomWidget
{
protected:
	CustomButton();
	~CustomButton() override;
	std::function<bool(Event)> listener;
	std::function<bool(Event)> doubleClickListener;
	bool pushState, selectedState, disableState;
	bool allowedPush = false;//trigger an action every 100 ms if true while pushing a button
	ACTION_TYPE actionType;
	shared_ptr<CustomLabel> hotkeyLabel;
	shared_ptr<Timer> timer;
	shared_ptr<Timer> hotKeyTimer;
	static bool eventHotKeyButtonCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra);

	virtual void MouseMove(const int x, const int y);
	virtual void MouseEnter(const int x, const int y);
	virtual void MouseLeave(const int x, const int y);
	static bool eventActionCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra);
	virtual void LoseFocus();
	void Draw(const int x, const int y, const int width, const int height) override;
	virtual bool KeyDown(const KeyCode key);
public:
	bool t = false;
	WString buttonClickSound;
	WString buttonHoverSound;
	int timerDelay;
	static shared_ptr<CustomButton> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0);
	void Enable() override;
	void Disable() override;
	void SetText(const WString& text) override;
	void MouseDown(const MouseButton button, const int x, const int y) override;
	void MouseUp(const MouseButton button, const int x, const int y) override;
	void DoubleClick(const MouseButton button, const int x, const int y) override;
	void setListener(std::function<bool(Event)> listener);
	void setDoubleListener(std::function<bool(Event)> listener);
	bool useListener(Event event);
	bool useListener();
	virtual bool useListener(const EventId id, shared_ptr<Object> source, const int data, iVec2 position, iVec2 size, shared_ptr<Object> extra = NULL);
	virtual void clearHotkey();
	/// <summary>
	/// Set Hotkey for GUI Button
	/// </summary>
	/// <param name="actionType">Action Type</param>
	/// <param name="doClearPrev">Do clear all prev listeners for new action</param>
	/// 	<param name="doForceHotkeyLabel">Do set new hotkey label anyway</param>
	virtual void setHotkey(ACTION_TYPE actionType, bool doClearPrev = false, bool doForceHotkeyLabel = true);
	virtual void setHotkeyLabel(ACTION_TYPE actionType);
	virtual void updateHotkeyLabel();
	bool isPushed();
	virtual void select(bool selected);
	void enable(bool isEnable = true);
	bool isEnable();
	void enablePushing(bool push = true);
	void setHover(bool hover);
};
