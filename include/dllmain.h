#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <amethyst/Log.h>
#include <amethyst/Memory.h>
#include <minecraft/src/common/world/item/ItemStack.h>
#include <minecraft/src/common/world/item/ShulkerBoxBlockItem.h>
#include <minecraft/src/common/world/item/ItemStackBase.h>
#include <minecraft/src/common/nbt/CompoundTag.h>
#include <minecraft/src/common/nbt/ListTag.h>
#include <minecraft/src/common/nbt/CompoundTagVariant.h>
#include <minecraft/src/common/nbt/ByteTag.h>
#include <minecraft/src-client/common/client/renderer/actor/ItemRenderer.h>
#include <minecraft/src-client/common/client/renderer/BaseActorRenderContext.h>
#include <minecraft/src-client/common/client/gui/ScreenView.h>
#include <minecraft/src/common/world/item/registry/ItemRegistryManager.h>
#include <minecraft/src/common/world/item/registry/ItemRegistry.h>
#include <minecraft/src/common/world/actor/player/PlayerInventory.h>
#include <minecraft/src/common/world/Container.h>
#include <minecraft/src-deps/core/resource/ResourceHelper.h>
#include <minecraft/src-client/common/client/renderer/TexturePtr.h>
#include <minecraft/src-client/common/client/gui/controls/renderers/HoverRenderer.h>
#include <chrono>
#include <amethyst-deps/safetyhook.hpp>
#include <amethyst/runtime/AmethystContext.h>
#include <typeindex>
#include <minecraft/src/common/world/level/Level.h>

#include "ShulkerRenderer.h"

#define ModFunction extern "C" __declspec(dllexport)

class CollisionTest {
public:
    void Func1(const std::string&);
    void Func2(const std::string&);
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}