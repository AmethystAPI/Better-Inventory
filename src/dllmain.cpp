#include "dllmain.hpp"
#include "mc/src-client/common/client/renderer/TextureGroup.hpp"
#include <mc/src-client/common/client/renderer/TexturePtr.hpp>
#include <mc/src-deps/minecraftrenderer/renderer/MaterialPtr.hpp>
#include <mc/src-deps/minecraftrenderer/renderer/Mesh.hpp>
#include <mc/src-client/common/client/renderer/BaseActorRenderContext.hpp>
#include <mc/src-client/common/client/gui/gui/GuiData.hpp>

ClientInstance* client;

ShulkerRenderer shulkerRenderer;
ItemStack shulkerInventory[SHULKER_CACHE_SIZE][27];

Amethyst::InlineHook<void (Item::*)(const ItemStackBase&, Level&, std::string&, bool) const> _MapItem_appendFormattedHovertext;
Amethyst::InlineHook<decltype(&Item::appendFormattedHovertext)> _Item_appendFormattedHovertext;
Amethyst::InlineHook<decltype(&ShulkerBoxBlockItem::appendFormattedHovertext)> _ShulkerBoxBlockItem_appendFormattedHovertext;
Amethyst::InlineHook<decltype(&HoverRenderer::_renderHoverBox)> _HoverRenderer__renderHoverBox;

std::string getDimensionStringId(DimensionType type) {
	int id = type.runtimeID;

	switch (id) {
	case 0: return "minecraft:overworld";
	case 1: return "minecraft:nether";
	case 2: return "minecraft:the_end";
	case 3: return "undefined";
	}

	return "unknown";
}

static void MapItem_appendFormattedHovertext(Item* self, const ItemStackBase& itemStack, Level& level, std::string& text, bool showCategory) {
	CompoundTag* nbt = itemStack.mUserData;

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	Item* item = itemStack.mItem;
	std::string rawNameId = std::string(item->mRawNameId.c_str());
	text.append(std::format("\n{}8{}:{}{}r", "\xc2\xa7", item->mNamespace, rawNameId, "\xc2\xa7"));
	text.append(std::format("{}8{:16x}", "\xc2\xa7", (uint64_t)nbt->getInt64("map_uuid")));

	return;
}

