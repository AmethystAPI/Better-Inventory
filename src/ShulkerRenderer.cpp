#include "ShulkerRenderer.h"
#include <minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.hpp>
#include <minecraft/src-deps/core/math/Color.hpp>
#include <minecraft/src-client/common/client/game/ClientInstance.hpp>

extern ItemStack shulkerInventory[SHULKER_CACHE_SIZE][27];

// Texture loading
static HashedString flushString(0xA99285D21E94FC80, "ui_flush");

bool hasLoadedTexture = false;

// Slot sizing
float slotSize = 20.f;
float borderSize = (slotSize - 16.f) / 2;

// Uv positions
glm::tvec2<float> itemSlotUvPos(188.0f / 256.0f, 184.0f / 256.0f);
glm::tvec2<float> itemSlotUvSize(22.0f / 256.0f, 22.0f / 256.0f);
Amethyst::NinesliceHelper backgroundNineslice(16, 16, 4, 4);

int countNewlines(const std::string& str) {
    int newlineCount = 0;

    for (char c : str) {
        if (c == '\n') {
            newlineCount++;
        }
    }

    return newlineCount;
}

void ShulkerRenderer::Render(UIRenderContext* ctx, HoverRenderer* hoverRenderer, int index) {
	// Only load inventory resources once
	if (!hasLoadedTexture) {
        mBackgroundTexture = ctx->getTexture("textures/ui/purpleBorder", true);
        mItemSlotTexture = ctx->getTexture("textures/gui/gui", true);

        hasLoadedTexture = true;
	}

    float textHeight = (countNewlines(hoverRenderer->mFilteredContent) + 1) * 10.0f;
    float panelWidth = slotSize * 9;
    float panelHeight = slotSize * 3 + textHeight;

    float panelX = hoverRenderer->mCursorPosition.x + hoverRenderer->mOffset.x;
    float panelY = hoverRenderer->mCursorPosition.y + hoverRenderer->mOffset.y;

    // Draw the background panel
    RectangleArea background = {panelX - 4, panelX + panelWidth + 4, panelY - 4, panelY + panelHeight + 4};
    backgroundNineslice.Draw(background, &mBackgroundTexture, ctx);
    ctx->flushImages(mce::Color::WHITE, 1.0f, flushString);

    // Draw the item slots
	for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 3; y++) {
            glm::tvec2<float> size(slotSize, slotSize);
            glm::tvec2<float> position(panelX + slotSize * x, panelY + textHeight + slotSize * y);

            ctx->drawImage(mItemSlotTexture, &position, &size, &itemSlotUvPos, &itemSlotUvSize, 0);
        }
    }

    // It's possible to tint the background here
    ctx->flushImages(mce::Color::WHITE, 1.0f, flushString);

    // Draw the item icons
    BaseActorRenderContext renderCtxPtr = BaseActorRenderContext(ctx->mScreenContext, ctx->mClient, ctx->mClient->minecraftGame);

    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 3; y++) {
            const ItemStack* itemStack = &shulkerInventory[index][y * 9 + x];
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
            ItemStack* itemStack = &shulkerInventory[index][y * 9 + x];
            if (itemStack->mItem == nullptr) continue;
            if (itemStack->mCount == 1) continue;

            float top = (y * slotSize) + borderSize + textHeight + panelY;
            float bottom = top + 16.f;

            float left = (x * slotSize) + borderSize + panelX;
            float right = left + 16.f;

            std::string text = fmt::format("{}", itemStack->mCount);
            RectangleArea rect = { left, right, top + 7.f, bottom};

            ctx->drawDebugText(&rect, &text, &mce::Color::WHITE, 1.0f, ui::Left, &textData, &caretData);
        }
    }

    ctx->flushText(0.0f);

    RectangleArea textArea = { panelX, panelX + panelWidth, panelY, panelY + textHeight };
    RectangleArea rect = { panelX, panelX + panelWidth, panelY, panelY + panelHeight };
    ctx->drawDebugText(&textArea, &hoverRenderer->mFilteredContent, &mce::Color::WHITE, 1.0f, ui::Left, &textData, &caretData);
    ctx->flushText(0.0f);
}