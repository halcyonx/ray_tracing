# CLAUDE.md

Guidance for AI assistants working in this repository.

## Project Overview

A C++ implementation of Peter Shirley's *Ray Tracing in One Weekend*, forked from
https://github.com/petershirley/raytracinginoneweekend. The program renders a scene of
randomly placed spheres with Lambertian, metal, and dielectric materials and writes a
PNG via `stb_image_write`. Rendering is parallelized across a fixed pool of threads.

## Repository Layout

```
src/                 All header-only ray tracer code plus main.cpp
  Vec.h              Templated Vec3<T>; `using vec3 = Vec3<float>`
  Ray.h              Templated Ray<T>; `using ray = Ray<float>`
  Hitable.h          `Hitable` interface, `hit_record` POD
  HitableList.h      Container of `Hitable::Ptr` (unique_ptr) with templated `add<T>(args...)`
  Sphere.h           Sphere primitive; owns its `Material*` and deletes it in dtor
  Material.h         `Material` interface + `Lambertian`, `Metal`, `Dielectric`;
                     also defines free helpers `get_rand`, `random_in_unit_sphere`,
                     `reflect`, `refract`, `schlick`
  Camera.h           Pinhole camera with depth-of-field (aperture, focus distance);
                     defines `random_in_unit_disk` and `PI`
  main.cpp           Scene setup, threaded render loop, PNG output
  stb_image.cpp      Defines STB_IMAGE_IMPLEMENTATION
  stb_image.h        Vendored stb_image
  stb_image_write.h  Vendored stb_image_write (impl macro defined in main.cpp)
output/              Reference renders (test_01.png … test_12.png)
premake5.lua         Premake build configuration
```

There is no separate `.cpp` for `Sphere`, `HitableList`, or materials — definitions live
in their headers. `main.cpp` is the only translation unit beyond `stb_image.cpp`.

## Build & Run

The project uses **Premake5**. Premake is not committed; install it locally.

```bash
premake5 gmake2          # generates Makefiles into ./proj
make -C proj config=release_x64
./build/x64/bin/Release/RayTracing
```

Notes from `premake5.lua` worth knowing before touching it:
- Workspace `RayTracing`, single project `RayTracing` (kind `ConsoleApp`, C++).
- Build artifacts: `build/%{cfg.architecture}/bin/%{cfg.buildcfg}` and `obj/...`.
- Project files generated into `proj/`.
- The Debug/Release filters are **swapped** in the current config: `Release` defines
  `DEBUG` with `symbols "On"`, and `Debug` defines `NDEBUG` with `optimize "On"`. Do
  not "fix" this casually — preserve existing behavior unless the user asks.
- `.gitignore` excludes `/build`, `*.d`, `*.o`, `*.exe`, `*.ppm`, `*.make`, `Makefile`.

The renderer writes `test.png` to the current working directory (not into `output/`).
The `output/` directory holds curated reference images and is not the runtime target.

## Runtime Configuration

Configuration lives as globals near the top of `main.cpp`:

- `nx`, `ny` — image dimensions (currently 200×100; a 1920×1080 set is commented out).
- `ns` — samples per pixel (currently 100).
- `CORES` — fixed at 4 inside `main`; the printed `std::thread::hardware_concurrency()`
  is informational only and is **not** used to size the thread pool.
- Camera position, FOV (45°), aperture (0.1), focus distance (10.0) are hardcoded in
  `main`.
- Max ray bounce depth is hardcoded to 50 in `color()`.

`MAXFLOAT` in `main.cpp` is set to `numeric_limits<short>::max()` (~32767). This is
intentional in the existing code; do not silently change it.

## Architecture & Conventions

### Class structure
- `Hitable` is the abstract base for scene primitives (`hit(...)` returns the closest
  intersection in `[t_min, t_max]` via a `hit_record`).
- `Material` is the abstract base for shading; `scatter` produces an outgoing ray and
  attenuation. The three concrete materials are `Lambertian`, `Metal`, `Dielectric`.
- `HitableList::add<T>(args...)` perfect-forwards to `new T(...)` and stores in a
  `vector<unique_ptr<Hitable>>`. Use this rather than constructing primitives directly.

### Memory ownership (important, not idiomatic)
- `Sphere` takes a raw `Material*` and **deletes it in its destructor**. Callers must
  pass a freshly `new`'d material (see `init_random_scene`). Do not share a single
  `Material*` across multiple `Sphere`s — that would double-free.
- `HitableList` owns its hitables via `unique_ptr`.
- The render buffer in `main` is `new uint8_t[...]` and `delete[]`'d manually.

If refactoring ownership, preserve these invariants or update all call sites.

### Naming
- Types: `PascalCase` (`Sphere`, `Material`, `Camera`).
- Type aliases for the float specializations: lowercase (`vec3`, `ray`).
- Free helpers and methods: `snake_case` (`unit_vec`, `random_in_unit_sphere`,
  `point_at_parameter`).
- Headers use `#pragma once`.

### Style
- Header-only definitions for small classes; inline keyword on hot accessors.
- C++11/14 idioms: range-for, `auto`, `unique_ptr`, variadic templates.
- Existing code mixes `float` and `double` literals and uses C-style `rand()` via
  `get_rand()` in `Material.h` — match the surrounding style when editing.

## Threading Model

`main` partitions the image into `CORES` horizontal stripes of `ny / CORES` rows and
spawns one `std::thread` per stripe running `do_job`. Each thread writes to disjoint
rows of the shared `data` buffer, so no synchronization is needed. After joining, the
buffer is flipped vertically (`flip`) before being passed to `stbi_write_png`.

Caveats to keep in mind when modifying:
- `get_rand()` uses `rand()`, which is not thread-safe / well-distributed across
  threads on all platforms. The existing code accepts this.
- If `ny % CORES != 0`, the bottom rows are not rendered. Preserve divisibility or
  fix the partitioning explicitly.
- Materials are shared read-only across threads via `Sphere::material`.

## Working in This Repo

- Prefer editing the existing headers over splitting into `.cpp` files unless the user
  asks — it would change the build graph and contradict the current style.
- Do not commit anything under `build/` or generated `proj/` files; `.gitignore` covers
  the common artifacts but not `proj/` itself.
- Reference renders in `output/` are committed; if you change the renderer in a way
  that materially changes output, mention it but do not regenerate `output/*.png`
  unless asked.
- There is no test suite, linter, or formatter configured. "Working" means it compiles
  and produces a sensible image.

## Git Workflow (this session)

Active development branch: `claude/add-claude-documentation-aY4au`. Commit and push to
this branch; do not push elsewhere without explicit instruction.
