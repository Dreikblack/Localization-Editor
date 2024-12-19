#include "UltraEngine.h"
#include "NumberSliderField.h"

NumberSliderField::NumberSliderField() {
	blocks.resize(2);//background, border
}

shared_ptr<NumberSliderField> NumberSliderField::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int maxNumber, const int style) {
	struct Struct : public NumberSliderField {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, maxNumber, style);
	return instance;
}

bool NumberSliderField::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int maxNumber, const int style) {
	bool flag = CustomWidget::Initialize("", x, y, width, height, parent, style);
	setAutoFontScale();
	int textWidth = GetInterface()->GetTextWidth(GetInterface()->font, getFontScale(), WString(maxNumber), 1) + indent;
	numberField = CustomTextField::create(0, 0, textWidth, height, Self()->As<NumberSliderField>(), CUSTOM_TEXT_FIELD_INTEGER);
	numberField->maxValue = maxNumber;
	horizontalSlider = CustomSlider::create(textWidth + indent, 0, width - indent - textWidth, height, Self()->As<NumberSliderField>(), CUSTOM_SLIDER_HORIZONTAL);
	horizontalSlider->setMaxValue(maxNumber + 1);
	horizontalSlider->setListener([this](Event event) {
		if ((numberField->getIntegerValue() + 1) != horizontalSlider->getCurrentValue())
			numberField->SetText(horizontalSlider->getCurrentValue() - 1);
		return true;
		});
	numberField->setValueChangeListener([this](Event event) {
		if ((numberField->getIntegerValue() + 1) != horizontalSlider->getCurrentValue())
			horizontalSlider->setCurrentValue(event.data + 1);
		return true;
		});
	return flag;
}

void NumberSliderField::setMinValue(int minValue) {
	numberField->minValue = minValue;
}

void NumberSliderField::setMaxValue(int maxValue) {
	numberField->maxValue = maxValue;
	horizontalSlider->setMaxValue(maxValue + 1);
}

void NumberSliderField::setMinSymbols(int minSymbols) {
	numberField->minSymbols = minSymbols;
}

void NumberSliderField::setMaxSymbols(int maxSymbols) {
	numberField->maxSymbols = maxSymbols;
}

void NumberSliderField::setValue(int value) {
	numberField->SetText(value);
	horizontalSlider->setCurrentValue(value + 1, true);
}

int NumberSliderField::getValue() {
	return numberField->getIntegerValue();
}

void NumberSliderField::Draw(const int x, const int y, const int width, const int height) {
	auto size = iVec2(width, height);
	auto position = iVec2(0, 0);
	for (auto& block : blocks) {
		block.hidden = true;
	}
	//Background
	if (hasWidgetStyle(WIDGET_BACKGROUND)) {
		blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
		blocks[0].wireframe = false;
		blocks[0].position = position;
		blocks[0].size = size;
		blocks[0].hidden = false;
	}

	//Border
	if (hasWidgetStyle(WIDGET_BORDER)) {
		blocks[1].hidden = false;
		blocks[1].color = color[WIDGETCOLOR_BORDER];
		blocks[1].wireframe = true;
		blocks[1].position = position;
		blocks[1].size = size;
		blocks[1].radius = (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0;
	}
}