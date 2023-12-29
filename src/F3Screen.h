#pragma once
#include <entt/entt.hpp>
#include <minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.h>
#include <minecraft/src/common/world/entity/EntityContext.h>
#include <minecraft/src/common/world/entity/components/ActorHeadRotationComponent.h>
#include <iostream>
#include <amethyst/Log.h>
#include <amethyst/Memory.h>
#include <minecraft/src-client/common/client/player/LocalPlayer.h>

class F3Screen {
public:
	void Render(MinecraftUIRenderContext* ctx);
};