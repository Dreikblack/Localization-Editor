#pragma once

#include "CustomWidget.h"
#include "CustomSlider.h"
#include "../../Managers/ControlsManager.h"

using namespace UltraEngine;

enum ContainerStyle {
	CONTAINER_SCROLL = 16
};

class InnerContainer;

class Container : public CustomWidget {
protected:
	Container();
	~Container() override;
	shared_ptr<InnerContainer> innerContainer;
	shared_ptr<InnerContainer> viewContainer;
	shared_ptr<CustomSlider> verticalSlider;
	shared_ptr<CustomSlider> horizontalSlider;
	int scrollSize = 20;
	bool isShift = false;
public:
	static std::shared_ptr<Container> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style = 0);
	bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style) override;
	virtual bool KeyDown(const KeyCode key);
	virtual void KeyUp(const KeyCode key);
	void addWidget(shared_ptr<Widget> widget);
	void addWidgets(vector<shared_ptr<Widget>> widgets);
	void removeWidget(shared_ptr<Widget> widget);
	void clear();
	vector<shared_ptr<Widget>> getWidgets();
	void updateInnerContainerSize();
	void moveContent(int x, int y);
	void resetScrollPosition();
	int getElementCount();
	iVec2 getInnerContainerPosition();
	void setInnerContainerSize(iVec2 size);
	virtual void SetShape(const iVec2& position, const iVec2& size);
	virtual void SetShape(const int x, const int y, const int width, const int height);
};

