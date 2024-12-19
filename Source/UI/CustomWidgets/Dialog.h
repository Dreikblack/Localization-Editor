#pragma once

#include "CustomWidget.h"
#include "Container.h"

class CustomButton;
class ButtonPanel;

enum CustomDialogStyle {
	DIALOG_MODAL = 16,
	DIALOG_CROSS_ONLY = 32,
	DIALOG_ESC_CLOSE = 64,
};

class Dialog : public CustomWidget {
protected:
	Dialog();
	~Dialog() override;
	int titleAlignment = TEXT_CENTER | TEXT_MIDDLE;
	WString titleText = "";
	int buttonHeight = 32;
	int contentHeight = 128;

	shared_ptr<ButtonPanel> buttonPanel;
	shared_ptr<CustomButton> okButton;
	shared_ptr<CustomButton> cancelButton;
	shared_ptr<CustomButton> crossButton;
	shared_ptr<Container> container;
	shared_ptr<Widget> modalBackgroundPanel;
	void setContentHeight(int height);
	//Called each time the widget is redrawn
	void Draw(const int x, const int y, const int width, const int height) override;
public:
	static std::shared_ptr<Dialog> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int style = 0);
	bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int style = 0) override;
	void SetParent(shared_ptr<Widget> parent) override;
	void setTitle(const WString& title);
	void setLocalTextTitle(const WString& title);
	void setLocalTextOk(const WString& localText);
	void setLocalTextCancel(const WString& localText);
	void setOkListener(std::function<bool(Event)> const& listener);
	void useOkListener();
	void setCancelListener(std::function<bool(Event)> const& listener);
	void useCancelListener();
	void removeCancelButton();
	void setTitleToCenter(bool isCenter);
	void addWidget(shared_ptr<Widget> widget);
	void removeWidget(shared_ptr<Widget> widget);
	void clear();
	vector<shared_ptr<Widget>> getWidgets();
	void SetHidden(const bool hide) override;
	void setHeight(int height) override;
	void setWidth(int width);
	void KeyUp(const KeyCode key) override;
	void updateModelPanel();
};

