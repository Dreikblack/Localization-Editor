#pragma once

#include "CustomWidget.h"
#include "CustomTextField.h"
#include "CustomSlider.h"

class NumberSliderField : public CustomWidget
{
protected:
	shared_ptr<CustomTextField> numberField;
	shared_ptr<CustomSlider> horizontalSlider;
	NumberSliderField();
	virtual void Draw(const int x, const int y, const int width, const int height);
public:
	static std::shared_ptr<NumberSliderField> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int maxNumber = 9999, int style = 0);
	bool virtual Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int maxNumber, int style = 0);
	void setMinValue(int minValue);
	void setMaxValue(int maxValue);
	void setMinSymbols(int minSymbols);
	void setMaxSymbols(int maxSymbols);
	void setValue(int value);
	int getValue();
};