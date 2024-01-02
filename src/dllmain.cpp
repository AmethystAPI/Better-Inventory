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
#include <minecraft/src-client/common/client/input/ClientInputHandler.h>
#include <minecraft/src-client/common/client/input/VanillaClientInputMappingFactory.h>

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

bool shouldRenderF3 = false;
static F3Screen f3Screen;

extern "C" __declspec(dllexport) void OnRenderUI(ScreenView * screenView, MinecraftUIRenderContext * ctx) {
    if (shouldRenderF3) f3Screen.Render(ctx);
}

extern "C" __declspec(dllexport) void Shutdown() {
    hookManager.Shutdown();
}

VanillaClientInputMappingFactory::__createBaseNormalGamePlayKeyboardAndMouseMapping __createBaseNormalGamePlayKeyboardAndMouseMapping;

std::string testButtonId = "button.test";
int32_t keyNum = 0x72;
bool addedAlready = false;

static void* _createBaseNormalGamePlayKeyboardAndMouseMapping(VanillaClientInputMappingFactory* self, KeyboardInputMapping* keyboardMap, MouseInputMapping* mouseMap) {
    void* ret = __createBaseNormalGamePlayKeyboardAndMouseMapping(self, keyboardMap, mouseMap);
    self->_bindActionToKeyboardAndMouseInput(keyboardMap, mouseMap, &testButtonId, (Remapping::ActionEnum)100, FocusImpact::Neutral);

    Log::Info("\n_createBaseNormalGamePlayKeyboardAndMouseMapping");

    Keymapping newKeymapping = { "key.test", { keyNum }, true };

    if (!addedAlready) {
        self->mLayout->mDefaultMappings.push_back(newKeymapping); // key...
        self->mLayout->mKeymappings.push_back(newKeymapping);
        addedAlready = true;
    }

    /*for (auto it = self->mActiveInputMappings.begin(); it != self->mActiveInputMappings.end(); ++it) {
        const std::string mappingName = it->first;
        KeyboardKeyBinding key = { testButtonId, keyNum, FocusImpact::Neutral };

        InputMapping inputMapping = it->second;
        inputMapping.keyboardMapping.keyBindings.push_back(key);
    }*/

    return ret;
}

extern "C" __declspec(dllexport) void OnStartJoinGame(ClientInstance * clientInstance) {
    /*InputHandler* handler = clientInstance->inputHandler->mInputHandler;

    handler->registerButtonDownHandler(testButtonId, [](FocusImpact impact, IClientInstance& instance) {
        Log::Info("{} pressed!!", testButtonId);
        shouldRenderF3 = !shouldRenderF3;
    }, false);*/
}

extern "C" __declspec(dllexport) void Initialize(const char* gameVersion) {
    MH_Initialize();

    hookManager.CreateHook(
        SigScan("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F1 4C 89 44 24 ? 4C 8B F2 48 8B D9"),
        &Item_appendFormattedHovertext, reinterpret_cast<void**>(&_Item_appendFormattedHovertext)
    );

    hookManager.CreateHook(
        SigScan("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC ? 49 8B F0 48 8B DA 48 8B F9 E8"),
        &_createBaseNormalGamePlayKeyboardAndMouseMapping, reinterpret_cast<void**>(&__createBaseNormalGamePlayKeyboardAndMouseMapping)
    );
}