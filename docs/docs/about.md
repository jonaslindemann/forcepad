# About ForcePAD

ForcePAD is a sketch-based finite element analysis tool developed at the Division of Structural Mechanics, Lund University, by Jonas Lindemann. The application is designed to make 2D structural analysis immediately accessible: users draw shapes with a pen tool, place forces and constraints with mouse clicks, and results are shown without delay.

The conceptual design is based on the same model as consumer image-editing applications. This lowers the entry barrier for students and engineers exploring structural form and topology.

## Academic background

ForcePAD has been used in structural mechanics education and research at Lund University. The application implements a plane-stress finite element solver built on the [CALFEM](https://www.byggmek.lth.se/english/software/calfem/) framework and the `newmat` matrix library.

As of version 2.4, ForcePAD also includes a topology optimizer based on the Sigmund filter, allowing automatic derivation of efficient structural shapes from an initial design domain.

## Implementation

ForcePAD is a C++ application using OpenGL for hardware-accelerated rendering. The codebase supports two UI targets:

| Target | Framework | Status |
| --- | --- | --- |
| `forcepad` | FLTK + OpenGL | Production |
| `qtforcepad` | Qt6 + QOpenGLWidget | In development |

Both targets share the same FEM solver and core libraries (`calfemcxx`, `newmat`, `ivf2d`).

## Future development

- Qt6 UI port — a fully cross-platform Qt6 build is in progress alongside the existing FLTK build.
- Improved result visualization and export options.
- Deeper integration with CALFEM for Python as a pre-processing tool for student workflows.

Jonas Lindemann, 2026
