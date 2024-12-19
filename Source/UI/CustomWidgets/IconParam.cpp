#include "UltraEngine.h"
#include "IconParam.h"

IconParam::IconParam()
{
	textAlignment = TEXT_LEFT | TEXT_MIDDLE;
}

shared_ptr<IconParam> IconParam::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon, WString name, WString value)
{
	struct Struct : public IconParam {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, icon, name, value);
	return instance;
}

bool IconParam::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon, WString name, WString value)
{
	bool flag = Widget::Initialize(value, x, y, width, height, parent, 0);
	IconParam::name = name;
	if (icon)
	{
		SetIcon(icon);
	}
	setAutoFontScale();
	Redraw();
	return flag;
}

void IconParam::Draw(const int x, const int y, const int width, const int height)
{
	blocks.clear();
	//Image
	if (pixmap) {
		int block = AddBlock(pixmap, iVec2(0), Vec4(1));
		blocks[block].size = iVec2(height);
	}
	int valueTextWidth = indent;
	if (!text.empty()) {
		valueTextWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, text, 1);
	}

	//Name
	int block = AddBlock(name, iVec2(height + indent, 0), iVec2(width - height - valueTextWidth - indent, height), color[WIDGETCOLOR_SLIDERTRACKHOVERED], textAlignment);

	//Value
	block = AddBlock(text, iVec2(width - valueTextWidth - indent, 0), iVec2(valueTextWidth, height), color[WIDGETCOLOR_SLIDERTRACK], TEXT_RIGHT);
}

void IconParam::SetIcon(shared_ptr<Icon> icon)
{
	if (icon)
	{
		CustomWidget::SetIcon(icon, PIXMAP_CENTER, (float)getHeight() / (float)GUI_SCALE);
	}
}
