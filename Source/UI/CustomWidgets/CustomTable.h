#pragma once
#include "UltraEngine.h"
#include "CustomButton.h"
#include "IconParam.h"

class CustomTable : public CustomWidget {
protected:
	//records with strings per column
	vector<vector<WString>> data;
	vector<WString> headers;
	vector<int> columnWidthes;
	int rowHeight;
	int selectedItemId = -1;
	int highlightItemId = -1;
	std::function<bool(Event)> doubleClickItemListener;
	std::function<bool(vector<WString>)> filter;
	//dataIds[visual current row]=real data order
	vector<int> dataIds;
	CustomTable();
	~CustomTable() override;
	void Draw(const int x, const int y, const int width, const int height) override;
	void MouseDown(const MouseButton button, const int x, const int y) override;
	void MouseMove(const int x, const int y) override;
	void DoubleClick(const MouseButton button, const int x, const int y) override;
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, vector<int> columnWidthes, int rowHeight, int style);
	void updateSize();
public:
	static shared_ptr<CustomTable> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, vector<int> columnWidthes, int rowHeight, int style = 0);
	void setHeaders(vector<WString> headersToSet);
	void setData(vector<vector<WString>> dataToSet);
	void setColumnWidthes(vector<int> columnWidthesToSet);
	void selectFirst();
	void deselect();
	vector<WString> getSelectedItem();
	void setDoubleClickListener(std::function<bool(Event)> const& listener);
	void setFilter(std::function<bool(vector<WString>)> const& _filter);
	void updateDataRow(int id, vector<WString> dataRow);
	vector<vector<WString>> getData() const;
};

