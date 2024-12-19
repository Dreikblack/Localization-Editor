#include "UltraEngine.h"
#include "ParamTable.h"
#include "../../Managers/SettingsManager.h"

ParamTable::ParamTable() {
}

shared_ptr<ParamTable> ParamTable::create(const int x, const int y, const int width, const int height, const int paramHeight, shared_ptr<Widget> parent) {
	struct Struct : public ParamTable {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, paramHeight, parent);
	return instance;
}

bool ParamTable::Initialize(const int x, const int y, const int width, const int height, const int paramHeight, shared_ptr<Widget> parent) {
	bool flag = Widget::Initialize("", x, y, width, height, parent, style);
	ParamTable::paramHeight = paramHeight;
	auto settingsManager = SettingsManager::getInstance();
	float fonstScale = (float)paramHeight / (float)settingsManager->fontHeight;
	rightIndex = 0;
	rightDelta = 0;
	SetFontScale(fonstScale);
	return flag;
}

void ParamTable::Draw(const int x, const int y, const int width, const int height) {
	blocks.clear();
	AddBlock(iVec2(0), size, color[WIDGETCOLOR_BORDER], true, (hasWidgetStyle(WIDGET_BORDER_ROUNDED)) ? cornerradius : 0);
}


void ParamTable::addIconParam(shared_ptr<IconParam> param, bool isRightSide) {
	iconParams.push_back(param);
	param->SetParent(As<ParamTable>());
	param->setHeight(paramHeight);
	if (isRightSide) {
		param->setPositionX(getWidth() / 2);
		rightIndex++;
	}
	param->setPositionY((iconParams.size() - (isRightSide ? rightDelta : rightIndex))  * paramHeight - paramHeight);
	setHeight((iconParams.size() - rightIndex) * paramHeight);
}

void ParamTable::addIconParams(vector<shared_ptr<IconParam>> params, bool isRightSide) {
	for (auto param : params) {
		addIconParam(param, isRightSide);
	}
}

int ParamTable::getParamsCount() const {
	return iconParams.size();
}
