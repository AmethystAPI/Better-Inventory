#pragma once
#include "entt/entt.hpp"
#include "minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.h"
#include "minecraft/src/common/world/entity/EntityContext.h"
#include "minecraft/src/common/world/entity/components/ActorHeadRotationComponent.h"
#include <iostream>

class F3Screen {
public:
	void Render(MinecraftUIRenderContext* ctx);
};

static_assert(sizeof(entt::basic_registry<EntityId>) == 0x120);