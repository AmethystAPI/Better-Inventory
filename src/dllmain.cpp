#include "dllmain.h"

ClientInstance* client;

ShulkerRenderer shulkerRenderer;
ItemStack shulkerInventory[SHULKER_CACHE_SIZE][27]; 

SafetyHookInline _Item_appendFormattedHoverText;
SafetyHookInline _Shulker_appendFormattedHoverText;
SafetyHookInline _HoverRenderer__renderHoverBox;

static void Item_appendFormattedHovertext(Item* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t a5) {
    _Item_appendFormattedHoverText.call<void, Item*, const ItemStackBase&, Level&, std::string&, uint8_t>(self, itemStack, level, text, a5);

    Item* item = itemStack.mItem;
    uint64_t max = item->getMaxDamage();

    if (max != 0) {
        uint64_t current = max - item->getDamageValue(itemStack.mUserData);
        text.append(fmt::format("\n{}7Durability: {} / {}{}r", "\xc2\xa7", current, max, "\xc2\xa7"));
    }
    
    std::string rawNameId = std::string(item->mRawNameId.c_str());

    if (rawNameId == "bee_nest" || rawNameId == "beehive") {
        AppendBeeNestInformation(text, itemStack);
    }

    if (rawNameId.find("shulker_box") != std::string::npos) {
        // Don't append the id for shulker boxes (makes it too long)
        text.append(fmt::format("\n{}8{}:{}{}r", "\xc2\xa7", item->mNamespace, rawNameId, "\xc2\xa7"));
        return;
    }

    text.append(fmt::format("\n{}8{}:{} ({}){}r", "\xc2\xa7", item->mNamespace, rawNameId, item->mId, "\xc2\xa7"));
}

static void AppendBeeNestInformation(std::string& text, const ItemStackBase& itemStack) {
    CompoundTag* userData = itemStack.mUserData;
    
    // There are no bees in the bee nest
    if (userData == nullptr || !userData->contains("Occupants")) {
        text.append(fmt::format("\n{}5Contains 0 bees", "\xc2\xa7"));
        return;
    };

    ListTag* occupants = userData->getList("Occupants");
    text.append(fmt::format("\n{}5Contains {:d} bee{}", "\xc2\xa7", occupants->size(), occupants->size() > 1 ? "s" : ""));
}

int index = 0;

static void Shulker_appendFormattedHovertext(ShulkerBoxBlockItem* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t someBool) {
    // Use the appendFormattedHovertext for regular items, we don't want the list of items
    Item_appendFormattedHovertext(self, itemStack, level, text, someBool);
    
    index++;
    if (index >= SHULKER_CACHE_SIZE) index = 0;

    // Hide a secret index in the shulker name
    // We do this because appendFormattedHovertext gets called for the neightboring items so if there is a shulker
    // to the right of this one then its preview will get overriden, so we keep track of multiple at once using a rolling identifier
    text += fmt::format("{}{:x}", "\xc2\xa7", index);
    int thisIndex = index;

    // Reset all the currrent item stacks
    for (auto& itemStack : shulkerInventory[index]) {
        itemStack = ItemStack();
    }

    if (!itemStack.mUserData) return;
    if (!itemStack.mUserData->contains("Items")) return;

    const ListTag* items = itemStack.mUserData->getList("Items");

    for (int i = 0; i < items->size(); i++) {
        const CompoundTag* itemCompound = items->getCompound(i);
        byte slot = itemCompound->getByte("Slot");
        shulkerInventory[thisIndex][slot]._loadItem(itemCompound);
    }
}

static void HoverRenderer__renderHoverBox(HoverRenderer* self, MinecraftUIRenderContext* ctx, IClientInstance* client, RectangleArea* aabb, float someFloat) {
    // This is really bad code, it is relying on the fact that I have also hooked appendFormattedHovertext for items to append the item identifier
    // I have no idea where the currently hovered item is stored in the game! I can't find any references to it, so it might be set in some weird place?

    if (self->mFilteredContent.find("shulker_box") != std::string::npos) {
        std::string cachedIndex = self->mFilteredContent.substr(self->mFilteredContent.size() - 7, 1);

        try {
            int index = std::stoi(cachedIndex, nullptr, 16);
            shulkerRenderer.Render(ctx, self, index);
        }
        catch (...) {
            Log::Warning("There was an issue reading the shulker box! No id found, instead got: {}", cachedIndex);
            return;
        }

        return;
    }

    _HoverRenderer__renderHoverBox.thiscall(self, ctx, client, aabb, someFloat);
}

ModFunction void Initialize(AmethystContext& ctx) {
    Amethyst::InitializeAmethystMod(ctx);

    auto& hooks = Amethyst::GetHookManager();

    hooks.RegisterFunction<&Item::appendFormattedHovertext>("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F1 4C 89 44 24 ? 4C 8B F2 48 8B D9");
    hooks.CreateHook<&Item::appendFormattedHovertext>(_Item_appendFormattedHoverText, &Item_appendFormattedHovertext);

    hooks.RegisterFunction<&ShulkerBoxBlockItem::appendFormattedHovertext>("40 53 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 4D 8B F9 48 8B DA");
    hooks.CreateHook<&ShulkerBoxBlockItem::appendFormattedHovertext>(_Shulker_appendFormattedHoverText, &Shulker_appendFormattedHovertext);

    hooks.RegisterFunction<&HoverRenderer::_renderHoverBox>("48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 4C 89 70 ? 55 48 8D 68 ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 49 8B D9");
    hooks.CreateHook<&HoverRenderer::_renderHoverBox>(_HoverRenderer__renderHoverBox, &HoverRenderer__renderHoverBox);
}