#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>

#include "amethyst/Log.h"
#include "amethyst/HookManager.h"
#include "MinHook.h"

#include "minecraft/src/common/world/item/Item.h"
#include "minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.h"
#include "minecraft/src-client/common/client/gui/ScreenView.h"

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

ScreenView::_setupAndRender _ScreenView_setupAndRender;

static void* ScreenView_setupAndRender(ScreenView* self, UIRenderContext* ctx) {
    RectangleArea rect = { 0.0f, 200.0f, 0.0f, 200.0f };
    std::string text = "Hello, World!";
    mce::Color col(1.0f, 1.0f, 1.0f, 1.0f);

    TextMeasureData textData;
    memset(&textData, 0, sizeof(TextMeasureData));
    textData.fontSize = 1.0f;

    CaretMeasureData caretData;
    memset(&caretData, 1, sizeof(CaretMeasureData));

    ctx->drawDebugText(&rect, &text, &col, 1.0f, ui::TextAlignment::Left, &textData, &caretData);
    return _ScreenView_setupAndRender(self, ctx);
}

extern "C" __declspec(dllexport) void Initialize() {
    MH_Initialize();

    hookManager.CreateHook(
        SigScan("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F1 4C 89 44 24 ? 4C 8B F2 48 8B D9"),
        &Item_appendFormattedHovertext, reinterpret_cast<void**>(&_Item_appendFormattedHovertext)
    );

    hookManager.CreateHook(
        SigScan("48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B FA 48 89 54 24 ? 4C 8B E9 48 89 4C 24"),
        &ScreenView_setupAndRender, reinterpret_cast<void**>(&_ScreenView_setupAndRender)
    );
}

extern "C" __declspec(dllexport) void Shutdown() {
    hookManager.Shutdown();
}