#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>

#include <amethyst/Log.h>
#include <amethyst/HookManager.h>

#include <minecraft/src/common/world/item/Item.h>
#include <minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.h>
#include <minecraft/src-client/common/client/gui/ScreenView.h>
#include <minecraft/src-client/common/client/renderer/TexturePtr.h>
#include <minecraft/src-deps/core/string/StringHash.h>
#include <minecraft/src/common/world/item/ItemStackBase.h>

#include "F3Screen.h"

HookManager hookManager;

Item::_appendFormattedHovertext _Item_appendFormattedHovertext;

static void Item_appendFormattedHovertext(Item* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t a5) {
    _Item_appendFormattedHovertext(self, itemStack, level, text, a5);
    Item* item = itemStack.mItem;

    uint64_t max = item->getMaxDamage();

    if (max != 0) {
        uint64_t current = max - item->getDamageValue(itemStack.mUserData);
        text.append(fmt::format("\n{}7Durability: {} / {}{}r", "\xc2\xa7", current, max, "\xc2\xa7"));
    }

    std::string rawNameId;
    itemStack.getRawNameId(rawNameId);
    text.append(fmt::format("\n{}8{}:{} ({}){}r", "\xc2\xa7", item->mNamespace, rawNameId, item->mId, "\xc2\xa7"));
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

extern "C" __declspec(dllexport) void Initialize() {
    MH_Initialize();

    hookManager.CreateHook(
        SigScan("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F1 4C 89 44 24 ? 4C 8B F2 48 8B D9"),
        &Item_appendFormattedHovertext, reinterpret_cast<void**>(&_Item_appendFormattedHovertext)
    );
}

static F3Screen f3Screen;

extern "C" __declspec(dllexport) void OnRenderUI(ScreenView * screenView, MinecraftUIRenderContext * ctx) {
    f3Screen.Render(ctx);
}

extern "C" __declspec(dllexport) void Shutdown() {
    hookManager.Shutdown();
}