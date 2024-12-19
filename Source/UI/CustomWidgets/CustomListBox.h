#pragma once
#include "CustomWidget.h"
#include "CustomSlider.h"

using namespace UltraEngine;

template<typename Value>
class CustomListBox : public CustomWidget
{
friend class CodexContent;

protected:
	CustomListBox();
	~CustomListBox() override;
	bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) override;
	iVec2 itemSize = iVec2(100, 20);
	shared_ptr<CustomSlider> verticalSlider;
	//Called each time the widget is redrawn
	void Draw(const int x, const int y, const int width, const int height) override;
	//Called when the mouse button is pressed
	void MouseDown(const MouseButton button, const int x, const int y) override;
	//Called when mouse double-click occurs
	void DoubleClick(const MouseButton button, const int x, const int y) override;
	//Called when the mouse moves if this widget has the focus
	void MouseMove(const int x, const int y) override;
	int getExtraTotalItemCount();
	std::function<bool(Event)> pickItemListener;
	std::function<bool(Event)> doubleClickItemListener;
	int initBlockCount = 4;
	int selectedItemId = -1;
	int highlightItemId = -1;
	int extraItemCountInView = 0;
	vector <std::pair<Value, WString>> listBoxItems;
public:
	static std::shared_ptr<CustomListBox> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0);
	void MouseWheel(const int delta, const int x, const int y) override;
	void SetHidden(const bool hide) override;
	void updateScrollbar();
	int getItemHeight();
	void setListener(std::function<bool(Event)> const& listener);
	void setDoubleClickListener(std::function<bool(Event)> const& listener);
	void setHeight(int height) override;
	void SetFontScale(const float scale) override;
	void addItem(Value value, WString const& name);
	void addItem(std::pair<Value, WString> item);
	void addItems(vector<std::pair<Value, WString>> items);
	void selectItem(std::pair<Value, WString> item);
	void selectFirst();
	void deselect();
	void clearItems();
	vector <std::pair<Value, WString>> getItems();
	std::pair<Value, WString> getSelectedItem();
	int getItemCount();
	void useItem();
};
