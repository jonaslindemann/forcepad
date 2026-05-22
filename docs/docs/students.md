# Learning FEM with ForcePAD

ForcePAD is useful for learning finite element analysis because it makes structural mechanics visible and interactive. Instead of starting with mesh generation and solver setup, students can sketch a structure, apply loads, and immediately ask what changed.

## Experiments to Try

### Beam Bending

1. Draw a horizontal beam.
2. Constrain the left edge.
3. Apply a downward force near the right edge.
4. Switch to displacement visualization.
5. Move the force closer to the support and compare the deformation.

What to look for:

- Where is the displacement largest?
- How does the deformation change as the load moves?
- Which side of the beam is in tension and which side is in compression?

### Support Conditions

1. Start from the same beam.
2. Replace the left-side support with different constraint directions.
3. Compare the displacement and stress visualizations.

What to look for:

- Which constraints prevent rigid-body motion?
- Which constraints create large local stresses?
- How does the structure behave when it is under-constrained?

### Stress Concentrations

1. Draw a rectangular plate.
2. Remove a circular region from the middle.
3. Apply tension or bending loads.
4. View von Mises stress.

What to look for:

- Where do high stresses appear?
- How does the hole shape influence the stress field?
- What happens if the hole is moved closer to an edge?

### Load Paths

1. Draw a broad design domain.
2. Add two supports and one force.
3. View principal stress directions.
4. Rotate or move the force in Action mode.

What to look for:

- How do tensile and compressive stress directions change?
- Which parts of the domain appear important for carrying the load?
- How do support locations influence the load path?

### Topology Optimisation

1. Draw a filled rectangular domain.
2. Add supports and a force.
3. Run topology optimisation in Action mode.
4. Compare the final shape with the principal stress visualization.

What to look for:

- Which regions are removed?
- Which material paths remain?
- How does the result change when the volume fraction changes?

## Teacher Notes to Add

!!! note "Teaching material placeholder"
    Add downloadable worksheets, expected screenshots, and short explanation notes for each experiment. A useful first set would be a 30 minute lab exercise and a 90 minute classroom activity.
