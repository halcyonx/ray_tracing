#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"
#include <limits>

const float MAXFLOAT = std::numeric_limits<short>::max();

float get_rand()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void flip(void *image, int w, int h, int bytes_per_pixel)
{
   int row;
   size_t bytes_per_row = (size_t)w * bytes_per_pixel;
   uint8_t temp[2048];
   uint8_t *bytes = (uint8_t *)image;

   for (row = 0; row < (h>>1); row++) {
      uint8_t *row0 = bytes + row*bytes_per_row;
      uint8_t *row1 = bytes + (h - row - 1)*bytes_per_row;
      // swap row0 with row1
      size_t bytes_left = bytes_per_row;
      while (bytes_left) {
         size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
         memcpy(temp, row0, bytes_copy);
         memcpy(row0, row1, bytes_copy);
         memcpy(row1, temp, bytes_copy);
         row0 += bytes_copy;
         row1 += bytes_copy;
         bytes_left -= bytes_copy;
      }
   }
}

template <typename T, typename R = typename std::decay<T>::type>
constexpr typename std::enable_if<!std::is_integral<R>::value, R>::type lerp(T v1, T v2, float t)
{
    // return (t <= 0.0f) ? v1 : ((t >= 1.0f) ? v2 : v1 + (v2 - v1) * t);
    return (1.0f - t) * v1 + t * v2;
}

vec3 random_in_unit_sphere()
{
    vec3 p;
    do
    {
        p = 2.0f * vec3(get_rand(), get_rand(), get_rand()) - vec3(1.f, 1.f, 1.f);
    }
    while(p.squared_length() >= 1.f);

    return p;
}

vec3 color(const ray & r, const Hitable & world)
{
    hit_record rec;

    if (world.hit(r, 0.001f, MAXFLOAT, rec))
    {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5f * color( ray(rec.p, target - rec.p), world );
        // return 0.5f * vec3{rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1};
    }
    else
    {
        vec3 unit_direction = unit_vec(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);
        return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
    }
}

int main(int argc, char const *argv[])
{
    int nx = 400;
    int ny = 200;
    int ns = 200;

    uint8_t * data = new uint8_t [nx * ny * 4];

    Camera camera;

    HitableList world;
    world.add<Sphere>(vec3{0.f, -100.5f, -1.f}, 100.f);
    world.add<Sphere>(vec3{0.f, 0.f, -1.f}, 0.5f);

    // std::cout << "P3\n" << nx << " " << ny  << "\n255\n";

    // for (int j = 0; j < ny; j++)
    for (int j = ny-1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0.f);
            for (int s = 0; s < ns; s++)
            {
                float u = static_cast<float>(i + get_rand()) / static_cast<float>(nx);
                float v = static_cast<float>(j + get_rand()) / static_cast<float>(ny);

                ray r = camera.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0f);
                col += color(r, world);
            }

            col /= float(ns);

            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );

            int ir = static_cast<int>(255.9f * col.r());
            int ig = static_cast<int>(255.9f * col.g());
            int ib = static_cast<int>(255.9f * col.b());

            data[4 * (j * nx + i)] = ir;
            data[4 * (j * nx + i)+ 1] = ig;
            data[4 * (j * nx + i)+ 2] = ib;
            data[4 * (j * nx + i)+ 3] = 255;

            // std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    flip(data, nx, ny, 4);
    stbi_write_png("test.png", nx, ny, 4, data, nx * 4);
    delete [] data;

    return 0;
}