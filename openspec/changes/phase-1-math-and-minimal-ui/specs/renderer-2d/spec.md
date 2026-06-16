## ADDED Requirements

### Requirement: 2D drawing primitives
The renderer SHALL provide a renderer-agnostic 2D API to draw lines, circles, polygons
(filled and outlined), and arrows, batching geometry and flushing it within a
begin/end frame. Callers SHALL NOT need to issue OpenGL calls directly; all GL state
stays inside `render`/`platform`.

#### Scenario: Primitives render without GL errors
- **WHEN** a frame draws lines, a circle, a polygon, and an arrow
- **THEN** the geometry is presented and no OpenGL errors are reported for the frame

#### Scenario: Renderer API exposes no GL types
- **WHEN** a scene includes the renderer header
- **THEN** it can draw primitives without including GLFW/OpenGL headers

### Requirement: 2D camera with pan and zoom
The renderer SHALL provide a `Camera2D` orthographic view supporting pan and zoom, and
SHALL expose world↔screen coordinate conversion so scenes and UI can map between pixels
and world units.

#### Scenario: World/screen mapping round-trips
- **WHEN** a world point is converted to screen coordinates and back under a given camera
- **THEN** the original world point is recovered within tolerance

#### Scenario: Zoom scales the visible world extent
- **WHEN** the camera zoom changes
- **THEN** the world-space extent mapped to the viewport changes accordingly while the view stays centered on the camera target

### Requirement: Text rendering
The renderer SHALL render ASCII text at a screen position in a given color using an
embedded bitmap font (no external font dependency), and SHALL report text dimensions so
the UI can lay out and measure labels.

#### Scenario: Text draws at a screen position
- **WHEN** `text` is called with a string, screen position, and color
- **THEN** the string is rendered legibly at that position with no new runtime dependency

#### Scenario: Text can be measured for layout
- **WHEN** the UI queries the size of a string
- **THEN** the renderer returns its pixel width and height for the embedded font
