# codephys_warnings — strict warning set for FIRST-PARTY code only.
#
# Linked PRIVATE into our own targets (physics/render/ui/platform/app/tests). Third-party
# and fetched code (GLFW, glad, doctest) never links this target, so their warnings can
# never fail our build (DESIGN §D5).
add_library(codephys_warnings INTERFACE)
add_library(codephys::warnings ALIAS codephys_warnings)

if(MSVC)
    # /external:W0 compiles headers marked external (SYSTEM includes -> /external:I, e.g.
    # vendored glad/doctest) at warning level 0, so their warnings never trip /WX on our
    # first-party TUs that include them. GCC/Clang get this for free via -isystem.
    target_compile_options(codephys_warnings INTERFACE /W4 /WX /permissive- /external:W0)
else()
    target_compile_options(codephys_warnings INTERFACE
        -Wall -Wextra -Wpedantic -Werror)
endif()
