#pragma once

#include "UltraEngine.h"
#include "CustomWidget.h"

class Container;

class InnerContainer : public CustomWidget
{
protected:
	InnerContainer();
public:
	static std::shared_ptr<InnerContainer> create(shared_ptr<CustomWidget> parent);
	virtual bool KeyDown(const KeyCode key);
	virtual void KeyUp(const KeyCode key);
};