# Sketch Mode

In sketch mode the analysed structure is defined by painting stiffness onto the canvas. Black represents 100% stiffness (solid material) and white represents 0% stiffness (void). Any gray value in between is treated as a proportionally reduced stiffness.

Sketch mode is selected using the **Sketch** mode button in the lower right corner of the window.

## Drawing with the pen tool

The pen tool is the primary way to draw structure. Select the **pen task** in the left toolbar. The right toolbar then shows the pen properties:

- **Stiffness** — click one of the grayscale buttons to choose the stiffness value (black = 100%, white = 0%).
- **Pen size** — click one of the five circles to select the pen thickness.

Draw by moving the cursor over the canvas while holding the **left mouse button** down. The pen leaves a trace with the selected stiffness and thickness.

The **eraser tool** works identically to the pen tool but always uses 0% stiffness (white), removing material.

## Filling surfaces

To fill a closed region with a uniform stiffness, use the **flood fill tool** from the left toolbar.

1. Select the stiffness in the right property toolbar.
2. Click inside the closed region with the left mouse button.

The fill spreads from the clicked point until it reaches the boundary of the region.

## Geometric tools

The geometric tools category (left toolbar) provides shape drawing tools for quickly creating regular shapes.

### Rectangle tool

Click to set the start corner, drag with the left mouse button, and release at the opposite corner. The rectangle is drawn with the currently selected stiffness.

### Circle / ellipse tool

Click to set the start point, drag, and release at the end point. The bounding box of the drag defines the ellipse.

### Line tool

Click to set the start point, drag, and release at the end point. Line thickness is controlled by the pen size setting.

## Using the drawing grid

For more precise drawings, activate snap-to-grid with **Settings → Snap to Grid**. The grid can also be activated temporarily while drawing by holding the **Shift** key.

The grid spacing matches the calculation grid set in **Settings → Calculation…**.

## Cut, Copy, and Paste

ForcePAD has a basic clipboard for moving or duplicating parts of the drawing.

First, switch to the **selection task** in the left toolbar. Create a selection by clicking the start point, dragging, and releasing at the end point. A selection box appears over the canvas.

With a selection active, the right toolbar shows the clipboard tools:

- **Copy** — copies the pixels inside the selection to the ForcePAD clipboard, leaving the drawing unchanged.
- **Cut** — copies the pixels to the clipboard and replaces them with white (no stiffness) on the canvas.
- **Paste** — the clipboard contents appear under the cursor. Move them to the desired position and click the left mouse button to stamp a copy onto the canvas. Multiple clicks paste multiple copies.

## Expanding the workspace

When the ForcePAD window is resized the drawing surface does not automatically resize. To expand the surface to fill the new window size:

1. Select the **selection tools** button in the left toolbar.
2. Click the **expand workspace** button in the right toolbar.

The existing drawing is copied to the lower-left corner of the expanded canvas.
