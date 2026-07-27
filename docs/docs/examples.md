# Examples

ForcePAD includes small `.fp2` models that can be used as starting points for teaching, demonstrations, and regression testing. The current sample files live in the repository under `bin/release/samples/`.

!!! note "Curated example placeholder"
    Add screenshots, expected result images, and short lesson notes for each example. The first high-value set should cover a cantilever beam, bridge or truss-like domain, stress concentration, topology optimisation, and an image-based model.

## Bundled Sample Models

| Example | File | Open | Suggested use |
| --- | --- | --- | --- |
| Symmetric beam | [`beam_sym.fp2`](https://github.com/jonaslindemann/forcepad/blob/master/bin/release/samples/beam_sym.fp2) | [▶](https://jonaslindemann.github.io/forcepad/app/?model=models/beam_sym.fp2) | Basic bending, support conditions, and symmetry. |
| Thick beam | [`thick_beam.fp2`](https://github.com/jonaslindemann/forcepad/blob/master/bin/release/samples/thick_beam.fp2) | [▶](https://jonaslindemann.github.io/forcepad/app/?model=models/thick_beam.fp2) | Compare coarse and thick structural domains. |
| Block | [`block.fp2`](https://github.com/jonaslindemann/forcepad/blob/master/bin/release/samples/block.fp2) | [▶](https://jonaslindemann.github.io/forcepad/app/?model=models/block.fp2) | Simple design domain for stress and optimisation experiments. |
| Demo example | [`demo_example.fp2`](https://github.com/jonaslindemann/forcepad/blob/master/bin/release/samples/demo_example.fp2) | [▶](https://jonaslindemann.github.io/forcepad/app/?model=models/demo_example.fp2) | General walkthrough model for demonstrations. |
| Pantheon | [`pantheon.fp2`](https://github.com/jonaslindemann/forcepad/blob/master/bin/release/samples/pantheon.fp2) | [▶](https://jonaslindemann.github.io/forcepad/app/?model=models/pantheon.fp2) | Image-based geometry and architectural form exploration. |
| Pantheon variants | [`pantheon2.fp2`](https://github.com/jonaslindemann/forcepad/blob/master/bin/release/samples/pantheon2.fp2), [`pantheon3.fp2`](https://github.com/jonaslindemann/forcepad/blob/master/bin/release/samples/pantheon3.fp2) | [▶](https://jonaslindemann.github.io/forcepad/app/?model=models/pantheon2.fp2) [▶](https://jonaslindemann.github.io/forcepad/app/?model=models/pantheon3.fp2) | Variants for comparing edits and responses. |

## Opening a Model From a Link

The browser version can open a model straight from a link, so an assignment or a lecture slide can point students at a prepared starting point instead of asking them to download a file and open it by hand. Add a `model` parameter to the app URL:

```text
https://jonaslindemann.github.io/forcepad/app/?model=models/beam_sym.fp2
```

Every sample model listed above is published with the app, so `models/<file>.fp2` works for any of them — the ▶ Open links in the table are exactly these URLs. If the file cannot be fetched, ForcePAD starts with an empty model and reports the problem.

### Linking to Your Own Models

The `model` value can also be a full `https://` address, and it does not have to point at anything in this project. Teachers can prepare their own models and hand out links without needing access to the ForcePAD site — the app simply fetches whatever the link points at.

The simplest route is a public GitHub repository:

1. Commit your `.fp2` file to a public repository (a [gist](https://gist.github.com) works too).
2. Open the file on GitHub and copy the **raw** address — `https://raw.githubusercontent.com/<user>/<repo>/<branch>/<path>`. The ordinary `github.com/.../blob/...` page address will not work: it serves a web page rather than the file.
3. Append it to the app URL:

```text
https://jonaslindemann.github.io/forcepad/app/?model=https://raw.githubusercontent.com/your-name/your-repo/main/models/beam.fp2
```

Paste the address as it is — no escaping or encoding is needed.

Two things make such a link durable:

- **The repository has to be public.** Raw addresses for private repositories require an access token, and those expire.
- **Point at a fixed branch or commit.** A link containing `/main/` follows the branch, so editing the model later changes what students open. Using a commit id instead of the branch name freezes it.

### If a Model Will Not Load

A model hosted somewhere else is downloaded by the browser, which enforces a few rules ForcePAD cannot work around:

- The host must allow cross-site downloads by sending the `Access-Control-Allow-Origin` header. GitHub and GitHub Pages do this by default, which is why they are the easy option; an ordinary university or personal web server usually does not, and the model will silently fail to load. If in doubt, ask whoever administers the server for it, or host the file on GitHub instead.
- The address must be `https://`. A plain `http://` address is blocked because the app itself is served over `https`.

!!! note
    A model that refuses to load in the browser will still open in the desktop version — these restrictions come from the browser, not from ForcePAD.

### From the Desktop Version

The desktop version accepts the same thing on the command line, either as a local file or as an address:

```text
forcepad https://example.org/models/beam.fp2
```

## Example Walkthroughs to Add

These are placeholders for future example pages or downloadable teaching packs.

### Cantilever Beam

Goal: show bending deformation, tensile/compressive stress regions, and the effect of moving the load.

Recommended assets to add:

- `examples/cantilever-beam.fp2`
- screenshot in Sketch mode
- screenshot in Action mode with displacement visualization
- 5-second GIF of moving the load

### Stress Concentration Plate

Goal: show how holes, sharp corners, and narrow regions influence von Mises stress.

Recommended assets to add:

- `examples/stress-concentration.fp2`
- screenshot with von Mises stress
- short note explaining why the stress field changes near geometric discontinuities

### Topology Optimisation Domain

Goal: show a rectangular design domain evolving into an efficient structural layout.

Recommended assets to add:

- `examples/topology-optimisation.fp2`
- before/after screenshots
- short GIF of the optimiser iterations

![Topology optimisation result](images/opt-tool-3.png "Topology optimisation result")

### Load Path Exploration

Goal: help students understand how load direction and support placement change principal stress directions.

Recommended assets to add:

- `examples/load-path.fp2`
- screenshot with principal stress arrows
- GIF of rotating the force in Action mode

![Principal stress visualization](images/stress-step-1.png "Principal stress visualization")

## Suggested Top-Level Examples Directory

For GitHub discoverability, a future cleanup could move curated examples into a top-level `examples/` directory and keep installer-specific copies under `bin/release/samples/`.
