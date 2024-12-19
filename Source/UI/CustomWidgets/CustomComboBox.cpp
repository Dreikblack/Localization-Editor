#include "UltraEngine.h"
#include "CustomComboBox.h"
#include "Paint.h"
#include "../../Managers/SettingsManager.h"
#include "../../Managers/ResourceManager.h"

template<typename Value>
CustomComboBox<Value>::CustomComboBox<Value>()
{
	textAlignment = TEXT_MIDDLE;
}

template<typename Value>
CustomComboBox<Value>::~CustomComboBox<Value>()
{
}

template<typename Value>
std::shared_ptr<CustomComboBox<Value>> CustomComboBox<Value>::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	struct Struct : public CustomComboBox<Value> {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, WIDGET_BACKGROUND | WIDGET_BORDER | style);
	return instance;
}

template<typename Value>
void CustomComboBox<Value>::SetParent(shared_ptr<Widget> parent)
{
	Widget::SetParent(parent);
	auto globalPosition = GetPosition(true);
	globalPosition.y = globalPosition.y + getHeight();
	listBox->setPosition(globalPosition);
}

template<typename Value>
bool CustomComboBox<Value>::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	bool isInit = CustomWidget::Initialize("", x, y, width, height, parent, style);
	setAutoFontScale();
	auto globalPosition = GetPosition(true);
	globalPosition.y = globalPosition.y + getHeight();
	listBox = CustomListBox<Value>::create(globalPosition.x, globalPosition.y, width, guiScale, GetInterface()->root);
	listBox->SetHidden(true);
	listBox->setListener([this](Event event)
		{
			selectItem(listBox->getSelectedItem());
			return true;
		});
	listBox->SetFontScale(fontscale);
	auto arrowIcon = ResourceManager::getInstance()->getGuiIcon("ComboBoxArrowDown");
	auto color = GetColor(WIDGETCOLOR_SLIDERTRACK);
	arrowIcon->SetColor(color.x, color.y, color.z, color.w);
	SetIcon(arrowIcon, PIXMAP_CENTER, ((float)height / (float)GUI_SCALE));
	return isInit;
}

template<typename Value>
void CustomComboBox<Value>::addItem(std::pair<Value, WString> item)
{
	listBox->addItem(item);
	updateDropBox();
	if (selectedItem.second.empty())
	{
		selectItem(item);
	}
}

template<typename Value>
void CustomComboBox<Value>::addItems(vector<std::pair<Value, WString>> items)
{
	listBox->addItems(items);
	updateDropBox();
	if (items.size() > 0 && selectedItem.second.empty())
	{
		selectItem(items[0]);
	}
}

template<typename Value>
vector<std::pair<Value, WString>> CustomComboBox<Value>::getItems()
{
	return listBox->getItems();
}

template<typename Value>
void CustomComboBox<Value>::clearItems()
{
	selectedItem.second="";
	SetText("");
	listBox->clearItems();
}

template<typename Value>
std::pair<Value, WString> CustomComboBox<Value>::getSelectedItem()
{
	return selectedItem;
}

template<typename Value>
void CustomComboBox<Value>::closeDropDownWindow()
{
	dropDownBoxShown = false;
	listBox->SetHidden(true);
}

template<typename Value>
void CustomComboBox<Value>::setListener(std::function<bool(Event)> listener)
{
	selectItemListener = listener;
}

template<typename Value>
void CustomComboBox<Value>::selectItem(std::pair<Value, WString> item)
{
	if (!item.second.empty())
	{
		selectedItem = item;
		SetText(item.second);
		if (selectItemListener)
		{
			selectItemListener(Event(EVENT_WIDGETACTION, Self()));
		}
		listBox->selectItem(item);
	}
}

template<typename Value>
void CustomComboBox<Value>::Draw(const int x, const int y, const int width, const int height)
{
	CustomWidget::Draw(x, y, width, height);
	//Background
	blocks[0].color = color[WIDGETCOLOR_BACKGROUND];
	if (hoverState)
	{
		blocks[0].color += 0.05;
	}
	blocks[0].gradient = 0.75f;//with gradient alpha channel stops working
	//Border
	if (hoverState)
	{
		blocks[2].color = color[WIDGETCOLOR_SELECTION];
	}
	blocks[3].color = color[WIDGETCOLOR_SLIDERTRACK];
	blocks[3].color = color[WIDGETCOLOR_SELECTEDTEXT];
	blocks[3].position.x = indent;
	blocks[3].size.width = size.width - indent;
	//Arrow
	blocks[1].position.x = size.width - size.height;
	blocks[1].size.width = size.height;


}

template<typename Value>
void CustomComboBox<Value>::MouseDown(const MouseButton button, const int x, const int y)
{
	CustomWidget::MouseDown(button, x, y);
	dropDownBoxShown = listBox->getItemCount() > 0 ? !dropDownBoxShown : false;
	listBox->SetHidden(!dropDownBoxShown);
	if (dropDownBoxShown)
	{
		auto globalPosition = GetPosition(true);
		globalPosition.y = globalPosition.y + getHeight();
		listBox->setPosition(globalPosition);
	}
}

template<typename Value>
void CustomComboBox<Value>::MouseUp(const MouseButton button, const int x, const int y) {
	CustomWidget::MouseUp(button, x, y);	
}

template<typename Value>
void CustomComboBox<Value>::LoseFocus()
{
	CustomWidget::LoseFocus();
	closeDropDownWindow();
}

template<typename Value>
void CustomComboBox<Value>::MouseEnter(const int x, const int y)
{
	CustomWidget::MouseEnter(x, y);
	if (GetInterface() && !GetInterface()->MouseDown(MOUSE_LEFT))
	{
		hoverState = true;
		Redraw();
	}
}

template<typename Value>
void CustomComboBox<Value>::MouseLeave(const int x, const int y)
{
	CustomWidget::MouseLeave(x, y);
	if (hoverState) {
		hoverState = false;
		Redraw();
	}
}

template<typename Value>
void CustomComboBox<Value>::updateDropBox()
{
	auto ui = GetInterface();
	if (!ui) return;
	int height = listBox->getItemCount() * listBox->getItemHeight();
	//int windowHeight = SettingsManager::getInstance()->windowHeight - (window->display->GetSize().height - window->display->ClientArea().height);
	int windowHeight = ui->root->ClientSize().height;
	height = (height > (windowHeight - listBox->getPositionY())) ? (windowHeight - listBox->getPositionY()) : height;
	listBox->setHeight(height);
}