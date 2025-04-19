#include "UltraEngine.h"
#include "CustomTable.h"
#include "../../Managers/SettingsManager.h"

CustomTable::CustomTable() {
}

CustomTable::~CustomTable() = default;

shared_ptr<CustomTable> CustomTable::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, vector<int> columnWidthes, int rowHeight, int style) {
	struct Struct : public CustomTable {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, columnWidthes, rowHeight, style);
	return instance;
}

bool CustomTable::Initialize(const int x, const int y, const int width, const int height,shared_ptr<Widget> parent, vector<int> columnWidthes, int rowHeight, int style) {
	bool flag = Widget::Initialize("", x, y, width, height, parent, style);
	auto settingsManager = SettingsManager::getInstance();
	float fonstScale = (float)rowHeight / (float)settingsManager->fontHeight;
	SetFontScale(fonstScale);
	CustomTable::columnWidthes = columnWidthes;
	CustomTable::rowHeight = rowHeight;
	textAlignment = hasWidgetStyle(WIDGET_TEXT_LEFT) ? (textAlignment - TEXT_CENTER) : textAlignment;
	return flag;
}


void CustomTable::setData(vector<vector<WString>> dataToSet) {
	data = dataToSet;
	dataIds.resize(data.size());
	updateSize();
}

void CustomTable::setColumnWidthes(vector<int> columnWidthesToSet) {
	columnWidthes = columnWidthesToSet;
	updateSize();
}

void CustomTable::setHeaders(vector<WString> headersToSet) {
	headers = headersToSet;
	updateSize();
}

void CustomTable::updateSize() {
	int headerRowWidth = 0;
	for (int i = 0; i < headers.size(); i++) {
		int columnWidth = i < columnWidthes.size() ? columnWidthes[i] : guiScale;
		headerRowWidth = headerRowWidth + columnWidth;
	}
	if (headerRowWidth == 0) {
		headerRowWidth = getWidth();
	}
	int totalHeight = headers.empty() ? 0 : rowHeight;
	totalHeight = totalHeight + data.size() * rowHeight;
	if (totalHeight == 0) {
		totalHeight = getHeight();
	}
	setSize(headerRowWidth, totalHeight);
}

void CustomTable::Draw(const int x, const int y, const int width, const int height) {
	blocks.clear();
	if (hasWidgetStyle(CustomWidgetStyle::WIDGET_BACKGROUND)) {
		AddBlock(iVec2(0), size, color[WIDGETCOLOR_BACKGROUND], false, 0);
	}
	int offsetY = headers.empty() ? 0 : rowHeight;

	//Highlight for selected
	if (selectedItemId >= 0) {
		AddBlock(iVec2(0, offsetY + selectedItemId * rowHeight), iVec2(getWidth(), rowHeight), color[WIDGETCOLOR_SELECTION]);
	}
	//Highlight under cursor
	if (highlightItemId >= 0 && highlightItemId != selectedItemId) {
		AddBlock(iVec2(0, offsetY + highlightItemId * rowHeight), iVec2(getWidth(), rowHeight), color[WIDGETCOLOR_HIGHLIGHT]);
	}

	int headerRowWidth = 0;
	for (int i = 0; i < headers.size(); i++) {
		int columnWidth = i < columnWidthes.size()? columnWidthes[i] : guiScale;
		auto cellPosition = iVec2(headerRowWidth, 0);
		auto cellSize = iVec2(columnWidth, rowHeight);
		AddBlock(cellPosition, cellSize, color[WIDGETCOLOR_FOREGROUND], true);
		auto textBlock = AddBlock(headers[i], cellPosition, cellSize, color[WIDGETCOLOR_FOREGROUND], textAlignment);
		if (!hasWidgetStyle(TEXT_CENTER)) {
			blocks[textBlock].position.x = blocks[textBlock].position.x + indent;
			blocks[textBlock].size.x = blocks[textBlock].size.x - indent;
		}
		headerRowWidth = headerRowWidth + columnWidth;
	}

	for (int i = 0; i < dataIds.size(); i++) {
		dataIds[i] = -1;
	}
	int rowId = 0;
	int i = 0;
	for (auto const& rowData : data) {
		if (filter && !filter(rowData)) {		
			i++;
			continue;
		}
		dataIds[rowId] = i;
		int rowWidth = 0;	
		for (int i = 0; i < rowData.size(); i++) {
			int columnWidth = i < columnWidthes.size() ? columnWidthes[i] : guiScale;
			auto cellPosition = iVec2(rowWidth, offsetY + rowId * rowHeight);
			auto cellSize = iVec2(columnWidth, rowHeight);
			AddBlock(cellPosition, cellSize, color[WIDGETCOLOR_BORDER], true);
			auto textBlock = AddBlock(rowData[i], cellPosition, cellSize, color[WIDGETCOLOR_FOREGROUND], textAlignment);
			if (!hasWidgetStyle(TEXT_CENTER)) {
				blocks[textBlock].position.x = blocks[textBlock].position.x + indent;
				blocks[textBlock].size.x = blocks[textBlock].size.x - indent;
			}
			rowWidth = rowWidth + columnWidth;
		}
		rowId++;
		i++;
	}
}

