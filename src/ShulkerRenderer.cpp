#include "ShulkerRenderer.h"

static HashedString flushString(0xA99285D21E94FC80, "ui_flush");

// Texture loading
static auto itemSlotTexture = std::make_unique<mce::TexturePtr>();
static ResourceLocation itemSlotLocation("textures/gui/gui");
bool hasLoadedTexture = false;

// Slot sizing
float slotSize = 20.f;
float borderSize = (slotSize - 16.f) / 2;

// Uv positions
glm::tvec2<float> itemSlotUvPos(188.0f / 256.0f, 184.0f / 256.0f);
glm::tvec2<float> itemSlotUvSize(22.0f / 256.0f, 22.0f / 256.0f);

extern ItemStack shulkerInventory[27];

mce::Color panelBackground(27.0f / 255.0f, 12.0f / 255.0f, 27.0f / 255.0f, 1.0f);

void ShulkerRenderer::Render(UIRenderContext* ctx, HoverRenderer* hoverRenderer) {
	if (ctx == nullptr || ctx->mClient->getLocalPlayer() == nullptr) return;

	// Only load inventory resources once
	if (!hasLoadedTexture) {
		ctx->getTexture(itemSlotTexture.get(), &itemSlotLocation, true);
		hasLoadedTexture = true;
	}

    float textHeight = 20.0f;
    float panelWidth = slotSize * 9;
    float panelHeight = slotSize * 3 + textHeight;

    float panelX = hoverRenderer->mCursorPosition.x + hoverRenderer->mOffset.x;
    float panelY = hoverRenderer->mCursorPosition.y + hoverRenderer->mOffset.y;

    RectangleArea background = {panelX, panelX + panelWidth, panelY, panelY + panelHeight};
    ctx->drawRectangle(&background, &panelBackground, 1.0f, 1);

    // Draw the item slots
	for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 3; y++) {
            glm::tvec2<float> size(slotSize, slotSize);
            glm::tvec2<float> position(panelX + slotSize * x, panelY + textHeight + slotSize * y);

            ctx->drawImage(*itemSlotTexture, &position, &size, &itemSlotUvPos, &itemSlotUvSize, 0);
        }
    }

    // It's possible to tint the background here
    ctx->flushImages(mce::Color::WHITE, 1.0f, flushString);

    // Draw the item icons
    BaseActorRenderContext renderCtxPtr = BaseActorRenderContext(ctx->mScreenContext, ctx->mClient, ctx->mClient->minecraftGame);

    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 3; y++) {
            const ItemStack* itemStack = &shulkerInventory[y * 9 + x];
            if (itemStack->mItem == nullptr) continue;

            float xPos = (x * slotSize) + borderSize + panelX;
            float yPos = (y * slotSize) + borderSize + textHeight + panelY;

            renderCtxPtr.itemRenderer->renderGuiItemNew(&renderCtxPtr, itemStack, 0, xPos, yPos, false, 1.f, 1.f, 1.f);
        }
    }

    ctx->flushImages(mce::Color::WHITE, 1.0f, flushString);

    // Draw Item Counts
    TextMeasureData textData;
    memset(&textData, 0, sizeof(TextMeasureData));
    textData.fontSize = 1.0f;

    CaretMeasureData caretData;
    memset(&caretData, 1, sizeof(CaretMeasureData));

    // Draw the item counts
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 3; y++) {
            ItemStack* itemStack = &shulkerInventory[y * 9 + x];
            if (itemStack->mItem == nullptr) continue;
            if (itemStack->count == 1) continue;

            float top = (y * slotSize) + borderSize + textHeight + panelY;
            float bottom = top + 16.f;

            float left = (x * slotSize) + borderSize + panelX;
            float right = left + 16.f;

            std::string text = fmt::format("{}", itemStack->count);
            RectangleArea rect = { left, right, top + 7.f, bottom};

            ctx->drawDebugText(&rect, &text, &mce::Color::WHITE, 1.0f, ui::Left, &textData, &caretData);
        }
    }

    ctx->flushText(0.0f);

    RectangleArea rect = { panelX, panelX + panelWidth, panelY, panelY + panelHeight };
    ctx->drawDebugText(&background, &hoverRenderer->mFilteredContent, &mce::Color::WHITE, 1.0f, ui::Left, &textData, &caretData);
    ctx->flushText(0.0f);
}