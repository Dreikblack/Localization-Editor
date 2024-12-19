#pragma once
#include "CustomWidget.h"
#include "CustomListBox.h"
#include "CustomListBox.cpp"

using namespace UltraEngine;

template<typename Value>
class CustomComboBox : public CustomWidget
{
protected:
	CustomComboBox();
	~CustomComboBox() override;
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style);
	shared_ptr<CustomListBox<Value>> listBox;
	std::pair<Value, WString> selectedItem;
	std::function<bool(Event)> selectItemListener;
	bool hoverState = false;
	bool dropDownBoxShown = false;
	//Called each time the widget is redrawn
	virtual void Draw(const int x, const int y, const int width, const int height);
	//Called when the mouse button is pressed
	virtual void MouseDown(const MouseButton button, const int x, const int y);
	void MouseUp(const MouseButton button, const int x, const int y) override;
	//Called when another widget becomes selected
	virtual void LoseFocus();
	virtual void MouseEnter(const int x, const int y);
	virtual void MouseLeave(const int x, const int y);
public:
	static std::shared_ptr<CustomComboBox> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0);
	virtual void SetParent(shared_ptr<Widget> parent);
	void addItem(std::pair<Value, WString> item);
	void addItems(vector<std::pair<Value, WString>> items);
	vector <std::pair<Value, WString>> getItems();
	void updateDropBox();
	void clearItems();
	std::pair<Value, WString> getSelectedItem();
	void closeDropDownWindow();
	void setListener(std::function<bool(Event)> listener);
	void selectItem(std::pair<Value, WString> item);
};

