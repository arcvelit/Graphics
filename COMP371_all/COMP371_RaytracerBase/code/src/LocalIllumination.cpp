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
    for (int y = 0; y < output.size[1]; y++)
    {
        Vec3 u_vec = up * (delta_u - y * DELTA);

        for (int x = 0; x < output.size[0]; x++)
        {
            Vec3 s_vec = side * (delta_s + x * DELTA); 
            Point grid_center = viewport_center + u_vec + s_vec;


            // Intersect with closest geometry, skip if none seen
            Ray viewportRay = Ray(output.center, grid_center - output.center);
            PairedRoot hit = tu_IntersectSceneGeometries(viewportRay, scene);
            if (hit.geometry == nullptr) continue;


            Point p = viewportRay.origin + hit.root * viewportRay.direction;

            Vec3 Nhat;
            if (hit.geometry->type == RECTANGLE) Nhat = hit.geometry->normal;
            else if (hit.geometry->type == SPHERE) Nhat = (p - hit.geometry->center).normalized();

            // RGB Colors
            double IR(0), IG(0), IB(0);

            // Compute colors
            for (Light light : scene.lights)
            {
                // For each type of light 

                if (light.type == POINT || light.usecenter)
                {
                    Ray lightRay = Ray(p, light.center - p);

                    PairedRoot eclipse = tu_IntersectSceneGeometries(lightRay, scene);

                    // Geometry eclipses thethe light.
                    // p to light < p to eclipse
                    // |light.center - p| < eclipse.root
                    if ((eclipse.geometry != nullptr && eclipse.geometry != hit.geometry) &&
                        (light.center - p).squaredNorm() > eclipse.root * eclipse.root)
                    {
                        continue;
                    }

                    // Diffuse light
                    float diffuseFactor = hit.geometry->kd * std::max(0.0f, lightRay.direction.dot(Nhat));

                    IR += diffuseFactor * hit.geometry->dc[0] * light.id[0];
                    IG += diffuseFactor * hit.geometry->dc[1] * light.id[1];
                    IB += diffuseFactor * hit.geometry->dc[2] * light.id[2];

                    // Specular light
                    Vec3 Rhat = 2 * (lightRay.direction.dot(Nhat)) * Nhat - lightRay.direction;
                    Vec3 Vhat = -viewportRay.direction;

                    float speculativeFactor = hit.geometry->ks * std::pow(std::max(0.0f, Vhat.dot(Rhat)), hit.geometry->pc);

                    IR += speculativeFactor * hit.geometry->sc[0] * light.is[0];
                    IG += speculativeFactor * hit.geometry->sc[1] * light.is[1];
                    IB += speculativeFactor * hit.geometry->sc[2] * light.is[2];

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
                    float sum_IR(0), sum_IB(0), sum_IG(0);

                    for (int i = 0; i < light.n; i++) 
                    {
                        Vec3 horizontal_jump = i*x_square_size*x_strata;
                        for (int j = 0; j < light.n; j++) 
                        {
                            Vec3 vertical_jump = j*y_square_size*y_strata;
                            Point cell_center = bottom_left + horizontal_jump + vertical_jump;

                            Ray lightRay = Ray(p, cell_center - p);

                            PairedRoot eclipse = tu_IntersectSceneGeometries(lightRay, scene);

                            if ((eclipse.geometry != nullptr && eclipse.geometry != hit.geometry) &&
                                (cell_center - p).squaredNorm() > eclipse.root * eclipse.root)
                            {
                                continue;
                            }

                            // Diffuse light
                            float diffuseFactor = hit.geometry->kd * std::max(0.0f, lightRay.direction.dot(Nhat));

                            sum_IR += diffuseFactor * hit.geometry->dc[0] * light.id[0];
                            sum_IG += diffuseFactor * hit.geometry->dc[1] * light.id[1];
                            sum_IB += diffuseFactor * hit.geometry->dc[2] * light.id[2];

                            // Specular light
                            Vec3 Rhat = 2 * (lightRay.direction.dot(Nhat)) * Nhat - lightRay.direction;
                            Vec3 Vhat = -viewportRay.direction;

                            float speculativeFactor = hit.geometry->ks * std::pow(std::max(0.0f, Vhat.dot(Rhat)), hit.geometry->pc);

                            sum_IR += speculativeFactor * hit.geometry->sc[0] * light.is[0];
                            sum_IG += speculativeFactor * hit.geometry->sc[1] * light.is[1];
                            sum_IB += speculativeFactor * hit.geometry->sc[2] * light.is[2];


                        }
                    }

                    IR += sum_IR / (light.n*light.n);
                    IB += sum_IB / (light.n*light.n);
                    IG += sum_IG / (light.n*light.n);


                }
            }

            // Ambient light
            IR += hit.geometry->ka * hit.geometry->ac[0] * output.ai[0];
            IG += hit.geometry->ka * hit.geometry->ac[1] * output.ai[1];
            IB += hit.geometry->ka * hit.geometry->ac[2] * output.ai[2];

            buffer[3 * (x + IMAGE_WIDTH * y) + 0] = std::min(1.0, IR);
            buffer[3 * (x + IMAGE_WIDTH * y) + 1] = std::min(1.0, IG);
            buffer[3 * (x + IMAGE_WIDTH * y) + 2] = std::min(1.0, IB);

        }
    }
}