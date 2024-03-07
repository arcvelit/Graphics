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

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>


void renderSceneGlobal(Buffer& buffer, SceneInfo& scene, OutputInfo& output)
{
    // Random init
    unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);


    // Look through all the grid holes
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

    // Stratification parameters
    const int GLOBAL_RAYS_SPECIFICATION = output.raysperpixel.size();

    // Specification defaults
    uint rays_per_cell(10);
    uint number_of_columns(10);
    uint number_of_rows(10);

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

    const int RAYS_PER_GRID = rays_per_cell * number_of_rows * number_of_columns;

    // Traveling on xy axis of the viewport
    for (int y = 0; y < output.size[1]; y++)
    {
        Vec3 u_vec = up * (delta_u - y * DELTA);

        for (int x = 0; x < output.size[0]; x++)
        {
            Vec3 s_vec = side * (delta_s + x * DELTA);
            Point grid_bottom_left = viewport_center + u_vec + s_vec;
            
            Vec3 y_strata(up);
            Vec3 x_strata(side);

            float x_cell_lentgh = DELTA / number_of_columns;
            float y_cell_lentgh = DELTA / number_of_rows;

            // RGB Colors
            double IR(0), IG(0), IB(0);

            // for each row,
            // for each column, 
            // throw rays through cell
            for (int cell_row = 0; cell_row < number_of_rows; cell_row++)
            {
                // Moving up a cell row in pixel
                Vec3 vertical_strata = y_cell_lentgh * y_strata;
                Vec3 vertical_cell_jump = cell_row * vertical_strata;

                for (int cell_col = 0; cell_col < number_of_columns; cell_col++)
                {
                    // Moving up a cell column in pixel
                    Vec3 horizontal_strata = x_cell_lentgh * x_strata;
                    Vec3 horizontal_cell_jump = cell_row * horizontal_strata;

                    Point cell_bottom_left = grid_bottom_left + horizontal_cell_jump + vertical_cell_jump;

                    double cell_IR(0), cell_IG(0), cell_IB(0);

                    // Shoot random rays in grid cell
                    for (int ray = 0; ray < rays_per_cell; ray++) 
                    {
                        // Random sampling
                        float random_x = distribution(generator);
                        float random_y = distribution(generator);

                        Point sampled_point = cell_bottom_left + random_y * horizontal_strata + random_x * vertical_strata;

                        // Intersect geometries in scene
                        Ray viewportRay = Ray(output.center, sampled_point - output.center);
                        PairedRoot hit = tu_IntersectSceneGeometries(viewportRay, scene);
                        if (hit.geometry == nullptr) 
                        {
                            // Background color 
                            cell_IR += output.bkc[0];
                            cell_IG += output.bkc[1];
                            cell_IB += output.bkc[2];
                            continue;
                        };

                        // Make rays bounce ==============================================================
                        for (int bounce = 0; bounce < output.maxbounces; bounce++)
                        {
                            // Make sure we're not bouncing 

                            // Ray probably terminates
                            if (output.probterminate <= distribution(generator)) break;
                        }

                        // ===============================================================================
                        
                        // Ambient light
                        cell_IR += 1 * hit.geometry->ac[0] * output.ai[0];
                        cell_IG += 1 * hit.geometry->ac[1] * output.ai[1];
                        cell_IB += 1 * hit.geometry->ac[2] * output.ai[2];
                    }

                    IR += cell_IR;
                    IG += cell_IG;
                    IB += cell_IB;
                }
            }

            IR /= RAYS_PER_GRID;
            IG /= RAYS_PER_GRID;
            IB /= RAYS_PER_GRID;


            buffer[3 * (x + IMAGE_WIDTH * y) + 0] = std::min(1.0, IR);
            buffer[3 * (x + IMAGE_WIDTH * y) + 1] = std::min(1.0, IG);
            buffer[3 * (x + IMAGE_WIDTH * y) + 2] = std::min(1.0, IB);

        }

    }
}