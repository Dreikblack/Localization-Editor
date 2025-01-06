#pragma once
#include "UltraEngine.h"
#include "CustomListBox.h"

template<typename Value>
CustomListBox<Value>::CustomListBox() {
	blocks.resize(initBlockCount);//background, border, highlight background for selected item, highlight under cursor
	textAlignment = TEXT_MIDDLE;
}

template<typename Value>
CustomListBox<Value>::~CustomListBox() {
}

template<typename Value>
std::shared_ptr<CustomListBox<Value>> CustomListBox<Value>::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	struct Struct : public CustomListBox<Value> {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, WIDGET_BACKGROUND | WIDGET_BORDER | style);
	return instance;
}

template<typename Value>
bool CustomListBox<Value>::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) {
	bool isInit = CustomWidget::Initialize("", x, y, width, height, parent, style);
	itemSize = iVec2(width, getItemHeight());
	int scrollWidth = indent * 2;
	verticalSlider = CustomSlider::create(width - scrollWidth, 0, scrollWidth, height, Self()->As<CustomListBox<Value>>());
	verticalSlider->SetHidden(true);
	verticalSlider->setListener([this](Event event) {
		extraItemCountInView = verticalSlider->getCurrentValue() - 1;
		Redraw();
		return true;
		});
	return isInit;
}

template<typename Value>
int CustomListBox<Value>::getItemHeight() {
	if (!GetInterface()) {
		return guiScale / 2;
	}
	return Round(float(GetInterface()->GetFontHeight(GetInterface()->font, fontscale, fontweight)));
}

