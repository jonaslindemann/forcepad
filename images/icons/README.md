# ForcePAD Icon Set

Complete SVG icon set for the ForcePAD application. All icons are designed in a minimalist, engineering-focused style with clean lines and clear symbolism.

## Icon Categories

### Edit Modes (`/edit-modes/`)
- **brush.svg** - EM_BRUSH: Brush/drawing tool
- **erase.svg** - EM_ERASE: Eraser tool
- **force.svg** - EM_FORCE: Single point load with force vector (circle + downward arrow)
- **constraint.svg** - EM_CONSTRAINT: Fixed support constraint (hinge + ground support)
- **constraint-vector.svg** - EM_CONSTRAINT_VECTOR: Directional constraint with vector (hinge + arrow)
- **constraint-hinge.svg** - EM_CONSTRAINT_HINGE: Hinge/pin support (circle with diagonal line)
- **dynamic-force-update.svg** - EM_DYNAMIC_FORCE_UPDATE: Add force (force + plus sign)
- **rotate-constraint-vector.svg** - EM_ROTATE_CONSTRAINT_VECTOR: Rotatable hinge constraint
- **line.svg** - EM_LINE: Line drawing tool
- **rectangle.svg** - EM_RECTANGLE: Rectangle drawing tool
- **ellipse.svg** - EM_ELLIPSE: Ellipse drawing tool
- **floodfill.svg** - EM_FLOODFILL: Fill tool
- **erase-constraints-forces.svg** - EM_ERASE_CONSTRAINTS_FORCES: Remove constraints and forces (strike-through)
- **select-box.svg** - EM_SELECT_BOX: Selection tool
- **paste.svg** - EM_PASTE: Paste content
- **result.svg** - EM_RESULT: View results/analysis
- **ruler.svg** - EM_RULER: Measurement tool with tick marks
- **arch.svg** - EM_ARCH: Arch structure tool
- **distributed-load.svg** - EM_DISTRIBUTED_LOAD: Multiple downward arrows (distributed load)
- **thickness.svg** - EM_THICKNESS: Bidirectional measurement arrow

### Import Modes (`/import-modes/`)
- **new-model.svg** - IM_NEW_MODEL: Create new model
- **paste.svg** - IM_PASTE: Import/paste model

### View Modes (`/view-modes/`)
- **sketch.svg** - VM_SKETCH: Sketch/design view
- **physics.svg** - VM_PHYSICS: Physics simulation view
- **action.svg** - VM_ACTION: Action/interaction view

### Visualization Modes (`/visualization-modes/`)
- **principal-stress.svg** - VM_PRINCIPAL_STRESS: Principal stress visualization
- **mises-stress.svg** - VM_MISES_STRESS: Von Mises stress visualization
- **displacements.svg** - VM_DISPLACEMENTS: Displacement visualization
- **structure.svg** - VM_STRUCTURE: Structural view

## Usage

All icons are 24x24px SVG format with:
- Stroke width: 1.5px
- Color: `currentColor` (inherits from parent)
- Clean, minimal design
- Optimized for both light and dark backgrounds

### Qt/C++ Integration

```cpp
// Load SVG icon
QIcon icon(":/icons/edit-modes/brush.svg");
button->setIcon(icon);

// Or use QSvgWidget for direct rendering
QSvgWidget *svgWidget = new QSvgWidget(":/icons/edit-modes/force.svg");
```

### Styling

Icons use `currentColor` so they automatically adapt to your theme:

```css
/* Dark theme */
.toolbar-icon {
    color: #ffffff;
}

/* Light theme */
.toolbar-icon {
    color: #000000;
}
```

## Design Principles

1. **Clarity**: Each icon clearly represents its function
2. **Consistency**: Uniform stroke width and style across all icons
3. **Engineering Focus**: Symbols are technically accurate for structural analysis
4. **Scalability**: Vector format ensures crisp rendering at any size
5. **Accessibility**: High contrast and clear shapes for visibility

## Customization

To modify colors or styles, edit the SVG files directly. Key attributes:
- `stroke="currentColor"` - Line color
- `fill="currentColor"` - Fill color
- `stroke-width="1.5"` - Line thickness
- `opacity="0.3"` - Transparency for accents
