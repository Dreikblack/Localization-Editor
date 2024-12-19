#include "UltraEngine.h"
#include "NumberField.h"

NumberField::NumberField()
{
	blocks.resize(2);//text, value
	textAlignment = TEXT_RIGHT | TEXT_MIDDLE;
}


shared_ptr<NumberField> NumberField::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, WString text)
{
	struct Struct : public NumberField {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, text);
	return instance;
}

void NumberField::setValueChangeListener(std::function<void(Event)> valueChangelistener)
{
	NumberField::valueChangelistener = valueChangelistener;
}


bool NumberField::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, WString text)
{
	bool flag = CustomWidget::Initialize(text, x, y, width, height, parent, style);
	unsigned int triangleColor = getRgbaColor(WIDGETCOLOR_FOREGROUND);
	upButton = TriangleButton::create(width - height, 0, height, height / 2 - 1, Self()->As<NumberField>(), TRIANGLE_TOP, triangleColor);
	upButton->timerDelay = 200;
	upButton->setListener([this](Event event)
		{
			if (value < maxValue)
			{
				value++;
				if (valueChangelistener)
				{
					valueChangelistener(Event(EVENT_WIDGETACTION, Self()->As<NumberField>(), value));
				}
				Redraw();
			}
			return true;
		});
	downButton = TriangleButton::create(width - height, height / 2, height, height / 2 - 1, Self()->As<NumberField>(), TRIANGLE_BOTTOM, triangleColor);
	downButton->timerDelay = 200;
	downButton->setListener([this](Event event)
		{
			if (value > minValue)
			{
				value--;
				if (valueChangelistener)
				{
					valueChangelistener(Event(EVENT_WIDGETACTION, Self()->As<NumberField>(), value));
				}
				Redraw();
			}
			return true;
		});
	setAutoFontScale();
	return flag;
}


void NumberField::Draw(const int x, const int y, const int width, const int height)
{
	if (text.size() > 0)
	{
		// Text
		blocks[0].hidden = false;
		blocks[0].position = iVec2(0, 0);
		blocks[0].size.width = width - height * 2;
		blocks[0].size.height = height;
		blocks[0].SetText(text);
		blocks[0].textalignment = textAlignment;
		blocks[0].color = color[WIDGETCOLOR_SLIDERTRACK];
	}
	else
	{
		blocks[0].hidden = true;
	}

	// Value
	blocks[1].hidden = false;
	blocks[1].position = iVec2(width - height * 2, 0);
	blocks[1].size.width = height;
	blocks[1].size.height = height;
	blocks[1].SetText(value);
	blocks[1].textalignment = TEXT_CENTER | TEXT_MIDDLE;
	blocks[1].color = color[WIDGETCOLOR_SLIDERTRACK];
}

void NumberField::useUpButton() {
	upButton->useListener();
}

void NumberField::useDownButton() {
	downButton->useListener();
}