void CustomTable::MouseDown(const MouseButton button, const int x, const int y) {
	int offsetY = headers.empty() ? 0 : rowHeight;
	if (button == MOUSE_LEFT && x >= 0 && y >= offsetY && x < size.x && y < size.y) {
		int posY = y - offsetY;
		int itemId = posY / rowHeight;
		if (itemId >= 0 and itemId < data.size()) {
			if (dataIds[itemId] != -1) {
				selectedItemId = itemId;
			} else {
				selectedItemId = -1;
			}
			Redraw();
			//useItem();
		}
	}
}

void CustomTable::MouseMove(const int x, const int y) {
	int oldValue = highlightItemId;
	int offsetY = headers.empty() ? 0 : rowHeight;
	if (x >= 0 && y >= offsetY && x < size.x && y < size.y) {
		int posY = y - offsetY;
		int itemId = posY / rowHeight;
		if (itemId >= 0 && itemId < data.size() && dataIds[itemId] != -1) {
			highlightItemId = itemId;
		} else {
			highlightItemId = -1;
		}
	} else {
		highlightItemId = -1;
	}
	if (oldValue != highlightItemId) {
		Redraw();
	}
}

void CustomTable::DoubleClick(const MouseButton button, const int x, const int y) {
	if (button == MOUSE_LEFT && doubleClickItemListener && selectedItemId >=0) {
		doubleClickItemListener(Event(EVENT_WIDGETACTION, Self(), dataIds[selectedItemId]));
	}
}

void CustomTable::selectFirst() {
	if (data.empty()) {
		return;
	}
	selectedItemId = 0;
	Redraw();
}

void CustomTable::deselect() {
	if (data.empty()) {
		return;
	}
	selectedItemId = -1;
	highlightItemId = -1;
	Redraw();
}

vector<WString> CustomTable::getSelectedItem() {
	if (selectedItemId == -1) {
		return vector<WString>();
	} else {
		return data[dataIds[selectedItemId]];
	}
}

void CustomTable::setDoubleClickListener(std::function<bool(Event)> const& listener) {
	doubleClickItemListener = listener;
}

void CustomTable::setFilter(std::function<bool(vector<WString>)> const& _filter) {
	filter = _filter;
}

void CustomTable::updateDataRow(int id, vector<WString> dataRow) {
	if (id < data.size()) {
		data[id] = dataRow;
		Redraw();
	}
}

vector<vector<WString>> CustomTable::getData() const {
	return data;
}

void CustomTable::selectDataRow(vector<WString> dataRow) {
	for (int i = 0; i < data.size(); i++) {
		if (data[i] == dataRow) {
			for (int j = 0; j < data.size(); j++) {
				if (dataIds[j] == i) {
					selectedItemId = j;
					return;
				}
			}
			return;
		}
	}
}
