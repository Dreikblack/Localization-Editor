#include "UltraEngine.h"
#include "CustomCheckBox.h"
#include "Paint.h"
#include "../../Managers/ResourceManager.h"

CustomCheckBox::CustomCheckBox()
{
}

shared_ptr<CustomCheckBox> CustomCheckBox::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	struct Struct : public CustomCheckBox {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", x, y, width, height, parent, style);
	return instance;
}

bool CustomCheckBox::Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	bool isInit = CustomButton::Initialize("", x, y, width, height, parent, WIDGET_BACKGROUND | WIDGET_BORDER | style);
	allowedPush = false;
	//auto checkPixmap = CreatePixmap(height, height);
	//Paint::drawCheckMark(checkPixmap, getRgbaColor(WIDGETCOLOR_FOREGROUND));
	//SetPixmap(checkPixmap);
	auto checkMarkIcon = ResourceManager::getInstance()->getGuiIcon("CheckMark");
	SetIcon(checkMarkIcon, PIXMAP_CENTER,((float)width / (float) GUI_SCALE));
	SetColor(Vec4(0,0,0,1), WIDGETCOLOR_BORDER);
	return isInit;
}

void CustomCheckBox::Draw(const int x, const int y, const int width, const int height)
{
	CustomButton::Draw(x, y, width, height);
	blocks[1].hidden = !checked;
	blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
}

bool CustomCheckBox::useListener(const EventId id, shared_ptr<Object> source, const int data, iVec2 position, iVec2 size, shared_ptr<Object> extra)
{
	checked = !checked;
	Redraw();
	return CustomButton::useListener(id, source, data, position, size, extra);
}

bool CustomCheckBox::isChecked()
{
	return checked;
}

void CustomCheckBox::setChecked(bool check)
{
	CustomCheckBox::checked = check;
	Redraw();
}
