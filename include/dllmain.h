#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <amethyst/Log.h>
#include <amethyst/HookManager.h>
#include <amethyst/InputManager.h>
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

#include "ShulkerRenderer.h"

#define ModFunction extern "C" __declspec(dllexport)

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}