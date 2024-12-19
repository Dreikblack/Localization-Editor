#include "UltraEngine.h"
#include "TriangleButton.h"
#include "Paint.h"
#include "../../Managers/ResourceManager.h"

TriangleButton::TriangleButton()
{
	borderColor = getRgbaColor(WIDGETCOLOR_BORDER);
	color = getRgbaColor(WIDGETCOLOR_FOREGROUND);
	allowedPush = true;
//	classname = "TriangleButton";
}

shared_ptr<TriangleButton> TriangleButton::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, TriangleDirection direction, unsigned int color, const float gradient)
{
	struct Struct : public TriangleButton {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", x, y, width, height, parent, WIDGET_DEFAULT);
	if (color != 0) {
		instance->color = color;
	}
	//auto pixmap = UltraEngine::CreatePixmap(width, height);
	//vector<iVec2> points(3, iVec2(0, 0));
	//switch (direction) {
	//case TRIANGLE_TOP:
	//	points[0] = iVec2(Floor(float(width) / 2.0f), 0);
	//	points[1] = iVec2(0, height - 1);
	//	points[2] = iVec2(width - 1, height - 1);
	//	break;
	//case TRIANGLE_BOTTOM:
	//	points[0] = iVec2(Floor(float(width) / 2.0f), height - 1);
	//	points[1] = iVec2(0, 0);
	//	points[2] = iVec2(width - 1, 0);
	//	break;
	//case TRIANGLE_RIGHT:
	//{
	//	points[0] = iVec2(width - 1, Floor(float(height) / 2.0f));
	//	points[1] = iVec2(0, 0);
	//	points[2] = iVec2(0, height - 1);
	//}
	//break;
	//case TRIANGLE_LEFT:
	//default:
	//{
	//	points[0] = iVec2(0, Floor(float(height) / 2.0f));
	//	points[1] = iVec2(width - 1, 0);
	//	points[2] = iVec2(width - 1, height - 1);
	//}
	//break;
	//}
	//Paint::drawFilledShape(pixmap, points, instance->color, instance->borderColor, gradient);
	//instance->SetPixmap(pixmap);
	shared_ptr<Icon> arrowIcon;
	auto arrorColor = instance->GetColor(WIDGETCOLOR_SLIDERARROW);
	switch (direction) 
	{
	case TRIANGLE_TOP:
	{
		arrowIcon = ResourceManager::getInstance()->getGuiIcon("ArrowUp");
		break;
	}

	case TRIANGLE_BOTTOM:
	{
		arrowIcon = ResourceManager::getInstance()->getGuiIcon("ArrowDown");
		break;

	}
	case TRIANGLE_RIGHT:
	{
		arrowIcon = ResourceManager::getInstance()->getGuiIcon("ArrowRight");
		break;
	}
	case TRIANGLE_LEFT:
	default:
	{
		arrowIcon = ResourceManager::getInstance()->getGuiIcon("ArrowLeft");

	}
	}
	arrowIcon->SetColor(arrorColor.x, arrorColor.y, arrorColor.z, arrorColor.w);
	instance->SetIcon(arrowIcon, PIXMAP_CENTER, ((float)height / (float)GUI_SCALE));
	return instance;
}

void TriangleButton::setColor(unsigned int color, const float gradient)
{
	Paint::drawChangeColor(GetPixmap(), TriangleButton::color, color);
	TriangleButton::color = color;
}