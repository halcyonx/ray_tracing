#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "Sphere.h"
#include "Material.h"
#include "HitableList.h"
#include "Camera.h"
#include <limits>
#include <chrono>
#include <algorithm>
#include <thread>

const float MAXFLOAT = std::numeric_limits<short>::max();

void flip(void *image, int w, int h, int bytes_per_pixel)
{
   int row;
   size_t bytes_per_row = (size_t)w * bytes_per_pixel;
   uint8_t temp[2048];
   uint8_t *bytes = (uint8_t *)image;

   for (row = 0; row < (h>>1); row++)
   {
      uint8_t *row0 = bytes + row*bytes_per_row;
      uint8_t *row1 = bytes + (h - row - 1)*bytes_per_row;
      // swap row0 with row1
      size_t bytes_left = bytes_per_row;
      while (bytes_left)
      {
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

vec3 color(const ray & r, const Hitable & world, int depth)
{
    hit_record rec;

    if (world.hit(r, 0.001f, MAXFLOAT, rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.material->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            return vec3(0.f);
        }
    }
    else
    {
        vec3 unit_direction = unit_vec(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);
        return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
    }
}

void init_random_scene(HitableList & world)
{
    int n = 500;
    world.add<Sphere>(vec3{0.f, -1000.f, 0.f}, 1000.f, new Lambertian(vec3{0.5f, 0.5f, 0.5f}));

    int i = 1;
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float choose_material = get_rand();
            vec3 center {a + 0.9f * get_rand(), 0.2f, b + 0.9f*get_rand()};

            if ((center - vec3(4.f, 0.2f, 0.f)).length() > 0.9f)
            {
                if (choose_material < 0.8)
                {  
                    // diffuse
                    world.add<Sphere>(center, 0.2, new Lambertian(vec3(get_rand()*get_rand(), get_rand()*get_rand(), get_rand()*get_rand())));
                }
                else if (choose_material < 0.95)
                { 
                    // metal
                    world.add<Sphere>(center, 0.2,
                            new Metal(vec3(0.5*(1 + get_rand()), 0.5*(1 + get_rand()), 0.5*(1 + get_rand())),  0.5*get_rand()));
                }
                else
                {  
                    // glass
                    world.add<Sphere>(center, 0.2, new Dielectric(1.5));
                }
            }
        }
    }

    world.add<Sphere>(vec3(0, 1, 0), 1.0, new Dielectric(1.5));
    world.add<Sphere>(vec3(-4, 1, 0), 1.0, new Lambertian(vec3(0.4, 0.2, 0.1)));
    world.add<Sphere>(vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0));

    std::cout << "Random scene initialized\n";
}

// int nx = 1920;
// int ny = 1080;
// int ns = 200;

int nx = 200;
int ny = 100;
int ns = 100;

void do_job(uint8_t * data, HitableList & world, Camera & camera, int from, int to)
{
    for (int j = from; j < from + to; j++)
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
                col += color(r, world, 0);
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
        }
    }
}

int main(int argc, char const *argv[])
{
    uint8_t * data = new uint8_t [nx * ny * 4];
    uint32_t cores = std::max<int>(1, std::thread::hardware_concurrency());
    std::cout << "Num of threads: " << cores << "\n";

    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    Camera camera(lookfrom, lookat, vec3{0, 1, 0}, 45, float(nx)/float(ny), aperture, dist_to_focus);

    HitableList world;
    init_random_scene(world);
    // world.add<Sphere>(vec3{0.f, -100.5f, -1.f}, 100.f, new Lambertian(vec3{0.8f, 0.8f, 0.0f}));
    // world.add<Sphere>(vec3{-1.f, 0.f, -1.f}, 0.5f, new Metal(vec3{0.8f, 0.8f, 0.8f}, 0.3f));
    // world.add<Sphere>(vec3{-1.f, 0.f, -1.f}, -0.45f, new Dielectric(1.5f));
    // world.add<Sphere>(vec3{-1.f, 0.f, -1.f}, 0.5f, new Dielectric(1.5f));
    // world.add<Sphere>(vec3{0.f, 0.f, -1.f}, 0.5f, new Lambertian(vec3{0.1f, 0.2f, 0.5f}));
    // world.add<Sphere>(vec3{1.f, 0.f, -1.f}, 0.5f, new Metal(vec3{0.8f, 0.6f, 0.2f}, 1.0f));

    // std::cout << "P3\n" << nx << " " << ny  << "\n255\n";
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    const int CORES = 4;

    int step = ny / CORES;
    std::thread jobs[CORES];
    for (int i = 0; i < CORES; i++)
    {
        jobs[i] = std::thread([&]
        (int k)
        {
            do_job(data, world, camera, k * step, step);
        }, i);
    }

    for (int i = 0; i < CORES; i++)
    {
        jobs[i].join();
    }

    flip(data, nx, ny, 4);
    stbi_write_png("test.png", nx, ny, 4, data, nx * 4);
    delete [] data;

    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    std::cout << "Time elapsed = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() <<std::endl;

    return 0;
}