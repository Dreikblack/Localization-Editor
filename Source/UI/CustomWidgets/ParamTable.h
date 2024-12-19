#pragma once
#include "UltraEngine.h"
#include "CustomButton.h"
#include "IconParam.h"

class ParamTable : public CustomWidget {
protected:
	int paramHeight = GUI_SCALE / 2;
	vector<shared_ptr<IconParam>> iconParams;
	int rightIndex;
	ParamTable();
	void Draw(const int x, const int y, const int width, const int height) override;
	virtual bool Initialize(const int x, const int y, const int width, const int height, const int paramHeight, shared_ptr<Widget> parent);
public:
	int rightDelta;
	static shared_ptr<ParamTable> create(const int x, const int y, const int width, const int height, const int paramHeight, shared_ptr<Widget> parent);
	void addIconParam(shared_ptr<IconParam> param, bool isRightSide = false);
	void addIconParams(vector<shared_ptr<IconParam>> params, bool isRightSide = false);
	int getParamsCount() const;
};

