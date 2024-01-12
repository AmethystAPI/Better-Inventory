#include "dllmain.h"

HookManager hookManager;
ClientInstance* client;

ShulkerBoxBlockItem::_appendFormattedHovertext _Shulker_appendFormattedHoverText;
Item::_appendFormattedHovertext _Item_appendFormattedHovertext;

ItemStack shulkerInventory[27];
bool isHoveringShulkerbox = false;

static void Shulker_appendFormattedHovertext(ShulkerBoxBlockItem* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t someBool) {
    _Shulker_appendFormattedHoverText(self, itemStack, level, text, someBool);
    isHoveringShulkerbox = true;

    // Reset all the currrent item stacks
    for (auto& itemStack : shulkerInventory) {
        itemStack = ItemStack();
    }

    if (!itemStack.mUserData) return;
    if (!itemStack.mUserData->contains("Items")) return;

    const ListTag* items = itemStack.mUserData->getList("Items");
    ItemRegistryRef itemRegistryRef = ItemRegistryManager::getItemRegistry();
    auto registry = itemRegistryRef.mItemRegistry.lock();

    for (int i = 0; i < items->size(); i++) {
        const CompoundTag* itemCompound = items->getCompound(i);
        byte slot = itemCompound->getByte("Slot");
        shulkerInventory[slot]._loadItem(itemCompound);
    }
}

static void Item_appendFormattedHovertext(Item* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t a5) {
    _Item_appendFormattedHovertext(self, itemStack, level, text, a5);
    isHoveringShulkerbox = false;
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

ModFunction void Initialize(const char* gameVersion, InputManager * inputManager) {
    MH_Initialize();

    hookManager.CreateHook(
        SigScan("40 53 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 4D 8B F9 48 8B DA"),
        &Shulker_appendFormattedHovertext, reinterpret_cast<void**>(&_Shulker_appendFormattedHoverText)
    );

    hookManager.CreateHook(
        SigScan("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F1 4C 89 44 24 ? 4C 8B F2 48 8B D9"),
        &Item_appendFormattedHovertext, reinterpret_cast<void**>(&_Item_appendFormattedHovertext)
    );
}

ShulkerRenderer shulkerRenderer;

ModFunction void OnRenderUI(ScreenView* screenView, UIRenderContext* ctx) {
    shulkerRenderer.Render(ctx);
}

ModFunction void Shutdown() {
    hookManager.Shutdown();
}