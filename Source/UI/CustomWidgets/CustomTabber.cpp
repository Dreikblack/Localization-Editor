#include "UltraEngine.h"
#include "CustomTabber.h"
#include "CustomButton.h"
#include "ButtonPanel.h"
#include "../../Managers/ResourceManager.h"
#include "../../Managers/SettingsManager.h"

CustomTabber::CustomTabber()
{
	blocks.resize(2);//background, border
	buttonHeight = GUI_SCALE / 2;
	buttonWidth = GUI_SCALE;
}


shared_ptr<CustomTabber> CustomTabber::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	struct Struct : public CustomTabber {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, style);
	return instance;
}

bool CustomTabber::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	bool flag = CustomWidget::Initialize("", x, y, width, height, parent, style);
	buttonHeight = guiScale / 2;
	buttonWidth = guiScale;
	buttonPanel = ButtonPanel::create(0, 0, width, buttonWidth, buttonHeight,
		Self()->As<CustomTabber>(), BUTTON_PANEL_LEFT_TOP | BUTTON_PANEL_AGILE_ARROWS | BUTTON_PANEL_NO_INDENT);
	return flag;
}

int CustomTabber::addTab(WString tabName)
{
	auto localText = ResourceManager::getInstance()->getLocalString(tabName);
	auto settingsManager = SettingsManager::getInstance();
	float fonstScale = (float)buttonHeight / (float)settingsManager->fontHeight;
	auto button = CustomButton::create(containerOffsetX, 0, buttonWidth, buttonHeight, buttonPanel, WIDGET_BORDER | WIDGET_BACKGROUND);
	//button->SetColor(color[WIDGETCOLOR_BORDER], WIDGETCOLOR_BORDER);
	button->SetColor(Vec4(0), WIDGETCOLOR_BORDER);
	int textWidth = GetInterface()->GetTextWidth(GetInterface()->font, button->getFontScale(), localText, 1) + indent;
	button->setWidth(textWidth);
	button->SetText(localText);
	std::weak_ptr<CustomButton> buttonWeak(button);
	button->setListener(
		[this, buttonWeak](Event event) {
			buttonPanel->select(buttonWeak.lock(), true);
			switchTab();
			return true;
		});
	tabButtons.push_back(button);
	buttonPanel->addButton(button);
	auto container = Container::create(0, buttonHeight, getWidth(), getHeight() - buttonHeight, Self()->As<CustomTabber>(), CONTAINER_SCROLL);
	container->SetHidden(true);
	tabsContainers.push_back(container);
	return (tabsContainers.size() - 1);
}

void CustomTabber::switchTab() 
{
	for (int i = 0; i < tabButtons.size(); i++) 
	{
		if (tabButtons[i] == buttonPanel->getSelectedButton()) {
			tabsContainers[i]->SetHidden(false);
		}
		else {
			tabsContainers[i]->SetHidden(true);
		}
	}
}

void CustomTabber::addWidget(int index, shared_ptr<Widget> widget) 
{
	if (index < tabsContainers.size() && tabsContainers[index]) 
	{
		tabsContainers[index]->addWidget(widget);
	}
}

void CustomTabber::select(int index)
{
	if (index < tabButtons.size() && tabButtons[index]) 
	{
		tabButtons[index]->useListener();
	}
}

int CustomTabber::getTabIndex()
{
	for (int i = 0; i < tabButtons.size(); i++) 
	{
		if (tabButtons[i] == buttonPanel->getSelectedButton()) return i;
	}
	return 0;
}

int CustomTabber::getButtonHeight()
{
	return buttonHeight;
}

int CustomTabber::getContainerElementCount(int index)
{
	if (index < tabsContainers.size() && tabsContainers[index]) {
		return tabsContainers[index]->getElementCount();
	}
	return 0;
}

void CustomTabber::removeWidget(int index, shared_ptr<Widget> widget)
{
	if (index < tabsContainers.size() && tabsContainers[index]) {
		tabsContainers[index]->removeWidget(widget);
	}
}

void CustomTabber::Draw(const int x, const int y, const int width, const int height)
{
	auto size = iVec2(width, height);
	size.height = size.height - buttonHeight;
	auto position = iVec2(0, buttonHeight);
	for (auto& block : blocks)
	{
		block.hidden = true;
	}
	//Background
	if (hasWidgetStyle(WIDGET_BACKGROUND)) {
		blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
		blocks[0].wireframe = false;
		blocks[0].position = position;
		blocks[0].size = size;
		blocks[0].hidden = false;
	}

	//Border
	if (hasWidgetStyle(WIDGET_BORDER)) {
		blocks[1].hidden = false;
		blocks[1].color = color[WIDGETCOLOR_BORDER];
		blocks[1].wireframe = true;
		blocks[1].position = position;
		blocks[1].size = size;
		blocks[1].radius = (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0;
	}
}