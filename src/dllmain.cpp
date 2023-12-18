#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>

#include "amethyst/Log.h"
#include "amethyst/HookManager.h"
#include "MinHook.h"

#include "minecraft/src/common/world/item/Item.h"
#include "minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.h"

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

static RectangleArea rect;
static std::string text = "String!";
static mce::Color col(0.0f, 0.0f, 0.0f, 1.0f);

MinecraftUIRenderContext::_MinecraftUIRenderContext _MinecraftUIRenderContext_ctor;

static void MinecraftUIRenderContext_ctor(MinecraftUIRenderContext* self, IClientInstance& ci, ScreenContext& screen, const UIScene& scene) {
    _MinecraftUIRenderContext_ctor(self, ci, screen, scene);
    rect._x0 = 0.0f;
    rect._x1 = 1000.0f;
    rect._y0 = 0.0f;
    rect._y1 = 1000.0f;

    static TextMeasureData textData;
    memset(&textData, 0, sizeof(TextMeasureData));
    textData.fontSize = 10.0f;

    static CaretMeasureData caretData;
    memset(&caretData, 1, sizeof(CaretMeasureData));

    self->setTextAlpha(1.0f);
    self->drawDebugText(&rect, &text, &col, 0, ui::Left, &textData, &caretData);
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

    hookManager.CreateHook(
        SigScan("48 89 5C 24 ? 48 89 74 24 ? 48 89 4C 24 ? 57 48 83 EC ? 49 8B F9 48 8B DA 48 8B F1 48 8D 05"),
        &MinecraftUIRenderContext_ctor, reinterpret_cast<void**>(&_MinecraftUIRenderContext_ctor)
    );
}

extern "C" __declspec(dllexport) void Shutdown() {
    hookManager.Shutdown();
}