static void Item_appendFormattedHovertext(Item* self, const ItemStackBase& itemStack, Level& level, std::string& text, bool showCategory) {
	Log::Info("{}", itemStack.mItem->getRawNameId().c_str());
	_Item_appendFormattedHovertext(self, itemStack, level, text, showCategory);

	Item* item = itemStack.mItem;
	uint64_t maxDamage = item->getMaxDamage();

	if (maxDamage != 0) {
		uint64_t current = maxDamage - item->getDamageValue(itemStack.mUserData);
		text.append(std::format("\n{}7Durability: {} / {}{}r", "\xc2\xa7", current, maxDamage, "\xc2\xa7"));
	}

	std::string rawNameId = std::string(item->mRawNameId.c_str());

	if (rawNameId == "bee_nest" || rawNameId == "beehive") {
		AppendBeeNestInformation(text, itemStack);
	}

	if (rawNameId.find("shulker_box") != std::string::npos) {
		// Don't append the id for shulker boxes (makes it too long)
		text.append(std::format("\n{}8{}:{}{}r", "\xc2\xa7", item->mNamespace, rawNameId, "\xc2\xa7"));
		return;
	}

	if (rawNameId == "clock") {
		LocalPlayer* localPlayer = Amethyst::GetContext().mClientCtx->mClientInstance->getLocalPlayer();
		int tickTime = localPlayer->mLevel->getTime();
		int tickTimeOfDay = tickTime % 24000; // Minecraft day is 24000 ticks
		int day = std::floor(tickTime / 24000);

		// Convert ticks to hours and minutes
		// Minecraft day starts at 6:00 AM (tick 0)
		// Each hour = 1000 ticks
		int totalMinutes = (tickTimeOfDay / 1000.0) * 60; // Total minutes since midnight (tick 18000)
		int minutesSinceSix = totalMinutes + (6 * 60); // Add 6 hours offset

		// Wrap around if past 24 hours
		int totalMinutesInDay = minutesSinceSix % 1440; // 1440 minutes in a day

		int hours = totalMinutesInDay / 60;
		int minutes = totalMinutesInDay % 60;

		// Convert to 12-hour format with AM/PM
		bool isPM = hours >= 12;
		int displayHours = hours % 12;
		if (displayHours == 0) displayHours = 12; // 0 hours should display as 12

		std::string ampm = isPM ? "PM" : "AM";

		text.append(std::format("\n{}5{}:{:02d} {} Day {}", "\xc2\xa7", displayHours, minutes, ampm, day));
		text.append(std::format("\n{}5Tick {}/24000", "\xc2\xa7", tickTimeOfDay));
	}

	if (rawNameId == "compass") {
		LocalPlayer* localPlayer = Amethyst::GetContext().mClientCtx->mClientInstance->getLocalPlayer();
		Player::PlayerSpawnPoint respawn = localPlayer->mPlayerRespawnPoint;
		BlockPos defaultSpawn = localPlayer->mLevel->getDefaultSpawn();
		text.append(std::format("\n{}5XZ {} {}", "\xc2\xa7", defaultSpawn.x, defaultSpawn.z));
	}

	if (rawNameId == "lodestone_compass") {
		uint32_t trackingHandle = itemStack.mUserData->getInt("trackingHandle");
		LocalPlayer* localPlayer = Amethyst::GetContext().mClientCtx->mClientInstance->getLocalPlayer();
		PositionTrackingDB::PositionTrackingDBClient* positionTrackingDBClient = localPlayer->mLevel->getPositionTrackerDBClient();

		struct TrackingRecord {
			std::byte unk[4];
			BlockPos mPosition;
			DimensionType mDimensionType;
		}*trackingRecord;

		using fnDecl = uint8_t(*)(void*, uint32_t const&, TrackingRecord**);
		const auto fn = std::bit_cast<fnDecl>(SigScan("40 55 56 57 48 83 EC 50 49 8B F0"));

		uint8_t result = fn(positionTrackingDBClient, trackingHandle, &trackingRecord);

		BlockPos pos = trackingRecord->mPosition;
		text.append(std::format("\n{}5XYZ {} {} {} in {}({})", "\xc2\xa7", pos.x, pos.y, pos.z, getDimensionStringId(trackingRecord->mDimensionType), trackingRecord->mDimensionType.runtimeID));
	}

	if (rawNameId == "recovery_compass") {
		LocalPlayer* localPlayer = Amethyst::GetContext().mClientCtx->mClientInstance->getLocalPlayer();
		std::optional<DimensionType> lastDeathDimension = localPlayer->getLastDeathDimension();
		std::optional<BlockPos> lastDeathPos = localPlayer->getLastDeathPos();
		if (lastDeathDimension.has_value() && lastDeathPos.has_value()) {
			BlockPos pos = lastDeathPos.value();
			DimensionType dimension = lastDeathDimension.value();
			text.append(std::format("\n{}5XYZ {} {} {} in {}({})", "\xc2\xa7", pos.x, pos.y, pos.z, getDimensionStringId(dimension), dimension.runtimeID));
		}
		else {
			text.append(std::format("\n{}5No last death", "\xc2\xa7"));
		}
	}

	text.append(std::format("\n{}8{}:{} ({}){}r", "\xc2\xa7", item->mNamespace, rawNameId, item->mId, "\xc2\xa7"));
}

static void AppendBeeNestInformation(std::string& text, const ItemStackBase& itemStack) {
	CompoundTag* userData = itemStack.mUserData;

	// There are no bees in the bee nest
	if (userData == nullptr || !userData->contains("Occupants")) {
		text.append(std::format("\n{}5Contains 0 bees", "\xc2\xa7"));
		return;
	};

	ListTag* occupants = userData->getList("Occupants");
	text.append(std::format("\n{}5Contains {:d} bee{}", "\xc2\xa7", occupants->size(), occupants->size() > 1 ? "s" : ""));
}

int index = 0;

static void ShulkerBoxBlockItem_appendFormattedHovertext(ShulkerBoxBlockItem* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t someBool) {
	// Use the appendFormattedHovertext for regular items, we don't want the list of items
	Item_appendFormattedHovertext(self, itemStack, level, text, someBool);

	index++;
	if (index >= SHULKER_CACHE_SIZE) index = 0;

	// Hide a secret index in the shulker name
	// We do this because appendFormattedHovertext gets called for the neightboring items so if there is a shulker
	// to the right of this one then its preview will get overriden, so we keep track of multiple at once using a rolling identifier
	text += std::format("{}{:x}", "\xc2\xa7", index);
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
	Log::Info("\n{}", text);
}

