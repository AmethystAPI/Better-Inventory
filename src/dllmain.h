#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <amethyst/Log.hpp>
#include <amethyst/Memory.hpp>
#include <minecraft/src/common/world/item/ItemStack.hpp>
#include <minecraft/src/common/world/item/ShulkerBoxBlockItem.hpp>
#include <minecraft/src/common/world/item/ItemStackBase.hpp>
#include <minecraft/src/common/nbt/CompoundTag.hpp>
#include <minecraft/src/common/nbt/ListTag.hpp>
#include <minecraft/src/common/nbt/CompoundTagVariant.hpp>
#include <minecraft/src/common/nbt/ByteTag.hpp>
#include <minecraft/src-client/common/client/renderer/actor/ItemRenderer.hpp>
#include <minecraft/src-client/common/client/renderer/BaseActorRenderContext.hpp>
#include <minecraft/src-client/common/client/gui/ScreenView.hpp>
#include <minecraft/src/common/world/item/registry/ItemRegistryManager.hpp>
#include <minecraft/src/common/world/item/registry/ItemRegistry.hpp>
#include <minecraft/src/common/world/actor/player/PlayerInventory.hpp>
#include <minecraft/src/common/world/Container.hpp>
#include <minecraft/src-deps/core/resource/ResourceHelper.hpp>
#include <minecraft/src-client/common/client/renderer/TexturePtr.hpp>
#include <minecraft/src-client/common/client/gui/controls/renderers/HoverRenderer.hpp>
#include <chrono>
#include <amethyst-deps/safetyhook.hpp>
#include <amethyst/runtime/AmethystContext.hpp>
#include <typeindex>
#include <minecraft/src/common/world/level/Level.hpp>
#include <amethyst/runtime/ModContext.hpp>

#include "ShulkerRenderer.h"

#define ModFunction extern "C" __declspec(dllexport)

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

static void AppendBeeNestInformation(std::string& text, const ItemStackBase& itemStack);