template<typename Value>
void CustomListBox<Value>::Draw(const int x, const int y, const int width, const int height) {
	for (auto& block : blocks) {
		block.hidden = true;
	}
	//Background
	if (hasWidgetStyle(WIDGET_BACKGROUND)) {
		blocks[0].color = color[WIDGETCOLOR_SUNKEN];
		blocks[0].wireframe = false;
		blocks[0].position = iVec2(0);
		blocks[0].size = size;
		blocks[0].hidden = false;
	}
	//Border
	if (hasWidgetStyle(WIDGET_BORDER)) {
		blocks[1].hidden = false;
		blocks[1].color = color[WIDGETCOLOR_BORDER];
		blocks[1].wireframe = true;
		blocks[1].position = iVec2(0);
		blocks[1].size = size;
		blocks[1].radius = (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0;
	}
	//Highlight for selected
	if (selectedItemId >= 0) {
		blocks[2].color = color[WIDGETCOLOR_SELECTION];
		blocks[2].wireframe = false;
		blocks[2].position = iVec2(0, itemSize.height * (selectedItemId - extraItemCountInView));
		blocks[2].size = itemSize;
		blocks[2].hidden = false;
	}
	//Highlight under cursor
	if (highlightItemId >= 0 && highlightItemId != selectedItemId) {
		blocks[3].color = color[WIDGETCOLOR_HIGHLIGHT];
		blocks[3].wireframe = false;
		blocks[3].position = iVec2(0, itemSize.height * (highlightItemId - extraItemCountInView));
		blocks[3].size = itemSize;
		blocks[3].hidden = false;
	}
	// listBoxItems
	auto iterItem = listBoxItems.begin();
	for (int i = initBlockCount; i < blocks.size(); i++) {
		blocks[i].hidden = false;
		blocks[i].position = iVec2(indent, itemSize.height * (i - initBlockCount - extraItemCountInView));
		blocks[i].size = itemSize;
		blocks[i].SetText(iterItem->second);
		blocks[i].textalignment = textAlignment;
		blocks[i].color = color[WIDGETCOLOR_SELECTEDTEXT];
		iterItem++;
	}

}

template<typename Value>
void CustomListBox<Value>::MouseDown(const MouseButton button, const int x, const int y) {
	if (button == MOUSE_LEFT) {
		if (x >= 0 and y >= 0 and x < size.x and y < size.y) {
			int itemId = y / getItemHeight() + extraItemCountInView;
			if (itemId >= 0 and itemId < listBoxItems.size()) {
				selectedItemId = itemId;
				Redraw();
				useItem();
			}
		}
	}
}

template<typename Value>
void CustomListBox<Value>::DoubleClick(const MouseButton button, const int x, const int y) {
	if (button == MOUSE_LEFT) {
		if (doubleClickItemListener) {
			doubleClickItemListener(Event(EVENT_WIDGETACTION, Self(), selectedItemId));
		}
	}
}

template<typename Value>
void CustomListBox<Value>::MouseMove(const int x, const int y) {
	int oldValue = highlightItemId;
	if (x >= 0 and y >= 0 and x < size.x and y < size.y) {
		int itemId = y / getItemHeight() + extraItemCountInView;
		if (itemId >= 0 and itemId < listBoxItems.size()) {
			highlightItemId = itemId;
		}
		else {
			highlightItemId = -1;
		}
	}
	else {
		highlightItemId = -1;
	}
	if (oldValue != highlightItemId) {
		Redraw();
	}
}

template<typename Value>
void CustomListBox<Value>::updateScrollbar() {
	if (getExtraTotalItemCount() > 0) {
		verticalSlider->setHeight(getHeight());
		verticalSlider->setMaxValue(getExtraTotalItemCount() + 1);
		verticalSlider->SetHidden(false);
	}
	else {
		extraItemCountInView = 0;
		verticalSlider->SetHidden(true);
	}
}

template<typename Value>
int CustomListBox<Value>::getExtraTotalItemCount() {
	int lastItemInView = Floor((float)getHeight() / (float)getItemHeight());
	return (listBoxItems.size() - lastItemInView) >= 0 ? (listBoxItems.size() - lastItemInView) : 0;
}

template<typename Value>
void CustomListBox<Value>::MouseWheel(const int delta, const int x, const int y) {
	if (verticalSlider->GetHidden()) {
		return;
	}
	if (delta > 0) {
		verticalSlider->next();
	}
	else {
		verticalSlider->previous();
	}
}

template<typename Value>
void CustomListBox<Value>::SetHidden(const bool hide) {
	CustomWidget::SetHidden(hide);
	if (!hide) {
		highlightItemId = -1;
	}
}

template<typename Value>
void CustomListBox<Value>::addItem(std::pair<Value, WString> item) {
	listBoxItems.push_back(item);
	blocks.resize(initBlockCount + listBoxItems.size());
	updateScrollbar();
}

template<typename Value>
void CustomListBox<Value>::addItem(Value value, WString const& name) {
	addItem(std::make_pair(value, name));
}

template<typename Value>
void CustomListBox<Value>::addItems(vector<std::pair<Value, WString>> newItems) {
	listBoxItems.insert(listBoxItems.end(), newItems.begin(), newItems.end());
	blocks.resize(initBlockCount + listBoxItems.size());
	updateScrollbar();
}

template<typename Value>
void CustomListBox<Value>::selectItem(std::pair<Value, WString> item) {
	int i = 0;
	for (std::pair<Value, WString>& currentItem : listBoxItems) {
		if (currentItem == item) {
			selectedItemId = i;
			break;
		}
		i++;
	}
}

template<typename Value>
void CustomListBox<Value>::selectFirst() {
	if (listBoxItems.empty()) {
		return;
	}
	selectedItemId = 0;
	Redraw();
}

template<typename Value>
void CustomListBox<Value>::deselect() {
	if (listBoxItems.empty()) {
		return;
	}
	selectedItemId = -1;
	highlightItemId = -1;
	Redraw();
}

template<typename Value>
void CustomListBox<Value>::clearItems() {
	listBoxItems.clear();
	blocks.resize(initBlockCount);
	updateScrollbar();
	selectedItemId = -1;
	highlightItemId = -1;
}

template<typename Value>
vector<std::pair<Value, WString>> CustomListBox<Value>::getItems() {
	return listBoxItems;
}

template<typename Value>
std::pair<Value, WString> CustomListBox<Value>::getSelectedItem() {
	if (selectedItemId >= 0 && selectedItemId < getItemCount()) {
		return listBoxItems[selectedItemId];
	}
	return std::make_pair(Value(), WString(""));
}

template<typename Value>
int CustomListBox<Value>::getItemCount() {
	return listBoxItems.size();
}

template<typename Value>
void CustomListBox<Value>::useItem() {
	if (pickItemListener) {
		pickItemListener(Event(EVENT_WIDGETACTION, Self(), selectedItemId));
	}
}

template<typename Value>
void CustomListBox<Value>::setListener(std::function<bool(Event)> const& listener) {
	pickItemListener = listener;
}

template<typename Value>
void CustomListBox<Value>::setDoubleClickListener(std::function<bool(Event)> const& listener) {
	doubleClickItemListener = listener;
}

template<typename Value>
void CustomListBox<Value>::setHeight(int height) {
	CustomWidget::setHeight(height);
	updateScrollbar();
}

template<typename Value>
void CustomListBox<Value>::SetFontScale(const float scale) {
	CustomWidget::SetFontScale(scale);
	itemSize = iVec2(getWidth(), getItemHeight());
}