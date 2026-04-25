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
  Sphere.h           Sphere primitive; holds a `std::shared_ptr<Material>`
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
- `Debug` defines `DEBUG` with `symbols "On"`; `Release` defines `NDEBUG` with
  `optimize "On"`.
- `.gitignore` excludes `/build`, `*.d`, `*.o`, `*.exe`, `*.ppm`, `*.make`, `Makefile`.

The renderer writes `test.png` to the current working directory (not into `output/`).
The `output/` directory holds curated reference images and is not the runtime target.

## Runtime Configuration

Configuration lives as globals near the top of `main.cpp`:

- `nx`, `ny` — image dimensions (currently 200×100).
- `ns` — samples per pixel (currently 100).
- `CORES` — fixed at 4 inside `main`; the printed `std::thread::hardware_concurrency()`
  is informational only and is **not** used to size the thread pool.
- Camera position, FOV (45°), aperture (0.1), focus distance (10.0) are hardcoded in
  `main`.
- Max ray bounce depth is hardcoded to 50 in `color()`.

`T_MAX` in `main.cpp` is `numeric_limits<float>::max()` and is used as the upper
bound for ray parameter `t` in world traversal. (The original name `MAXFLOAT`
collides with a glibc macro from `<math.h>` and was renamed.)

## Architecture & Conventions

### Class structure
- `Hitable` is the abstract base for scene primitives (`hit(...)` returns the closest
  intersection in `[t_min, t_max]` via a `hit_record`).
- `Material` is the abstract base for shading; `scatter` produces an outgoing ray and
  attenuation. The three concrete materials are `Lambertian`, `Metal`, `Dielectric`.
- `HitableList::add<T>(args...)` perfect-forwards to `new T(...)` and stores in a
  `vector<unique_ptr<Hitable>>`. Use this rather than constructing primitives directly.

### Memory ownership
- `Sphere` holds a `std::shared_ptr<Material>`; construct via `std::make_shared<...>`
  at call sites (see `init_random_scene`). `hit_record::material` is a non-owning raw
  pointer obtained via `material.get()` inside `Sphere::hit`.
- `HitableList` owns its hitables via `unique_ptr`.
- The render buffer in `main` is a `std::vector<uint8_t>`; pass `data.data()` to the
  C-style consumers (`do_job`, `flip`, `stbi_write_png`).

### Naming
- Types: `PascalCase` (`Sphere`, `Material`, `Camera`).
- Type aliases for the float specializations: lowercase (`vec3`, `ray`).
- Free helpers and methods: `snake_case` (`unit_vec`, `random_in_unit_sphere`,
  `point_at_parameter`).
- Headers use `#pragma once`.

### Style
- Header-only definitions for small classes. Member functions defined out-of-class
  inside headers (e.g. `Sphere::hit`, `HitableList::hit`) and free helpers in headers
  must be marked `inline` to avoid ODR violations across translation units.
- C++11/14 idioms: range-for, `auto`, `unique_ptr` / `shared_ptr`, variadic templates.
- `get_rand()` uses a `thread_local` `std::mt19937` seeded from `std::random_device`,
  with a `uniform_real_distribution<float>` over `[0, 1)`.

## Threading Model

`main` partitions the image into `CORES` horizontal stripes of `ny / CORES` rows and
spawns one `std::thread` per stripe running `do_job`. Each thread writes to disjoint
rows of the shared `data` buffer, so no synchronization is needed. After joining, the
buffer is flipped vertically (`flip`) before being passed to `stbi_write_png`.

Caveats to keep in mind when modifying:
- `get_rand()` is now thread-safe (per-thread `std::mt19937`). Output varies between
  runs because each thread seeds from `std::random_device`.
- If `ny % CORES != 0`, the bottom rows are not rendered. Preserve divisibility or
  fix the partitioning explicitly.
- Materials are shared read-only across threads via `Sphere::material`
  (`std::shared_ptr<Material>`).

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
