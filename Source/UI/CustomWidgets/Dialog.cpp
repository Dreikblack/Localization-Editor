#include "UltraEngine.h"
#include "Dialog.h"
#include "../../Managers/ResourceManager.h"
#include "CustomButton.h"
#include "ButtonPanel.h"
#include "../../Managers/SettingsManager.h"
#include "../../Managers/InterfaceManager.h"

Dialog::Dialog() {
	blocks.resize(6);//background, image, border, text, title background, title text
}

Dialog::~Dialog() = default;

std::shared_ptr<Dialog> Dialog::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int style) {
	struct Struct : public Dialog {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, style);
	return instance;
}

bool Dialog::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, int style) {
	if (hasWidgetStyle(style, DIALOG_MODAL)) {
		modalBackgroundPanel = CreatePanel(0, 0, parent->GetInterface()->root->size.width, parent->GetInterface()->root->size.height, parent->GetInterface()->root);
		modalBackgroundPanel->SetColor(0.1f, 0.1f, 0.1f, 0.3f);
	}
	bool isInit = CustomWidget::Initialize(x, y, width < GUI_SCALE ? GUI_SCALE : width, height < GUI_SCALE ? GUI_SCALE : height, parent, WIDGET_BACKGROUND | style);
	bool isCrossOnly = hasWidgetStyle(style, DIALOG_CROSS_ONLY);
	buttonHeight = guiScale / 2;
	setContentHeight(height);

	auto settingsManager = SettingsManager::getInstance();
	SetFontScale((float)buttonHeight / (float)settingsManager->fontHeight);

	auto resourceManager = ResourceManager::getInstance();
	int okButtonWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, resourceManager->getLocalString("Menu.Dialog.Confirm"), 1) + indent * 2;
	int cancelButtonWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, resourceManager->getLocalString("Menu.Dialog.Cancel"), 1) + indent * 2;
	int buttonWidth = okButtonWidth > cancelButtonWidth ? okButtonWidth : cancelButtonWidth;
	buttonPanel = ButtonPanel::create(0, height - buttonHeight - indent, width, buttonWidth, buttonHeight,
		Self()->As<Dialog>(), BUTTON_PANEL_AGILE_ARROWS | (hasWidgetStyle(BUTTON_PANEL_CENTER) ? BUTTON_PANEL_CENTER : 0));

	okButton = CustomButton::create(width - guiScale * 2 - indent * 2, height - buttonHeight - indent, okButtonWidth, buttonHeight, Self()->As<Widget>(), WIDGET_BORDER | WIDGET_BACKGROUND);
	okButton->setLocalText("Dialog.Confirm");
	okButton->SetColor(0.8, 0.8, 0.8, 1, WIDGETCOLOR_SLIDERARROW);

	cancelButton = CustomButton::create(width - guiScale - indent, height - buttonHeight - indent, cancelButtonWidth, buttonHeight, Self()->As<Widget>(), WIDGET_BORDER | WIDGET_BACKGROUND);
	cancelButton->setLocalText("Dialog.Cancel");
	cancelButton->SetColor(0.8, 0.8, 0.8, 1, WIDGETCOLOR_SLIDERARROW);

	if (isCrossOnly) {
		int crossSize = buttonHeight;
		crossButton = CustomButton::create(width - crossSize, 0, crossSize, crossSize, Self()->As<Widget>(), WIDGET_BACKGROUND);
		crossButton->SetIcon(resourceManager->getGuiIcon("Cross"), PIXMAP_CENTER, (float)crossSize / (float)GUI_SCALE);
	}

	buttonPanel->addButtons({ okButton, cancelButton });
	if (isCrossOnly)
		buttonPanel->SetHidden(true);

	container = Container::create(0, buttonHeight, width, contentHeight + indent, Self()->As<Dialog>(), CONTAINER_SCROLL);
	InterfaceManager::getInstance()->addDialog(As<Dialog>());
	return isInit;
}

void Dialog::SetParent(shared_ptr<Widget> parent) {
	if (!parent && modalBackgroundPanel) {
		modalBackgroundPanel->SetParent(nullptr);
	}
	if (!parent) {
		InterfaceManager::getInstance()->removeDialog(As<Dialog>());
	}
	Widget::SetParent(parent);
}

void Dialog::setTitle(const WString& title) {
	titleText = title;
}

void Dialog::setLocalTextTitle(const WString& title) {
	setTitle(ResourceManager::getInstance()->getLocalString(title));
}

void Dialog::setLocalTextOk(const WString& localText) {
	if (okButton) {
		okButton->setLocalText(localText);
	}
}

void Dialog::setLocalTextCancel(const WString& localText) {
	if (cancelButton) {
		cancelButton->setLocalText(localText);
	}
}

