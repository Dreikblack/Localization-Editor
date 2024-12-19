#pragma once
#include "UltraEngine.h"
#include "CustomButton.h"

class IconText : public CustomButton
{
protected:
	IconText();
	virtual void Draw(const int x, const int y, const int width, const int height);
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon, WString text = "");
public:
	static shared_ptr<IconText> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon = nullptr, WString text = "");
	virtual void SetIcon(shared_ptr<Icon> icon);
	void setTextAlignment(int textAlignment);
	virtual void MouseEnter(const int x, const int y);
	virtual void MouseLeave(const int x, const int y);
	virtual void LoseFocus();
	virtual void MouseDown(const MouseButton button, const int x, const int y);
	virtual void MouseUp(const MouseButton button, const int x, const int y);
};