static void HoverRenderer__renderHoverBox(HoverRenderer* self, MinecraftUIRenderContext* ctx, IClientInstance* client, RectangleArea* aabb, float someFloat) {
	// This is really bad code, it is relying on the fact that I have also hooked appendFormattedHovertext for items to append the item identifier
	// I have no idea where the currently hovered item is stored in the game! I can't find any references to it, so it might be set in some weird place?

	if (self->mFilteredContent.find("shulker_box") != std::string::npos) {
		// Find the position right after "shulker_box§r"
		size_t shulkerPos = self->mFilteredContent.find("shulker_box");
		if (shulkerPos != std::string::npos) {
			// Look for the hex index after "shulker_box§r"
			// The pattern is: "shulker_box§r§X" where X is our hex digit
			size_t searchStart = shulkerPos + 11; // length of "shulker_box"

			// Skip the "§r" that follows
			if (searchStart + 2 < self->mFilteredContent.size() &&
				self->mFilteredContent[searchStart] == '\xc2' &&
				self->mFilteredContent[searchStart + 1] == '\xa7' &&
				self->mFilteredContent[searchStart + 2] == 'r') {
				searchStart += 3;
			}

			// Now look for our index marker "§X" where X is the hex digit
			if (searchStart + 2 < self->mFilteredContent.size() &&
				self->mFilteredContent[searchStart] == '\xc2' &&
				self->mFilteredContent[searchStart + 1] == '\xa7') {

				char hexChar = self->mFilteredContent[searchStart + 2];

				try {
					std::string hexStr(1, hexChar);
					int index = std::stoi(hexStr, nullptr, 16);

					if (index >= 0 && index < SHULKER_CACHE_SIZE) {
						shulkerRenderer.Render(ctx, self, index);
						return;
					}
				}
				catch (...) {
					// Fall through to default rendering
				}
			}
		}
	}

	if (self->mFilteredContent.find("filled_map") != std::string::npos) {
		size_t mapPos = self->mFilteredContent.find("filled_map");
		if (mapPos != std::string::npos) {
			size_t searchStart = mapPos + 10; // length of "filled_map"

			// Skip "§r"
			if (searchStart + 2 < self->mFilteredContent.size() &&
				self->mFilteredContent[searchStart] == '\xc2' &&
				self->mFilteredContent[searchStart + 1] == '\xa7' &&
				self->mFilteredContent[searchStart + 2] == 'r') {
				searchStart += 3;
			}

			// Skip "§8"
			if (searchStart + 2 < self->mFilteredContent.size() &&
				self->mFilteredContent[searchStart] == '\xc2' &&
				self->mFilteredContent[searchStart + 1] == '\xa7') {
				searchStart += 3;
			}

			// Extract exactly 16 hex characters
			std::string uuidStr = self->mFilteredContent.substr(searchStart, 16);

			try {
				uint64_t uuid = std::stoull(uuidStr, nullptr, 16);

				struct MapRenderer {
					std::byte unk1[0x198];
					SubClientId mClientId;
					mce::TexturePtr mDecorationTexture;
					mce::TexturePtr mMapBackgroundTexture;
					mce::TexturePtr mIconBackgroundTexture;
					std::byte unk2[1352];
					mce::Mesh mBackgroundMesh;
					mce::Mesh mForegroundMesh;
					mce::Mesh mDecorationMeshes[25];
				};

				auto mapRenderer = std::unique_ptr < MapRenderer, decltype([](MapRenderer* p) {
					std::free(p);
					}) > (static_cast<MapRenderer*>(std::malloc(sizeof(MapRenderer))));

				using mapRendererCtorDecl = MapRenderer * (MapRenderer::*)(SubClientId, std::shared_ptr<mce::TextureGroup>);
				const auto mapRendererCtor = std::bit_cast<mapRendererCtorDecl>(SigScan("48 89 5C 24 10 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 B0 FA"));

				(mapRenderer.get()->*mapRendererCtor)(client->asInstance().mClientSubId, std::shared_ptr<mce::TextureGroup>(*ctx->mTextures));

				BaseActorRenderContext renderCtxPtr = BaseActorRenderContext(*ctx->mScreenContext, *ctx->mClient, *ctx->mClient->mMinecraftGame);

				using mapRenderer_generateMeshesDecl = void (MapRenderer::*)(BaseActorRenderContext&);
				const auto mapRenderer_generateMeshes = std::bit_cast<mapRenderer_generateMeshesDecl>(SigScan("48 8B C4 48 89 58 18 48 89 70 20 55 57 41 56 48 8D A8 48 F9"));

				(mapRenderer.get()->*mapRenderer_generateMeshes)(renderCtxPtr);

				struct MapRenderer_MapInstance {
					std::byte unk1[0x24];
					MapRenderer& mMapRenderer;
					std::byte unk2[0x24];
					mce::TexturePtr mMapTexture;
					std::byte unk3[0x64];
				};
				struct MapItemSavedData {};

				auto mapInstance = std::unique_ptr < MapRenderer_MapInstance, decltype([](MapRenderer_MapInstance* p) {
					std::free(p);
					}) > (static_cast<MapRenderer_MapInstance*>(std::malloc(sizeof(MapRenderer_MapInstance))));
				auto savedData = std::bit_cast<MapItemSavedData*>(client->asInstance().getLocalPlayer()->getLevel()->getMapSavedData(uuid));

				using MapRenderer_MapInstanceCtorDecl = MapRenderer_MapInstance * (MapRenderer_MapInstance::*)(SubClientId, const MapItemSavedData*, MapRenderer*);
				const auto MapRenderer_MapInstanceCtor = std::bit_cast<MapRenderer_MapInstanceCtorDecl>(SigScan("48 89 5C 24 10 48 89 74 24 18 55 57 41 54 41 56 41 57 48 8D AC 24 50 FF FF FF 48 81 EC B0 01 00 00 48 8B F1 48 89 8D"));

				(mapInstance.get()->*MapRenderer_MapInstanceCtor)(client->asInstance().mClientSubId, savedData, mapRenderer.get());

				using MapRenderer_MapInstance_updateTextureDecl = void (MapRenderer_MapInstance::*)();
				const auto MapRenderer_MapInstance_updateTexture = std::bit_cast<MapRenderer_MapInstance_updateTextureDecl>(SigScan("48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D 6C 24 E0 48 81 EC 20 01 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 45 18 48 8B F1 48 89 4C"));

				(mapInstance.get()->*MapRenderer_MapInstance_updateTexture)();

				Matrix& matrix = ctx->mScreenContext->camera->worldMatrixStack.stack.top();
				Matrix originalMatrix = matrix;

				ClientInstance& client = *Amethyst::GetContext().mClientCtx->mClientInstance;
				Vec2 screenSize = client.mGuiData->clientUIScreenSize;

				mce::MaterialPtr* mapMaterial = reinterpret_cast<mce::MaterialPtr*>(SlideAddress(0x59BD7E0));

				matrix.translate(self->mCursorPosition.x + self->mOffset.x, self->mCursorPosition.y + self->mOffset.y, 0);
				mapRenderer->mBackgroundMesh.renderMesh(*ctx->mScreenContext, *mapMaterial, mapRenderer->mMapBackgroundTexture);
				mapRenderer->mBackgroundMesh.renderMesh(*ctx->mScreenContext, *mapMaterial, mapInstance->mMapTexture);

				Log::Info("{}", mapInstance->mMapTexture.mResourceLocation->mPath);
				Log::Info("{}", mapRenderer->mMapBackgroundTexture.mResourceLocation->mPath);

				matrix = originalMatrix;

				return;
			}
			catch (...) {
				// Fall through to default rendering
			}
		}
	}

	_HoverRenderer__renderHoverBox(self, ctx, client, aabb, someFloat);
}

ModFunction void Initialize(AmethystContext& ctx, const Amethyst::Mod& mod) {
	Amethyst::InitializeAmethystMod(ctx, mod);

	auto& hooks = Amethyst::GetHookManager();

	VHOOK(Item, appendFormattedHovertext, this);
	VHOOK(ShulkerBoxBlockItem, appendFormattedHovertext, this);
	HOOK(HoverRenderer, _renderHoverBox, this);

	hooks.CreateHookAbsolute(_MapItem_appendFormattedHovertext, SigScan("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F8 48 81 EC 08 01 00 00 4D 8B F9 4D"), &MapItem_appendFormattedHovertext);
}