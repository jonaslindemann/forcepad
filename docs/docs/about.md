# About ForcePAD

ForcePAD is a sketch-based finite element analysis tool developed at the Division of Structural Mechanics, Lund University, by Jonas Lindemann. The application is designed to make 2D structural analysis immediately accessible: users draw shapes with a pen tool, place forces and constraints with mouse clicks, and results are shown without delay.

The conceptual design is based on the same model as consumer image-editing applications. This lowers the entry barrier for students and engineers exploring structural form and topology.

## Academic background

ForcePAD has been used in structural mechanics education and research at Lund University. The application implements a plane-stress finite element solver built on a C++ version of the CALFEM  framework and the `eigen` matrix library.

As of version 2.4, ForcePAD also includes a topology optimizer based on the Sigmund filter, allowing automatic derivation of efficient structural shapes from an initial design domain.

## Implementation

ForcePAD is a C++ application using OpenGL for hardware-accelerated rendering. The user interface is built with the Qt framework, and the application is cross-platform, running on Windows, macOS, and Linux. The source code is available on GitHub under the GPL-2 License.

Jonas Lindemann, 2026
