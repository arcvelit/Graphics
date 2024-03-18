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




void renderSceneGlobal(Buffer& buffer, SceneInfo& scene, OutputInfo& output)
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
            double IR(0), IG(0), IB(0);

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
                        double cell_IR(0), cell_IG(0), cell_IB(0);

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
                            IR += std::min(1.0f, output.bkc[0]);
                            IG += std::min(1.0f, output.bkc[1]);
                            IB += std::min(1.0f, output.bkc[2]);
                            continue;
                        };

                        // Colored geometry point
                        Vec3 p = viewportRay.reach(hit.root);

                        // Intensities
                        double diffuse_damp_R(1), diffuse_damp_G(1), diffuse_damp_B(1);

                        // Make rays bounce ==============================================================
                        // Objective: modify damping factor and hit geometry 
                        PairedRoot last_geometry = hit;
                        for (int bounce = 0; bounce < output.maxbounces; bounce++) 
                        {
                            if (output.probterminate > distribution(generator)) { break; }

                            // Change hit geometry
                            Vec3 normal;
                            if (last_geometry.geometry->type == RECTANGLE) normal = last_geometry.geometry->normal;
                            else if (last_geometry.geometry->type == SPHERE) normal = (p - last_geometry.geometry->center).normalized();

                            // Hemisphere bounce
                            float theta = 2 * M_PI * distribution(generator);
                            float phi = std::acos(1-distribution(generator)); 

                            Eigen::Matrix3f model_frame = tu_GetLocalCoordinatesFrame(normal);
                            Vec3 spherical_coordinates = 
                            {
                                std::cos(theta) * std::sin(phi),
                                std::cos(phi),
                                std::sin(theta) * std::sin(phi)
                            };

                            Vec3 bounce_direction = model_frame * spherical_coordinates;

                            Ray bouncingRay = Ray(p, bounce_direction);

                            PairedRoot collided = tu_IntersectSceneGeometries(bouncingRay, scene);

                            // Collides 
                            if (collided.geometry == nullptr) {  break; }

                            p = bouncingRay.reach(collided.root);

                            // Damping of diffuse intensity
                            float diffuseFactor = last_geometry.geometry->kd * std::max(0.0f, bouncingRay.direction.dot(normal));

                            diffuse_damp_R *= diffuseFactor * last_geometry.geometry->dc[0];
                            diffuse_damp_G *= diffuseFactor * last_geometry.geometry->dc[1];
                            diffuse_damp_B *= diffuseFactor * last_geometry.geometry->dc[2];

                            last_geometry = collided;
                        }
                        // ===============================================================================
                        // Now light it up
                        if (hit.geometry == nullptr) continue;
                        
                        Vec3 Nhat;
                        if (last_geometry.geometry->type == RECTANGLE) Nhat = last_geometry.geometry->normal;
                        else if (last_geometry.geometry->type == SPHERE) Nhat = (p - last_geometry.geometry->center).normalized();

                        for (Light light : scene.lights)
                        {
                            Ray lightRay = Ray(p, light.center - p);

                            PairedRoot eclipse = tu_IntersectSceneGeometries(lightRay, scene);

                            // Geometry eclipses the light
                            if (tu_PointInShadow(light.center, eclipse, last_geometry, p)) {continue;}

                            // Diffuse light
                            float diffuseFactor = hit.geometry->kd * std::max(0.0f, lightRay.direction.dot(Nhat));

                            cell_IR += diffuseFactor * hit.geometry->dc[0] * light.id[0] * diffuse_damp_R;
                            cell_IG += diffuseFactor * hit.geometry->dc[1] * light.id[1] * diffuse_damp_G;
                            cell_IB += diffuseFactor * hit.geometry->dc[2] * light.id[2] * diffuse_damp_B;

                            // Specular light
                            if (!output.globalillum)
                            {
                                Vec3 Hhat = (lightRay.direction - viewportRay.direction).normalized();

                                float speculativeFactor = hit.geometry->ks * std::pow(std::max(0.0f, Nhat.dot(Hhat)), hit.geometry->pc);

                                cell_IR += speculativeFactor * hit.geometry->sc[0] * light.is[0];
                                cell_IG += speculativeFactor * hit.geometry->sc[1] * light.is[1];
                                cell_IB += speculativeFactor * hit.geometry->sc[2] * light.is[2];
                            }
                        }

                            
                        // Ambient light
                        if (!output.globalillum)
                        {
                            cell_IR += hit.geometry->ka * hit.geometry->ac[0] * output.ai[0];
                            cell_IG += hit.geometry->ka * hit.geometry->ac[1] * output.ai[1];
                            cell_IB += hit.geometry->ka * hit.geometry->ac[2] * output.ai[2];
                        }

                        IR += std::min(1.0, cell_IR);
                        IG += std::min(1.0, cell_IG);
                        IB += std::min(1.0, cell_IB);
                    }

                }
            }

            IR /= RAYS_PER_GRID;
            IG /= RAYS_PER_GRID;
            IB /= RAYS_PER_GRID;


            buffer[3 * (x + IMAGE_WIDTH * y) + 0] = std::min(1.0, IR);
            buffer[3 * (x + IMAGE_WIDTH * y) + 1] = std::min(1.0, IG);
            buffer[3 * (x + IMAGE_WIDTH * y) + 2] = std::min(1.0, IB);

        }
        std::cout << 100*y/IMAGE_HEIGHT << "%\t\r" << std::flush;
    }
}