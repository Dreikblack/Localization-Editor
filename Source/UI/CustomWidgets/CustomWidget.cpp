#include "UltraEngine.h"
#include "CustomWidget.h"
#include "CustomLabel.h"
#include "../../Managers/ResourceManager.h"
#include "../../Managers/SettingsManager.h"

CustomWidget::CustomWidget() {
	cornerradius = 8;
	hoverState = false;
	//blocks.resize(4);//background, image, border, text
}

CustomWidget::~CustomWidget() {
}

bool CustomWidget::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	return Initialize("", x, y, width, height, parent, style);
}

bool CustomWidget::Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	bool flag = Widget::Initialize(text, x, y, width, height, parent, style);
	updateScale();
	return flag;
}

void CustomWidget::updateScale() {
	auto settingsManager = SettingsManager::getInstance();
	guiScale = settingsManager->guiScale;
	indent = guiScale / 8;
}

void CustomWidget::setAutoFontScale() {
	auto settingsManager = SettingsManager::getInstance();
	float fonstScale = (float)getHeight() / (float)settingsManager->fontHeight;
	isAutoFontScale = true;
	SetFontScale(fonstScale);
}

bool CustomWidget::hasWidgetStyle(int widgetStyle, int style) {
	return !((widgetStyle & style) == 0);
}

bool CustomWidget::hasWidgetStyle(int style) {
	return hasWidgetStyle(CustomWidget::style, style);
}

void CustomWidget::setLocalText(const WString& text, bool scaleWidth) {
	auto localText = ResourceManager::getInstance()->getLocalString(text);
	SetText(localText);
	if (scaleWidth) {
		setWidth(GetInterface()->GetTextWidth(GetInterface()->font, fontscale, localText, 1) + indent);
	}
}


void CustomWidget::setHintText(const WString& text, bool scaleWidth, bool scaleHeight, bool doAutoFontScale, int hintHeight) {
	if (text.empty()) {
		if (hintLabel) hintLabel->SetHidden(true);
	}
	int hintWidth = getWidth();
	hintHeight = hintHeight > 0 ? hintHeight : getHeight();
	auto subStrings = text.Split("\n");
	if (!hintLabel) {
		auto globalPosition = GetPosition(true);
		globalPosition.y = globalPosition.y + getHeight() + indent;
		hintLabel = CustomLabel::create(text, 0, globalPosition.y, hintWidth, getHeight(), GetInterface()->root, TEXT_MIDDLE);
		hintLabel->SetColor(hintLabel->GetColor(WIDGETCOLOR_BACKGROUND) * 0.9f);
		hintLabel->setHeight(hintHeight);
		hintLabel->SetHidden(true);
		hintLabel->setBackground(true);
	} else {
		hintLabel->SetText(text);
		hintLabel->setSize(hintWidth, hintHeight);
	}
	if (doAutoFontScale)
		hintLabel->setAutoFontScale();
	auto hintFontScale = hintLabel->getFontScale();
	if (scaleHeight) {
		hintHeight = Round(float(GetInterface()->GetFontHeight(GetInterface()->font, hintFontScale, fontweight)));
		hintHeight = hintHeight * subStrings.size() + Round((float)subStrings.size() * (float)hintHeight * 0.2f);
		hintLabel->setHeight(hintHeight);
	}
	if (scaleWidth) {
		hintWidth = 0;
		for (auto& subString : subStrings) {
			int newWidth = GetInterface()->GetTextWidth(GetInterface()->font, hintFontScale, subString, 1);
			if (newWidth > hintWidth) hintWidth = newWidth;
		}
		hintWidth = hintWidth != 0 ? (hintWidth + indent * 2) : getWidth();
		hintLabel->setWidth(hintWidth);
	}
}

void CustomWidget::setLocalHintText(const WString& text, bool scaleWidth, bool scaleHeight, bool doAutoFontScale, int hintHeight) {
	SetInteractive(true);
	auto localText = ResourceManager::getInstance()->getLocalString(text);
	setHintText(localText, scaleWidth, scaleHeight, doAutoFontScale, hintHeight);
}

bool CustomWidget::eventHintCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra) {
	auto widget = extra->As<CustomWidget>();
	if (widget->hoverState)
		widget->hintLabel->SetHidden(false);
	if (widget->hintTimer) {
		widget->hintTimer->Stop();
		widget->hintTimer = nullptr;
	}
	return false;
}

int CustomWidget::getIndent() {
	return indent;
}


Vec4 CustomWidget::getVec4Color(unsigned int col) {
	return Vec4((float)Red(col) / 255.0f, (float)Green(col) / 255.0f, (float)Blue(col) / 255.0f, (float)Alpha(col) / 255.0f);
}

