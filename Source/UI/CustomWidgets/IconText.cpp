#include "UltraEngine.h"
#include "IconText.h"

IconText::IconText() {
	//blocks.resize(2);//icon, text
	buttonHoverSound = "";
}

shared_ptr<IconText> IconText::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon, WString text) {
	struct Struct : public IconText {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, icon, text);
	return instance;
}

void IconText::Draw(const int x, const int y, const int width, const int height) {
	blocks.clear();

	//Image
	if (pixmap) {
		int block = AddBlock(pixmap, iVec2(0), Vec4(1));
		blocks[block].size = iVec2(height);
	}

	//Text
	int block = AddBlock(text, iVec2(height, 0), iVec2(width - height, height), color[WIDGETCOLOR_SLIDERTRACK], textAlignment);
	blocks[block].size.width = width - height;
	blocks[block].size.height = height;

}

bool IconText::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, shared_ptr<Icon> icon, WString text) {
	bool flag = Widget::Initialize(text, x, y, width, height, parent, style);
	if (icon) {
		SetIcon(icon);
	}
	setAutoFontScale();
	Redraw();
	return flag;
}

void IconText::SetIcon(shared_ptr<Icon> icon) {
	if (icon) {
		CustomWidget::SetIcon(icon, PIXMAP_CENTER, (float)getHeight() / (float)GUI_SCALE);
	}
}

void IconText::setTextAlignment(int textAlignment) {
	IconText::textAlignment = textAlignment;
}

void IconText::MouseEnter(const int x, const int y) {
	CustomButton::MouseEnter(x, y);
	if (GetParent()->As<CustomButton>()) {
		GetParent()->As<CustomButton>()->setHover(true);
	}
}

void IconText::MouseLeave(const int x, const int y) {
	CustomButton::MouseLeave(x, y);
	if (GetParent()->As<CustomButton>()) {
		//GetParent()->As<CustomButton>()->setHover(false);
	}
}

void IconText::LoseFocus() {
	CustomButton::LoseFocus();
	if (GetParent()->As<CustomButton>()) {
		//GetParent()->As<CustomButton>()->setHover(false);
	}
}

void IconText::MouseDown(const MouseButton button, const int x, const int y) {
	if (GetParent()->As<CustomButton>()) {
		GetParent()->As<CustomButton>()->MouseDown(button, x, y);
	}
}

void IconText::MouseUp(const MouseButton button, const int x, const int y) {
	if (GetParent()->As<CustomButton>()) {
		GetParent()->As<CustomButton>()->MouseUp(button, x, y);
	}
}