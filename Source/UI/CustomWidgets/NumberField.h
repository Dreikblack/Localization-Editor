#pragma once
#include "UltraEngine.h"
#include "CustomButton.h"
#include "TriangleButton.h"

class NumberField : public CustomButton
{
protected:
	NumberField();
	virtual void Draw(const int x, const int y, const int width, const int height);
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, WString text = "");
	shared_ptr<TriangleButton> upButton;
	shared_ptr<TriangleButton> downButton;
	std::function<void(Event)> valueChangelistener;
public:
	static shared_ptr<NumberField> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, WString text = "");
	int value = 0;
	int minValue = 0;
	int maxValue = 0;
	void setValueChangeListener(std::function<void(Event)> valueChangelistener);
	void useUpButton();
	void useDownButton();
};
