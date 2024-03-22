#include "SceneInfo.h"
#include "Geometry.h"
#include "Light.h"
#include "BufferUtils.h"
#include "TracerUtils.h"
#include "PairedRoot.h"
#include "Types.h"
#include "Ray.h"

#include "RenderTechniques.h"

#include <Eigen/Dense>

# define M_PI 3.14159265358979323846

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>




void renderSceneAntialiasing(Buffer& buffer, SceneInfo& scene, OutputInfo& output)
{
    // Random
    unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    const int IMAGE_HEIGHT = output.size[1];
    const int IMAGE_WIDTH  = output.size[0];

    // Pixel size
    const double DELTA = 2 * tan(output.fov / 2) / IMAGE_HEIGHT;

    // Directions on the viewport
    Vec3 up = output.up;
    Vec3 side = output.lookat.cross(output.up).normalized();
    Point viewport_center = output.center + output.lookat;

    // Coefficients to reach first pixel on viewport
    float delta_u = (IMAGE_HEIGHT * DELTA / 2) - DELTA;
    float delta_s = -(IMAGE_WIDTH * DELTA / 2);


    // Specification defaults
    uint rays_per_cell(10);
    uint number_of_columns(10);
    uint number_of_rows(10);

    // Stratification parameters
    const int GLOBAL_RAYS_SPECIFICATION = output.raysperpixel.size(); 
    if (GLOBAL_RAYS_SPECIFICATION == 1)
    {
        rays_per_cell = output.raysperpixel[0];
        number_of_columns  = 1;
        number_of_rows     = 1;
    } else if (GLOBAL_RAYS_SPECIFICATION == 2)
    {
        rays_per_cell = output.raysperpixel[1];
        number_of_columns  = output.raysperpixel[0];
        number_of_rows     = output.raysperpixel[0];    
    } else if (GLOBAL_RAYS_SPECIFICATION == 3)
    {
        rays_per_cell = output.raysperpixel[2];
        number_of_columns  = output.raysperpixel[1];
        number_of_rows     = output.raysperpixel[0];    
    }

    const int RAYS_PER_PIXEL = rays_per_cell * number_of_rows * number_of_columns;

    // Traveling on xy axis of the viewport
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        Vec3 u_vec = up * (delta_u - y * DELTA);

        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            Vec3 s_vec = side * (delta_s + x * DELTA);
            Point grid_bottom_left = viewport_center + u_vec + s_vec;


            float y_cell_lentgh = DELTA / number_of_rows;
            float x_cell_lentgh = DELTA / number_of_columns;

            Vec3 vertical_strata = y_cell_lentgh * up;
            Vec3 horizontal_strata = x_cell_lentgh * side;

            // RGB Colors
            Color COLOR = Color::Zero();
            int voided_rays(0);

            // for each row,
            // for each column, 
            // throw rays through cell
            for (int cell_row = 0; cell_row < number_of_rows; cell_row++)
            {
                // Moving up a cell row in pixel
                Vec3 vertical_cell_jump = cell_row * vertical_strata;

                for (int cell_col = 0; cell_col < number_of_columns; cell_col++)
                {
                    // Moving up a cell column in pixel
                    Vec3 horizontal_cell_jump = cell_col * horizontal_strata;

                    Point cell_bottom_left = grid_bottom_left + horizontal_cell_jump + vertical_cell_jump;


                    // Shoot random rays in grid cell
                    for (int ray = 0; ray < rays_per_cell; ray++) 
                    {
                        Color cell_COLOR = Color::Zero();

                        // Random sampling
                        float random_y = distribution(generator);
                        float random_x = distribution(generator);

                        Point sampled_point = cell_bottom_left + random_y * horizontal_strata + random_x * vertical_strata;

                        // Geometry through viewport
                        Ray viewportRay = Ray(output.center, sampled_point - output.center);
                        PairedRoot hit = tu_IntersectSceneGeometries(viewportRay, scene);
                        if (hit.geometry == nullptr) 
                        {
                            // Background color 
                            COLOR += output.bkc.cwiseMin(Color::Ones());
                            continue;
                        };

                        // Colored geometry point
                        Point p = viewportRay.reach(hit.root);
                        
                        Vec3 Nhat = tu_GetNormal(p, hit.geometry);

                        for (Light light : scene.lights)
                        {
                            Ray lightRay = Ray(p, light.center - p);

                            PairedRoot eclipse = tu_IntersectSceneGeometries(lightRay, scene);

                            // Geometry eclipses the light
                            if (tu_PointInShadow(light.center, eclipse, hit, p)) {continue;}

                            // Diffuse light
                            float diffuseFactor = hit.geometry->kd * std::max(0.0f, lightRay.direction.dot(Nhat));
                            
                            cell_COLOR += diffuseFactor * hit.geometry->dc.cwiseProduct(light.id);

                            // Specular light
                            Vec3 Hhat = (lightRay.direction - viewportRay.direction).normalized();

                            float speculativeFactor = hit.geometry->ks * std::pow(std::max(0.0f, Nhat.dot(Hhat)), hit.geometry->pc);

                            cell_COLOR += speculativeFactor * hit.geometry->sc.cwiseProduct(light.is);
                        }
                            
                        // Ambient light
                        cell_COLOR += hit.geometry->ka * hit.geometry->ac.cwiseProduct(output.ai);

                        COLOR += cell_COLOR.cwiseMin(Color::Ones());
                    }

                }
            }

            COLOR /= RAYS_PER_PIXEL;


            buffer[3 * (x + IMAGE_WIDTH * y) + 0] = (double)COLOR[0];
            buffer[3 * (x + IMAGE_WIDTH * y) + 1] = (double)COLOR[1];
            buffer[3 * (x + IMAGE_WIDTH * y) + 2] = (double)COLOR[2];

        }
        std::cout << 100*y/IMAGE_HEIGHT << "%\t\r" << std::flush;
    }
}
