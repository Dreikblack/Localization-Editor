#include "UltraEngine.h"
#include "CustomLabel.h"

CustomLabel::CustomLabel()
{
	blocks.resize(2);//bg, text
	textAlignment = TEXT_MIDDLE;
}

void CustomLabel::Draw(const int x, const int y, const int width, const int height)
{
	blocks[0].hidden = true; 
	if (text.length() == 0)
	{
		blocks[1].hidden = true;
		return;
	}

	if (hasBackground) {
		blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
		blocks[0].wireframe = false;
		blocks[0].position = iVec2(0);
		blocks[0].size = size;
		blocks[0].hidden = false;
	}

	//Text
	blocks[1].hidden = false;
	blocks[1].position = iVec2(!hasWidgetStyle(textAlignment, TEXT_CENTER) && !hasWidgetStyle(textAlignment, TEXT_RIGHT) ? indent : 0, 0);
	blocks[1].size = size;
	blocks[1].SetText(text);
	blocks[1].textalignment = textAlignment;
	blocks[1].color = color[WIDGETCOLOR_FOREGROUND];


}

bool CustomLabel::Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	bool isInit = CustomWidget::Initialize(text, x, y, width, height, parent, style);
	textAlignment = style;
	setAutoFontScale();
	SetInteractive(false);
	return isInit;
}

shared_ptr<CustomLabel> CustomLabel::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	struct Struct : public CustomLabel {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", x, y, width, height, parent, style);
	return instance;
}

shared_ptr<CustomLabel> CustomLabel::create(WString text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	struct Struct : public CustomLabel {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(text, x, y, width, height, parent, style);
	return instance;
}

void CustomLabel::setBackground(bool hasBackground)
{
	CustomLabel::hasBackground = hasBackground;
	Redraw();
}
