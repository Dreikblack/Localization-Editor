#pragma once
#include "UltraEngine.h"
#include "CustomWidget.h"
#include "Container.h"

using namespace UltraEngine;

class CustomButton;
class ButtonPanel;

class CustomTabber : public CustomWidget
{
protected:
	CustomTabber();
	int buttonHeight;
	int buttonWidth;

	shared_ptr<ButtonPanel> buttonPanel;
	vector<shared_ptr<Container>> tabsContainers;
	vector<shared_ptr<CustomButton>> tabButtons;

	void switchTab();
	virtual void Draw(const int x, const int y, const int width, const int height);
public:
	int containerOffsetX = 0;
	static std::shared_ptr<CustomTabber> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int style = 0);
	bool virtual Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int style = 0);
	int addTab(WString name);
	void addWidget(int index, shared_ptr<Widget> widget);
	void removeWidget(int index, shared_ptr<Widget> widget);
	int getContainerElementCount(int index);
	void select(int index);
	int getTabIndex();
	int getButtonHeight();
};
