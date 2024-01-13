#include "dllmain.h"

HookManager hookManager;
ClientInstance* client;

ShulkerBoxBlockItem::_appendFormattedHovertext _Shulker_appendFormattedHoverText;
Item::_appendFormattedHovertext _Item_appendFormattedHovertext;
HoverRenderer::__renderHoverBox __renderHoverBox;

ShulkerRenderer shulkerRenderer;
ItemStack shulkerInventory[27];

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

static void Shulker_appendFormattedHovertext(ShulkerBoxBlockItem* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t someBool) {
    // Use the appendFormattedHovertext for regular items, we don't want the list of items
    Item_appendFormattedHovertext(self, itemStack, level, text, someBool);

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

static void _renderHoverBox(HoverRenderer* self, MinecraftUIRenderContext* ctx, IClientInstance* client, RectangleArea* aabb, float someFloat) {
    // This is really bad code, it is relying on the fact that I have also hooked appendFormattedHovertext for items to append the item identifier
    // I have no idea where the currently hovered item is stored in the game! I can't find any references to it, so it might be set in some weird place?
    if (self->mFilteredContent.find("shulker_box") != std::string::npos) {
        shulkerRenderer.Render(ctx, self);
        return;
    }

    __renderHoverBox(self, ctx, client, aabb, someFloat);
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

    hookManager.CreateHook(
        SigScan("48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 4C 89 70 ? 55 48 8D 68 ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 49 8B D9"),
        &_renderHoverBox, reinterpret_cast<void**>(&__renderHoverBox)
    );
}

ModFunction void Shutdown() {
    hookManager.Shutdown();
}