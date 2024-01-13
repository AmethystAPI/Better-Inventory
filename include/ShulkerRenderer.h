#pragma once
#include <minecraft/src/common/world/item/ItemStack.h>
#include <minecraft/src-client/common/client/gui/ScreenView.h>
#include <minecraft/src-deps/core/string/StringHash.h>
#include <minecraft/src-deps/core/resource/ResourceHelper.h>
#include <minecraft/src-client/common/client/renderer/TexturePtr.h>
#include <minecraft/src-client/common/client/renderer/BaseActorRenderContext.h>
#include <minecraft/src-client/common/client/renderer/actor/ItemRenderer.h>
#include <minecraft/src-client/common/client/gui/controls/renderers/HoverRenderer.h>
#include <minecraft/src-client/common/client/renderer/NinesliceInfo.h>
#include <amethyst/ui/NinesliceHelper.h>

class ShulkerRenderer {
public:
	void Render(UIRenderContext* ctx, HoverRenderer* hoverRenderer);
};