Vec4 CustomWidget::getVec4Color(iVec4 color) {
	return Vec4(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, color.w / 255.0f);
}

unsigned int CustomWidget::getRgbaColor(WidgetColor colorType) {
	//return Rgba(color[col].r * 255, color[col].g * 255, color[col].b * 255, color[col].a * 255);
	return getRgbaColor(color[colorType]);
}

unsigned int CustomWidget::getRgbaColor(Vec4 color) {
	return Rgba(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
}

float CustomWidget::getFontScale() {
	return fontscale;
}

void CustomWidget::MouseMove(const int x, const int y) {
}

bool CustomWidget::isCursorInWidgetArea() {
	if (!GetInterface() || !ActiveWindow()) return false;
	auto mousePosition = ActiveWindow()->GetMousePosition();
	auto widgetPosition = GetPosition(true);
	if (mousePosition.x < widgetPosition.x || mousePosition.y < widgetPosition.y
		|| mousePosition.x >= (getWidth() + widgetPosition.x)
		|| mousePosition.y >= (getHeight() + widgetPosition.y)) {
		return false;
	}
	return true;
}

void CustomWidget::MouseEnter(const int x, const int y) {
	if (hintLabel) {
		auto globalPosition = GetPosition(true);
		globalPosition.y = globalPosition.y + getHeight() + indent;
		hintLabel->setPosition(globalPosition);
		int windowSizeX = GetInterface()->background->size.width;
		int windowSizeY = GetInterface()->background->size.height;
		int deltaX = hintLabel->getPositionX(true) + hintLabel->getWidth() - windowSizeX;
		if (deltaX > 0)
			hintLabel->setPositionX(hintLabel->getPositionX() - deltaX - 1);
		int deltaY = hintLabel->getPositionY(true) + hintLabel->getHeight() - windowSizeY;
		if (deltaY > 0)
			hintLabel->setPositionY(hintLabel->getPositionY() - deltaY - 1);
		if (hintTimer) hintTimer->Stop();
		hintTimer = UltraEngine::CreateTimer(HINT_MAX_TIME);
		ListenEvent(EVENT_TIMERTICK, hintTimer, eventHintCallback, Self());
	}
	if (GetInterface() && !GetInterface()->MouseDown(MOUSE_LEFT)) {
		hoverState = true;
		Redraw();
	}
}

void CustomWidget::MouseLeave(const int x, const int y) {
	if (hintLabel) hintLabel->SetHidden(true);
	if (hintTimer) hintTimer->Stop();
	hintTimer = nullptr;
	hoverState = false;
}

void CustomWidget::MouseDown(const MouseButton button, const int x, const int y) {
	Widget::MouseDown(button, x, y);
}

void CustomWidget::MouseUp(const MouseButton button, const int x, const int y) {
	Widget::MouseUp(button, x, y);
}

void CustomWidget::LoseFocus() {
}

void CustomWidget::DoubleClick(const MouseButton button, const int x, const int y) {
	Widget::DoubleClick(button, x, y);
}

void CustomWidget::TripleClick(const MouseButton button, const int x, const int y) {
	Widget::TripleClick(button, x, y);
}

void CustomWidget::GainFocus() {
}

bool CustomWidget::KeyDown(const KeyCode key) {
	return Widget::KeyDown(key);
}

void CustomWidget::KeyUp(const KeyCode key) {
	Widget::KeyUp(key);
}

void CustomWidget::SetText(const WString& text) {
	if (CustomWidget::text != text) {
		Widget::SetText(text);
	}
}

void CustomWidget::KeyChar(const int keychar) {
}

void CustomWidget::MouseWheel(const int delta, const int x, const int y) {
}

void CustomWidget::Draw(const int x, const int y, const int width, const int height) {
	blocks.clear();
	//auto size = iVec2(width, height);
	float cornerRad = (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0;
	//Background
	int blockId = AddBlock(iVec2(0), size, color[WIDGETCOLOR_BACKGROUND], false, cornerRad);
	blocks[blockId].hidden = !hasWidgetStyle(CustomWidgetStyle::WIDGET_BACKGROUND);
	//Image
	drawPixmap(x, y, size.width, size.height);

	//Border
	blockId = AddBlock(iVec2(0), size, color[WIDGETCOLOR_BORDER], true, cornerRad);
	blocks[blockId].hidden = !hasWidgetStyle(WIDGET_BORDER);

	//Text
	int textOffsetY = 0;
	if (hasWidgetStyle(WIDGET_BORDER)) {
		//textOffsetY = 1;
	}
	AddBlock(text, iVec2(0, textOffsetY), size, color[WIDGETCOLOR_FOREGROUND], hasWidgetStyle(WIDGET_TEXT_LEFT) ? (textAlignment - TEXT_CENTER) : textAlignment);
}

void CustomWidget::drawPixmap(const int x, const int y, const int width, const int height) {
	auto size = iVec2(width, height);
	int blockId = 1;
	if (pixmap) {
		blockId = AddBlock(pixmap, iVec2(0), Vec4(1));
		blocks[blockId].size = iVec2(size);
		blocks[blockId].filter = pixmapfilter;

		float aspect, imageaspect;
		switch (pixmapalignment) {
		case PIXMAP_CONTAIN:
			aspect = float(size.y) / float(size.x);
			imageaspect = float(pixmap->size.y) / float(pixmap->size.x);
			if (aspect > imageaspect) {
				blocks[blockId].size.x = Min(size.x, pixmap->size.x) - padding[blockId] - padding[0];
				blocks[blockId].size.y = Round(float(blocks[blockId].size.x) * imageaspect);
			} else {
				blocks[blockId].size.y = Min(size.y, pixmap->size.y) - padding[2] - padding[3];
				blocks[blockId].size.x = Round(float(blocks[blockId].size.y) / imageaspect);
			}
			blocks[blockId].position.x = Round(float(size.x - padding[0] - blocks[blockId].size.x) / 2.0f);
			blocks[blockId].position.y = Round(float(size.y - padding[2] - blocks[blockId].size.y) / 2.0f);
			break;
		case PIXMAP_COVER:
			aspect = float(size.y) / float(size.x);
			imageaspect = float(pixmap->size.y) / float(pixmap->size.x);
			if (aspect < imageaspect) {
				blocks[blockId].position.x = 0;
				blocks[blockId].size.x = size.x;
				blocks[blockId].size.y = float(size.x) * imageaspect;
				blocks[blockId].position.y = Round(float(size.y - blocks[blockId].size.y) / 2.0f);
			} else {
				blocks[blockId].position.y = 0;
				blocks[blockId].size.y = size.y;
				blocks[blockId].size.x = float(size.y) / imageaspect;
				blocks[blockId].position.x = Round(float(size.x - blocks[blockId].size.x) / 2.0f);
			}
			break;
		case PIXMAP_STRETCH:
			blocks[blockId].position.x = 0;
			blocks[blockId].position.y = 0;
			blocks[blockId].size = size;
			break;
		case PIXMAP_CENTER:
			blocks[blockId].position.x = Round(float(size.x - pixmap->size.x) / 2.0f);
			blocks[blockId].position.y = Round(float(size.y - pixmap->size.y) / 2.0f);
			blocks[blockId].size = pixmap->size;
			break;
		default:
			blocks[blockId].position.x = padding[0];
			blocks[blockId].position.y = padding[2];
			blocks[blockId].size.x = pixmap->size.x - padding[blockId] - padding[0];
			blocks[blockId].size.y = pixmap->size.y - padding[3] - padding[2];
			break;
		}
		blocks[blockId].hidden = false;
		//blocks[blockId].color = Vec4(blockId);
		blocks[blockId].SetPixmap(pixmap);
	} else {
		blockId = AddBlock(iVec2(0), size);
		blocks[blockId].hidden = true;
	}
}

int CustomWidget::getWidth() {
	return GetSize().width;
}

void CustomWidget::setWidth(int width) {
	setSize(width, getHeight());
}

int CustomWidget::getHeight() {
	return GetSize().height;
}

void CustomWidget::setHeight(int height) {
	setSize(getWidth(), height);
}

void CustomWidget::setSize(int width, int height) {
	if (width > 0 && height > 0) {
		SetShape(GetPosition(), iVec2(width, height));
	}
}

void CustomWidget::setSize(iVec2 size) {
	if (size.width > 0 && size.height > 0) {
		SetShape(GetPosition(), size);
	}
}

int CustomWidget::getPositionX(bool isGlobal) {
	return GetPosition(isGlobal).x;
}

void CustomWidget::setPositionX(int x) {
	setPosition(x, getPositionY());
}

int CustomWidget::getPositionY(bool isGlobal) {
	return GetPosition(isGlobal).y;
}

void CustomWidget::setPositionY(int y) {
	setPosition(getPositionX(), y);
}

void CustomWidget::setPosition(int x, int y) {
	//if ((x < 0 - getWidth() / 2 || y < 0 - getHeight() / 2) && !GetHidden())
	{
		//SetHidden(true);
	}
	//if (x >= 0 && y >= 0 && !(x == getPositionX() && y == getPositionY()))
	{
		SetShape(iVec2(x, y), GetSize());
		//if (GetHidden()) SetHidden(false);
	}
}

void CustomWidget::setPosition(iVec2 position) {
	SetShape(position, GetSize());
}
