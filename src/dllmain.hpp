#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <amethyst/Log.hpp>
#include <amethyst/Memory.hpp>
#include <mc/src/common/world/item/ItemStack.hpp>
#include <mc/src/common/world/item/ShulkerBoxBlockItem.hpp>
#include <mc/src/common/world/item/ItemStackBase.hpp>
#include <mc/src/common/nbt/CompoundTag.hpp>
#include <mc/src/common/nbt/ListTag.hpp>
#include <mc/src/common/nbt/CompoundTagVariant.hpp>
#include <mc/src/common/nbt/ByteTag.hpp>
#include <mc/src-client/common/client/renderer/actor/ItemRenderer.hpp>
#include <mc/src-client/common/client/renderer/BaseActorRenderContext.hpp>
#include <mc/src-client/common/client/gui/ScreenView.hpp>
#include <mc/src/common/world/item/registry/ItemRegistryManager.hpp>
#include <mc/src/common/world/item/registry/ItemRegistry.hpp>
#include <mc/src/common/world/actor/player/PlayerInventory.hpp>
#include <mc/src/common/world/Container.hpp>
#include <mc/src-deps/core/resource/ResourceHelper.hpp>
#include <mc/src-client/common/client/renderer/TexturePtr.hpp>
#include <mc/src-client/common/client/gui/controls/renderers/HoverRenderer.hpp>
#include <mc/src-client/common/client/player/LocalPlayer.hpp>
#include <chrono>
#include <amethyst-deps/safetyhook.hpp>
#include <amethyst/runtime/AmethystContext.hpp>
#include <typeindex>
#include <mc/src/common/world/level/Level.hpp>
#include <amethyst/runtime/ModContext.hpp>

#include "ShulkerRenderer.hpp"

#define ModFunction extern "C" __declspec(dllexport)

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	return TRUE;
}

static void AppendBeeNestInformation(std::string& text, const ItemStackBase& itemStack);