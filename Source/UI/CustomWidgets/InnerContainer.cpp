#include "UltraEngine.h"
#include "InnerContainer.h"
#include "Container.h"

InnerContainer::InnerContainer()
{
	//classname = "InnerContainer";
}

bool InnerContainer::KeyDown(const KeyCode key)
{
	return GetParent()->As<CustomWidget>()->KeyDown(key);
}

void InnerContainer::KeyUp(const KeyCode key)
{
	GetParent()->As<CustomWidget>()->KeyUp(key);
}

std::shared_ptr<InnerContainer> InnerContainer::create(shared_ptr<CustomWidget> parent)
{
	struct Struct : public InnerContainer {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize("", 0, 0, parent->GetSize().width, parent->GetSize().height, parent, 0);
	return instance;
}
