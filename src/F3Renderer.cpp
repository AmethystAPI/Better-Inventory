//#include "F3Screen.h"
//
//extern int offset;
//
//void F3Screen::Render(MinecraftUIRenderContext* ctx) {
//    LocalPlayer* player = ctx->mClient->getLocalPlayer();
//    if (player == nullptr) return;
//
//    std::string text = "Minecraft 1.20.51.1 (AmethystAPI)\n";
//
//    const StateVectorComponent* stateComponent = player->tryGetComponent<StateVectorComponent>();
//    const Vec3* pos = &stateComponent->mPos;
//    text += fmt::format("\nXYZ: {:.3f}, {:.3f}, {:.3f}", pos->x, pos->y, pos->z);
//
//    const ActorRotationComponent* component = player->tryGetComponent<ActorRotationComponent>();
//    Vec2 headRot = component->mHeadRot;
//    std::string direction;
//
//    float cardinalRot = headRot.y + 180.0f;
//    if (cardinalRot >= 315.0f || cardinalRot < 45.0f) direction = "north (Towards negative Z)";
//    else if (cardinalRot >= 45.0f && cardinalRot < 135.0f) direction = "east (Towards positive X)";
//    else if (cardinalRot >= 135.0f && cardinalRot < 225.0f) direction = "south (Towards positive Z)";
//    else direction = "west (Towards negative X)";
//
//    text += fmt::format("\nFacing: {} ({:.1f} / {:.1f})", direction, headRot.y, headRot.x);
//
//    // Render text on screen
//    RectangleArea rect = { 0.0f, 0.0f, 0.0f, 0.0f };
//    mce::Color white(1.0f, 1.0f, 1.0f, 1.0f);
//
//    TextMeasureData textData;
//    memset(&textData, 0, sizeof(TextMeasureData));
//    textData.fontSize = 1.0f;
//
//    CaretMeasureData caretData;
//    memset(&caretData, 1, sizeof(CaretMeasureData));
//
//    ctx->drawDebugText(&rect, &text, &white, 1.0f, ui::Left, &textData, &caretData);
//}