#include "F3Screen.h"

void F3Screen::Render(MinecraftUIRenderContext* ctx) {
    LocalPlayer* player = ctx->mClient->getLocalPlayer();
    if (player == nullptr) return;

    std::string text = "Minecraft 1.20.51.1 (AmethystAPI)";

    const ActorRotationComponent* component = player->tryGetComponent<ActorRotationComponent>();
    Vec2 headRot = component->headRot;
    text += fmt::format("\n\n({} / {})", headRot.y, headRot.x);

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