# ui-toolkit

## Purpose

Provide an immediate-mode UI widget set (slider, button, checkbox, label, equation label,
line plot) that binds directly to caller-owned state and draws through the renderer. Input
arrives as an injected `InputState` snapshot so the UI depends downward only on the
renderer, never on GLFW or platform code.

## Requirements

### Requirement: Immediate-mode widgets
The UI SHALL provide an immediate-mode widget set re-declared each frame: a float slider,
a button, a checkbox, a text label, an equation label, and a line plot. Widgets SHALL
read and write caller-owned state directly (e.g., a slider bound to a `double`) and SHALL
draw through the renderer.

#### Scenario: Slider edits bound state
- **WHEN** the user drags a `SliderFloat` bound to a value within its range
- **THEN** the bound value updates to the dragged position, clamped to the range

#### Scenario: Button reports activation for one frame
- **WHEN** the user clicks a `Button`
- **THEN** the button call returns true on that activation and false otherwise

#### Scenario: Checkbox toggles bound state
- **WHEN** the user clicks a `Checkbox` bound to a boolean
- **THEN** the bound boolean flips

#### Scenario: Plot displays a data series
- **WHEN** `PlotLine` is given a numeric series (e.g., energy history)
- **THEN** the series is drawn as a line plot within the widget bounds

### Requirement: Input via injected snapshot (layering preserved)
The UI SHALL consume an `InputState` snapshot (mouse position, button states, pressed
keys, scroll) supplied by the application each frame, and SHALL NOT depend on GLFW or
platform code directly. The UI SHALL depend downward only on the renderer.

#### Scenario: UI reacts to injected input
- **WHEN** the application passes an `InputState` with the mouse over an active widget and the button pressed
- **THEN** the widget responds as interacted

#### Scenario: UI does not depend on the windowing system
- **WHEN** the `ui` target is built
- **THEN** it links/includes no GLFW/OpenGL/platform code (only the renderer and math)
