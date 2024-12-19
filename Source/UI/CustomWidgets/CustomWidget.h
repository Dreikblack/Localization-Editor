#pragma once
#include "../../Interface/AllConstants.h"

using namespace UltraEngine;

class CustomLabel;

enum CustomWidgetStyle {
	WIDGET_DEFAULT = 0,
	WIDGET_BORDER = 1,
	WIDGET_BORDER_ROUNDED = 2,
	WIDGET_BACKGROUND = 4,
	WIDGET_TEXT_LEFT = 8
};
class CustomWidget : public Widget {
protected:
	int indent = GUI_SCALE / 8;
	int guiScale = GUI_SCALE;
	bool isAutoFontScale = false;
	bool hoverState;
	shared_ptr<CustomLabel> hintLabel;
	WString hintText;
	shared_ptr<Timer> hintTimer;
	static bool eventHintCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra);

	//Called when the mouse cursor enters the widget bounds
	void MouseEnter(const int x, const int y) override;
	//Called when the mouse cursor leaves the widget bounds
	void MouseLeave(const int x, const int y) override;
	//Called when another widget becomes selected
	void LoseFocus() override;
	//Called when mouse double-click occurs
	void DoubleClick(const MouseButton button, const int x, const int y) override;
	//Called when mouse triple-click occurs
	void TripleClick(const MouseButton button, const int x, const int y) override;
	//Called when widget is selected
	void GainFocus() override;
	//Called for each keydown event
	void KeyChar(const int keychar) override;
	//Called when mouse wheel turns and mouse is hovered over this widget
	void MouseWheel(const int delta, const int x, const int y) override;
	//Called each time the widget is redrawn
	void Draw(const int x, const int y, const int width, const int height) override;
	virtual void drawPixmap(const int x, const int y, const int width, const int height);
public:
	CustomWidget();
	~CustomWidget() override;
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0);
	bool Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0) override;
	float getFontScale();
	//Called when the mouse moves if this widget has the focus
	void MouseMove(const int x, const int y) override;
	//Called when the mouse button is pressed
	void MouseDown(const MouseButton button, const int x, const int y) override;
	//Called when the mouse button is released
	void MouseUp(const MouseButton button, const int x, const int y) override;
	//indent, tileSize
	virtual void updateScale();
	virtual void setAutoFontScale();
	bool isCursorInWidgetArea();
	//Called when key is pressed
	bool KeyDown(const KeyCode key) override;
	//Called when key is released
	void KeyUp(const KeyCode key) override;
	void SetText(const WString& text) override;
	//has widget this style (made this method for easier and more intuitive check)
	bool hasWidgetStyle(int widgetStyle, int style);
	//has widget this style (made this method for easier and more intuitive check)
	bool hasWidgetStyle(int style);
	int textAlignment = TEXT_CENTER | TEXT_MIDDLE;
	void setLocalText(const WString& text, bool scaleWidth = false);
	void setHintText(const WString& text, bool scaleWidth = false, bool scaleHeight = true, bool doAutoFontScale = false, int hintHeight = -1);
	void setLocalHintText(const WString& text, bool scaleWidth = false, bool scaleHeight = true, bool doAutoFontScale = false, int hintHeight = -1);
	int getIndent();
	int getWidth();
	void setWidth(int width);
	int getHeight();
	virtual void setHeight(int height);
	void setSize(int width, int height);
	void setSize(iVec2 size);
	int getPositionX(bool isGlobal = false);
	void setPositionX(int x);
	int getPositionY(bool isGlobal = false);
	void setPositionY(int y);
	virtual void setPosition(int x, int y);
	void setPosition(iVec2 position);
	unsigned int getRgbaColor(WidgetColor col);
	unsigned int getRgbaColor(Vec4 color);
	Vec4 getVec4Color(unsigned int col);
	Vec4 getVec4Color(iVec4 color);
};

