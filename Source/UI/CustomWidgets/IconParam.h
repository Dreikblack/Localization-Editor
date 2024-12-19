#pragma once
#include "UltraEngine.h"
#include "CustomButton.h"

class IconParam : public CustomWidget
{
protected:
	const int NAME_FACTOR = 12;
	WString name;
	IconParam();
	virtual void Draw(const int x, const int y, const int width, const int height);
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon, WString name, WString value);
public:
	static shared_ptr<IconParam> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon = nullptr, WString name = "", WString value = "");
	virtual void SetIcon(shared_ptr<Icon> icon);
};