void Dialog::setOkListener(std::function<bool(Event)> const& listener) {
	okButton->setListener(listener);
}

void Dialog::useOkListener() {
	if (okButton) {
		okButton->useListener();
	}
}

void Dialog::setCancelListener(std::function<bool(Event)> const& listener) {
	cancelButton->setListener(listener);
	if (crossButton) crossButton->setListener(listener);
}

void Dialog::useCancelListener() {
	if (cancelButton) {
		cancelButton->useListener();
	}
}

void Dialog::removeCancelButton() {
	buttonPanel->removeButton(cancelButton);
	cancelButton->SetParent(nullptr);
	cancelButton = nullptr;
}

void Dialog::Draw(const int x, const int y, const int width, const int height) {
	for (auto& block : blocks) {
		block.hidden = true;
	}
	//Background
	if (hasWidgetStyle(WIDGET_BACKGROUND)) {
		blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
		blocks[0].wireframe = false;
		blocks[0].position = iVec2(0, buttonHeight);
		blocks[0].size = iVec2(size.x, size.y - buttonHeight);
		blocks[0].hidden = false;
	}
	//Title Background
	if (hasWidgetStyle(WIDGET_BACKGROUND)) {
		blocks[4].color = color[WIDGET_BACKGROUND] * 0.50f;
		blocks[4].color.w = 1;
		blocks[4].wireframe = false;
		blocks[4].position = iVec2(0, 0);
		blocks[4].size = iVec2(size.x, buttonHeight);
		blocks[4].hidden = false;
	}
	//Border
	if (hasWidgetStyle(WIDGET_BORDER)) {
		blocks[2].hidden = false;
		blocks[2].color = color[WIDGETCOLOR_BORDER];
		blocks[2].wireframe = true;
		blocks[2].position = iVec2(0);
		blocks[2].size = size;
		blocks[2].radius = (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0;
	}
	//Text
	blocks[3].hidden = false;
	blocks[3].position = iVec2(indent, buttonHeight + indent);
	blocks[3].size = iVec2(size.x - indent, contentHeight);
	blocks[3].SetText(text);
	blocks[3].textalignment = textAlignment;
	blocks[3].color = color[WIDGETCOLOR_FOREGROUND];

	//Title title
	blocks[5].hidden = false;
	blocks[5].position = iVec2(0, 0);
	blocks[5].size = iVec2(size.x, buttonHeight);
	blocks[5].SetText(titleText);
	blocks[5].textalignment = titleAlignment;
	blocks[5].color = color[WIDGETCOLOR_FOREGROUND];
}

void Dialog::setTitleToCenter(bool isCenter) {
	titleAlignment = isCenter ? TEXT_CENTER | TEXT_MIDDLE : TEXT_LEFT | TEXT_MIDDLE;
}

void Dialog::addWidget(shared_ptr<Widget> widget) {
	container->addWidget(widget);
}

void Dialog::removeWidget(shared_ptr<Widget> widget) {
	container->removeWidget(widget);
}

void Dialog::clear() {
	container->clear();
}

vector<shared_ptr<Widget>> Dialog::getWidgets() {
	return container->getWidgets();
}

void Dialog::SetHidden(const bool hide) {
	CustomWidget::SetHidden(hide);
	if (modalBackgroundPanel) modalBackgroundPanel->SetHidden(hide);
	if (hide) {
		InterfaceManager::getInstance()->removeDialog(As<Dialog>());
	} else {
		InterfaceManager::getInstance()->addDialog(As<Dialog>());
	}
}

void Dialog::setContentHeight(int height) {
	bool isCrossOnly = hasWidgetStyle(style, DIALOG_CROSS_ONLY);
	if (isCrossOnly) {
		contentHeight = height - buttonHeight - indent;
	} else {
		contentHeight = height - buttonHeight * 2 - indent * 3;
	}
}

void Dialog::setHeight(int height) {
	CustomWidget::setHeight(height);
	setContentHeight(height);
	container->setHeight(contentHeight + indent);
	Redraw();
}

void Dialog::setWidth(int width) {
	CustomWidget::setWidth(width);
	container->setWidth(width);
	if (crossButton) {
		crossButton->setPositionX(width - buttonHeight);
	}
}

void Dialog::KeyUp(const KeyCode key) {
	CustomWidget::KeyUp(key);
	//if (key == KEY_ESCAPE && hasWidgetStyle(style, DIALOG_ESC_CLOSE)) {
	//	if (crossButton) {
	//		crossButton->useListener();
	//	} else if (cancelButton) {
	//		cancelButton->useListener();
	//	}
	//}
}

void Dialog::updateModelPanel() {
	if (modalBackgroundPanel) {
		modalBackgroundPanel->SetShape(0, 0, GetInterface()->root->size.width, GetInterface()->root->size.height);
	}
}
