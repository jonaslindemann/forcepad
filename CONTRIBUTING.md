# Contributing to ForcePAD

Thanks for considering a contribution. ForcePAD is a long-running educational finite element project, so small, focused improvements are especially useful.

## Good First Contributions

- Improve documentation, screenshots, and quick-start material.
- Add curated `.fp2` example models with short teaching notes.
- Report reproducible bugs with model files and screenshots.
- Improve build notes for Windows, macOS, or Linux.
- Polish Qt UI behaviour and small workflow issues.

## Development Setup

ForcePAD uses CMake and vcpkg. The primary maintained application target is `qtforcepad`.

```powershell
cmake -B build-debug
cmake --build build-debug --config Debug
```

The executable is written to `bin/Debug/qtforcepad.exe` on Windows.

## Pull Requests

Before opening a pull request:

1. Keep the change focused.
2. Build the affected target when code changes are involved.
3. Update documentation when behaviour changes.
4. Include screenshots or `.fp2` sample files when visual behaviour changes.

## Examples and Teaching Material

Example contributions are welcome. A strong example usually includes:

- a small `.fp2` model
- one screenshot of the model setup
- one screenshot of the result
- a short note explaining the structural mechanics concept

See `examples/README.md` and the documentation examples page for planned example categories.
