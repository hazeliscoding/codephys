#pragma once

// Phase 0 stub for the renderer module (DESIGN §7.2). Wired but minimal: the real 2D/3D
// renderer lands in Phase 1. render depends downward only on physics::math.

namespace render {

// Placeholder public symbol; replaced by Renderer2D/Renderer3D in Phase 1.
const char* module_name();

}  // namespace render
