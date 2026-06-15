#include <render/render.hpp>

#include <physics/math/vec2.hpp>  // render may use physics::math types (DESIGN §5)

namespace render {

const char* module_name() {
    return "render";
}

}  // namespace render
