#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>

#include "amethyst/Log.h"
#include "amethyst/HookManager.h"
#include "MinHook.h"

#include "minecraft/src/common/world/item/Item.h"
#include "minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.h"
#include "minecraft/src-client/common/client/gui/ScreenView.h"
#include "minecraft/src-client/common/client/renderer/TexturePtr.h"
#include "minecraft/src-deps/core/string/StringHash.h"

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

static ClientInstance* g_client = nullptr;

extern "C" __declspec(dllexport) void OnStartJoinGame(ClientInstance* ci) {
    g_client = ci;
}

extern "C" __declspec(dllexport) void OnRenderUI(ScreenView* screenView, MinecraftUIRenderContext* ctx) {
    if (g_client == nullptr) return;
    
    LocalPlayer* player = g_client->getLocalPlayer();
    if (player == nullptr) return;

    std::string text = "Minecraft 1.20.30.02 (AmethystAPI)";

    Vec3* pos = player->getPosition();
    Vec3* rot = player->getHeadLookVector(1.0f);
    text.append(fmt::format("\n\nXYZ: {:.3f} / {:.3f} / {:.3f}", pos->x, pos->y, pos->z));
    text.append(fmt::format("\nRot: {} {} {}", rot->x, rot->y, rot->z));

    // Render text on screen
    RectangleArea rect = { 0.0f, 0.0f, 0.0f, 0.0f };
    mce::Color white(1.0f, 1.0f, 1.0f, 1.0f);

    TextMeasureData textData;
    memset(&textData, 0, sizeof(TextMeasureData));
    textData.fontSize = 1.0f;

    CaretMeasureData caretData;
    memset(&caretData, 1, sizeof(CaretMeasureData));

    ctx->drawDebugText(&rect, &text, &white, 1.0f, ui::Left, &textData, &caretData);
}

extern "C" __declspec(dllexport) void Shutdown() {
    hookManager.Shutdown();
}