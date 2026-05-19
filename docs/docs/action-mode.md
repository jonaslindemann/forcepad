# Action Mode

Action mode displays the results of the finite element analysis and allows interactive exploration of the structure's behaviour. Switch to this mode using the **Action** button in the lower right corner of the window.

The FEM calculation runs automatically when action mode is first entered. Results are updated in real-time when forces are moved or rotated.

## Visualization modes

The left toolbar in action mode selects which result quantity is displayed. The right toolbar updates to show the relevant display controls.

### Principal stresses

Principal stress visualization is the default when action mode is activated. Select the **principal** task category button in the left toolbar.

Result arrows are drawn across the structure:

- **Red arrows** — tensile stresses (pulling apart)
- **Blue arrows** — compressive stresses (pushing together)

The right toolbar provides the following controls:

| Control | Effect |
| --- | --- |
| **Size** roller | Scale the size of the stress arrows |
| **Transp** roller | Transparency of the arrows (reduce to better see the structure underneath) |
| **Step** roller | Number of grid steps between arrows — increasing reduces arrow density |
| **AutoScale** button | Automatically recalculate arrow sizes each time action mode is entered |
| Tensile/compressive toggles | Show only tensile, only compressive, or both stress arrows |

### Von Mises stress

Select the second button in the left toolbar to display the von Mises stress field as a color map.

Two color maps are available:

- **Rainbow** (default) — low stresses shown in blue, high stresses in red.
- **Hot** — low stresses shown in dark red, high stresses in yellow.

A slider controls the stress level that maps to the maximum color, allowing you to compress or expand the color range. A checkbox inverts the selected color map.

### Displacement

Select the displacement button in the left toolbar to visualize structural deformations. A slider in the right toolbar scales the magnitude of the displayed displacements.

## Interacting with forces in real-time

Action mode allows forces to be rotated and moved while the visualization updates immediately.

### Rotating forces

When action mode is first entered, ForcePAD is in **force direction** mode. Click on the tip of a force arrow and drag with the left mouse button to rotate the force. The visualization updates in real-time.

### Moving forces

Select the **force movement** mode button in the left toolbar. Click on a force tip and drag to relocate the force application point. Results update continuously.

## Magnifying the view

To examine visualizations in detail, activate the **zoom / magnify** button in the left toolbar. The view is immediately magnified. Adjust the zoom level with the scroll wheel or the **Page Up** / **Page Down** keys. Drag the mouse to pan the magnified view.

## Topology optimisation

ForcePAD includes a topology optimizer (Sigmund filter) that derives an efficient structural shape from the current design domain, forces, and constraints.

To run the optimizer:

1. In action mode, click the **optimize** button in the left toolbar.
2. A dialog appears with the initial optimization parameters (volume fraction, filter radius, penalty factor). Refer to topology optimization literature for a detailed description of these parameters.
3. Click **OK** to start. The optimizer runs iteratively, updating the displayed shape at each step.
4. Click **Stop** (lower right of the window) to halt early, or wait for the convergence criterion to be met.

### Transferring the optimized shape to sketch mode

After optimization is complete:

1. Click the **structure** button in the left toolbar to display the optimized shape without constraints or force overlays.
2. Click the **transfer structure** button in the upper right toolbar.

The optimized shape replaces the current sketch mode canvas.

!!! warning
    Transferring the optimized structure **overwrites** the existing sketch. Save your model before transferring if you want to keep the original drawing.
