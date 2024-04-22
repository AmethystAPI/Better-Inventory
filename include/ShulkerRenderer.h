#pragma once
#include <minecraft/src/common/world/item/ItemStack.hpp>
#include <minecraft/src-client/common/client/gui/ScreenView.hpp>
#include <minecraft/src-deps/core/string/StringHash.hpp>
#include <minecraft/src-deps/core/resource/ResourceHelper.hpp>
#include <minecraft/src-client/common/client/renderer/TexturePtr.hpp>
#include <minecraft/src-client/common/client/renderer/BaseActorRenderContext.hpp>
#include <minecraft/src-client/common/client/renderer/actor/ItemRenderer.hpp>
#include <minecraft/src-client/common/client/gui/controls/renderers/HoverRenderer.hpp>
#include <minecraft/src-client/common/client/renderer/NinesliceInfo.hpp>
#include <amethyst/ui/NinesliceHelper.hpp>

#define SHULKER_CACHE_SIZE 16

class ShulkerRenderer {
private:
	mce::TexturePtr mBackgroundTexture;
	mce::TexturePtr mItemSlotTexture;

public:
	void Render(UIRenderContext* ctx, HoverRenderer* hoverRenderer, int index);
};