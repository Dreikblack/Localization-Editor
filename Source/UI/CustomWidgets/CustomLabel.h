#pragma once
#include "CustomWidget.h"

class CustomLabel : public CustomWidget
{
protected: 
	CustomLabel();
	bool hasBackground = false;
	virtual void Draw(const int x, const int y, const int width, const int height);
	virtual bool Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style);
public:
	static shared_ptr<CustomLabel> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = TEXT_MIDDLE);
	static shared_ptr<CustomLabel> create(WString text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = TEXT_MIDDLE);
	void setBackground(bool hasBackground);
};

