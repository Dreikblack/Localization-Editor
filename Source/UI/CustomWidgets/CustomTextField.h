#pragma once
#include "UltraEngine.h"
#include "CustomWidget.h"


enum CustomTextFieldStyle
{
	CUSTOM_TEXT_FIELD_READONLY = 16,
	CUSTOM_TEXT_FIELD_INTEGER = 32,
	CUSTOM_TEXT_FIELD_PASSWORD = 64,
};

class CustomTextField : public CustomWidget
{
protected:
	bool ctrlPressed;
	bool pressed;
	iVec2 lastMousePosition;
	int textIndent;
	//cursor
	int caretPosition;
	//selection - how much symbols from carret. Right to carret is negative value
	int sellen;
	//when next not fitting widget size
	int offsetX;
	bool shiftPressed;
	int doubleClickRange;
	WString oldText;
	shared_ptr<Timer> timer;

	std::function<void(Event)> valueChangelistener;
	std::function<void(Event)> focusListener;

	CustomTextField();
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style, WString text = "");
	WString GetSelectedText();
	virtual int GetCharAtPosition(const int position, const bool clickOnChar = false);
	int GetCaretCoord();
	int GetCaretCoord(const int caret);
	virtual void UpdateOffset();
	virtual void fixResult();
	virtual void Cut(), Copy(), Paste();
	void MouseDown(const MouseButton button, const int x, const int y) override;
	void DoubleClick(const MouseButton button, const int x, const int y) override;
	void TripleClick(const MouseButton button, const int x, const int y)override ;
	void MouseUp(const MouseButton button, const int x, const int y) override;
	void MouseMove(const int x, const int y) override;
	void GainFocus() override;
	void LoseFocus() override;
	void MouseEnter(const int x, const int y) override;
	void MouseLeave(const int x, const int y) override;
	void KeyUp(const KeyCode key) override;
	bool KeyDown(const KeyCode key) override;
	void KeyChar(const int keychar) override;
	void Draw(const int x, const int y, const int width, const int height) override;
	static bool drawCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra);
	void moveCaretLeft();
	void moveCaretRight();
	void del();
	void resetCursorBlinking();
public:
	bool doHideCursor;
	int minValue = 0;
	int maxValue = 0;
	int minSymbols = 0;
	int maxSymbols = 0;
	bool doTriggerValueChangeOnType;
	static shared_ptr<CustomTextField> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0, WString text = "");
	int getIntegerValue();
	void setValueChangeListener(std::function<void(Event)> valueChangelistener);
	void setFocusListener(std::function<void(Event)> focusListenerToSet);
	virtual void SelectText(const int position, const int length);
	virtual WString GetText(const bool selected = false);
	virtual void SetText(const WString& text);
	virtual bool OK();
};
