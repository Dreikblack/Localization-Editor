#pragma once
#include "UltraEngine.h"
#include "CustomTextField.h"

/// <summary>
/// Multi line text field
/// </summary>
class CustomTextArea : public CustomTextField
{
protected:
	int stringHeight;
	CustomTextArea();
	virtual bool Initialize(const int x, const int y, const int width, const int height, const int _stringHeight, shared_ptr<Widget> parent, const int style, WString text = "");
	int GetCharAtPosition(const iVec2 position, const bool clickOnChar = false);
	iVec2 GetCaretCoord();
	iVec2 GetCaretCoord(const int caret);
	void UpdateOffset() override;
	void MouseDown(const MouseButton button, const int x, const int y) override;
	void DoubleClick(const MouseButton button, const int x, const int y) override;
	void TripleClick(const MouseButton button, const int x, const int y) override;
	void MouseMove(const int x, const int y) override;
	bool KeyDown(const KeyCode key) override;
	void KeyChar(const int keychar) override;
	void Draw(const int x, const int y, const int width, const int height) override;
public:
	static shared_ptr<CustomTextArea> create(const int x, const int y, const int width, const int height, const int _stringHeight, shared_ptr<Widget> parent, const int style = 0, WString text = "");
	void SetText(const WString& text) override;
	void SetHidden(const bool hide) override;
	void resetStates();
	void setStringHeight(int newStringHeight);
};
