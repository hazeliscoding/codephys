#pragma once

// RGBA color, components in [0, 1]. Plain value type used across the renderer and UI.

namespace render {

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

// A few convenient named colors.
namespace colors {
inline constexpr Color white{1.0f, 1.0f, 1.0f, 1.0f};
inline constexpr Color black{0.0f, 0.0f, 0.0f, 1.0f};
inline constexpr Color red{0.90f, 0.25f, 0.25f, 1.0f};
inline constexpr Color green{0.30f, 0.80f, 0.35f, 1.0f};
inline constexpr Color blue{0.30f, 0.55f, 0.95f, 1.0f};
inline constexpr Color yellow{0.95f, 0.85f, 0.30f, 1.0f};
inline constexpr Color gray{0.55f, 0.58f, 0.62f, 1.0f};
}  // namespace colors

}  // namespace render
