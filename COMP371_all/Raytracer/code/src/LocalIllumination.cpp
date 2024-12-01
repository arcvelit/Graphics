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
#include <cmath>


void renderSceneLocal(Buffer& buffer, SceneInfo& scene, OutputInfo& output)
{
    // Fill buffer with background color
    rt_fillBuffer(buffer, output.bkc);

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
    float delta_u =  (IMAGE_HEIGHT * DELTA - DELTA) / 2;
    float delta_s = -(IMAGE_WIDTH  * DELTA - DELTA) / 2;

    // Traveling on xy axis of the viewport
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        Vec3 u_vec = up * (delta_u - y * DELTA);

        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            Vec3 s_vec = side * (delta_s + x * DELTA); 
            Point grid_center = viewport_center + u_vec + s_vec;


            // Intersect with closest geometry, skip if none seen
            Ray viewportRay = Ray(output.center, grid_center - output.center);
            PairedRoot hit = tu_IntersectSceneGeometries(viewportRay, scene);
            if (hit.geometry == nullptr) continue;


            Point p = viewportRay.reach(hit.root);

            Vec3 Nhat = tu_GetNormal(p, hit.geometry);

            // RGB Colors
            Color COLOR = Color::Zero();

            // Compute colors
            for (Light light : scene.lights)
            {
                // For each type of light 

                if (light.type == POINT || light.usecenter)
                {
                    Ray lightRay = Ray(p, light.center - p);

                    PairedRoot eclipse = tu_IntersectSceneGeometries(lightRay, scene);

                    // Geometry eclipses the light
                    if (tu_PointInShadow(light.center, eclipse, hit, p))
                    {
                        continue;
                    }

                    // Diffuse light
                    float diffuseFactor = hit.geometry->kd * std::max(0.0f, lightRay.direction.dot(Nhat));

                    COLOR += diffuseFactor * hit.geometry->dc.cwiseProduct(light.id);

                    // Specular light
                    Vec3 Hhat = (lightRay.direction - viewportRay.direction).normalized();

                    float speculativeFactor = hit.geometry->ks * std::pow(std::max(0.0f, Nhat.dot(Hhat)), hit.geometry->pc);

                    COLOR += speculativeFactor * hit.geometry->sc.cwiseProduct(light.is);

                }
                else if (light.type == AREA)
                {

                    // Center sampling
                    Vec3 x_strata = light.p2 - light.p1;
                    float x_length = x_strata.norm();
                    x_strata.normalize();

                    Vec3 y_strata = light.p4 - light.p1;
                    float y_length = y_strata.norm();
                    y_strata.normalize();

                    float x_square_size = x_length / light.n;
                    float y_square_size = y_length / light.n; 

                    Point bottom_left = light.p1 + x_square_size*x_strata/2 + y_square_size*y_strata/2;

                    // Colours
                    Color cell_COLOR = Color::Zero();

                    for (int i = 0; i < light.n; i++) 
                    {
                        Vec3 horizontal_jump = i*x_square_size*x_strata;
                        for (int j = 0; j < light.n; j++) 
                        {
                            Vec3 vertical_jump = j*y_square_size*y_strata;
                            Point cell_center = bottom_left + horizontal_jump + vertical_jump;

                            Ray lightRay = Ray(p, cell_center - p);

                            PairedRoot eclipse = tu_IntersectSceneGeometries(lightRay, scene);

                            if (tu_PointInShadow(cell_center, eclipse, hit, p))
                            {
                                continue;
                            }

                            // Diffuse light
                            float diffuseFactor = hit.geometry->kd * std::max(0.0f, lightRay.direction.dot(Nhat));

                            cell_COLOR += diffuseFactor * hit.geometry->dc.cwiseProduct(light.id);

                            // Specular light
                            Vec3 Hhat = (lightRay.direction - viewportRay.direction).normalized();

                            float speculativeFactor = hit.geometry->ks * std::pow(std::max(0.0f, Nhat.dot(Hhat)), hit.geometry->pc);

                            cell_COLOR += speculativeFactor * hit.geometry->sc.cwiseProduct(light.is);

                        }
                    }

                    COLOR += cell_COLOR / (light.n*light.n);

                }
            }

            // Ambient light
            COLOR += hit.geometry->ka * hit.geometry->ac.cwiseProduct(output.ai);

            buffer[3 * (x + IMAGE_WIDTH * y) + 0] = std::min(1.0, (double)COLOR[0]);
            buffer[3 * (x + IMAGE_WIDTH * y) + 1] = std::min(1.0, (double)COLOR[1]);
            buffer[3 * (x + IMAGE_WIDTH * y) + 2] = std::min(1.0, (double)COLOR[2]);

        }
        std::cout << 100*y/IMAGE_HEIGHT << "%\t\r" << std::flush;
    }
}