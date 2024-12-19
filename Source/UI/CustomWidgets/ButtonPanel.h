#pragma once

#include "UltraEngine.h"
#include "CustomWidget.h"
#include "CustomButton.h"
#include "Container.h"


enum ButtonPanelStyle
{
	BUTTON_PANEL_VERTICAL = 16,
	BUTTON_PANEL_LEFT_TOP = 32,
	BUTTON_PANEL_CENTER = 64,
	//hide arrow buttons if usual buttons can be seen
	BUTTON_PANEL_AGILE_ARROWS = 128,
	BUTTON_PANEL_NO_INDENT = 256,
};

class TriangleButton;

/**  
* \brief List like panel for **Custom Buttons**
* \author Dreikblack
* \date January of 2022
* \details Panel for **Custom Buttons**  that looks like a liniar list and controlled by triangle buttons at beginning and at end of the list.
* Those triangle buttons select button and move buttons if selected button can't be seen
*/
class ButtonPanel : public CustomWidget
{
protected:
	ButtonPanel();
	unsigned int triangleColor = 0;
	shared_ptr<Container> container;
	int getContainerLength();
	int buttonWidth = 0;
	int buttonHeight = 0;
	bool doCycle = false;
	list<shared_ptr<CustomButton>> buttonList;
	shared_ptr <TriangleButton> firstTriangleButton;
	shared_ptr <TriangleButton> secondTriangleButton;
	iVec2 getTrianglePosition(bool isFirst);
	/**
	* create arrow button that will be used to navigate
	*
	* \param isFirst is Arrow left/top or right/buttom
	*/
	void createTriangleButton(bool isFirst = true);
	shared_ptr<CustomButton> selectedButton;
	int getButtonThickness();
	int getButtonLength();
	int getButtonLength(shared_ptr<CustomButton> button);
	KeyCode firstArrowKey;
	KeyCode secondArrowKey;
	virtual void selectButton(bool isForward);
	virtual bool doSelectButton(list<shared_ptr<CustomButton>>::iterator iter);
	bool KeyDown(const KeyCode key) override;
	virtual bool Initialize(const int x, const int y, const int maxLength, int buttonWidth, int buttonHeight, shared_ptr<Widget> parent, unsigned int color, const int style);
public:
	int maxLength = 0;
	static shared_ptr<ButtonPanel> create(const int x, const int y, const int maxLength, int buttonWidth, int buttonHeight, shared_ptr<Widget> parent, const int style, unsigned int color = Rgba(100, 100, 0, 255));
	void setButtonHeight(int height);
	int getLength();
	void addButtons(vector<shared_ptr<CustomButton>> buttons);
	void addButton(shared_ptr<CustomButton> button);
	vector<shared_ptr<Widget>> getButtons();
	void insertButton(shared_ptr<CustomButton> button, int index);
	void removeButton(shared_ptr<CustomButton> button);
	void updateContentPosition();
	void clear();
	void selectPrevious();
	void selectNext();
	void selectFirst();
	virtual void select(shared_ptr<CustomButton> button, bool doSwitchInteractive = false);
	void deselect();
	void setFirstArrowKey(KeyCode key);
	void setSecondArrowKey(KeyCode key);
	void useButtton();
	virtual void draw();
	shared_ptr<CustomButton> getSelectedButton();
	void updateHotkeyLabels();
};


