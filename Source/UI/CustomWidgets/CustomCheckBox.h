#pragma once
#include "CustomWidget.h"
#include "CustomListBox.h"

using namespace UltraEngine;
class CustomCheckBox : public CustomButton
{
protected:
	CustomCheckBox();
	bool checked = false;
	virtual void Draw(const int x, const int y, const int width, const int height);
public:
	static shared_ptr<CustomCheckBox> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0);
	virtual bool Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style);
	virtual bool useListener(const EventId id, shared_ptr<Object> source, const int data, iVec2 position, iVec2 size, shared_ptr<Object> extra = NULL);
	bool isChecked();
	void setChecked(bool check);
};

