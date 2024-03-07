#include "Geometry.h"

#include "TracerUtils.h"
#include "Types.h"

#include "json.hpp"
#include <Eigen/Dense>

#include <iostream>


Geometry::Geometry(const nlohmann::json &geometry) 
{
    ka = geometry["ka"];
    kd = geometry["kd"];
    ks = geometry["ks"];
    pc = geometry["pc"];

    ac = {geometry["ac"][0], geometry["ac"][1], geometry["ac"][2]};
    dc = {geometry["dc"][0], geometry["dc"][1], geometry["dc"][2]};
    sc = {geometry["sc"][0], geometry["sc"][1], geometry["sc"][2]};

    // Rectangle specifics
    if (geometry["type"] == "rectangle")
    {
        type = RECTANGLE;

        p1 = Eigen::Vector3f(geometry["p1"][0], geometry["p1"][1], geometry["p1"][2]);
        p2 = Eigen::Vector3f(geometry["p2"][0], geometry["p2"][1], geometry["p2"][2]);
        p3 = Eigen::Vector3f(geometry["p3"][0], geometry["p3"][1], geometry["p3"][2]);
        p4 = Eigen::Vector3f(geometry["p4"][0], geometry["p4"][1], geometry["p4"][2]);

        // Make sure p3 and p1 make the diagonal
        if ((p3-p1).norm() < (p2-p1).norm()) { std::swap(p3, p2); }
        if ((p3-p1).norm() < (p4-p1).norm()) { std::swap(p3, p4); }

    }
    
    // Sphere specifics
    if (geometry["type"] == "sphere")
    {
        type = SPHERE;

        center = Eigen::Vector3f(geometry["centre"][0], geometry["centre"][1], geometry["centre"][2]);
        radius = geometry["radius"];
    }
    
    // Optional

    // Default values
    visible = true;

    //
    if (geometry.contains("comment")) { comment = geometry["comment"]; }
    if (geometry.contains("visible")) { visible = geometry["visible"]; }
    if (geometry.contains("transform"))
    {
        auto json_array = geometry["transform"];
        for (int i = 0; i < 4; ++i) 
            for (int j = 0; j < 4; ++j)
            transform(i, j) = json_array[i][j];

        if (type == RECTANGLE)
        {
            Eigen::Matrix<float, 4, 4> homogeneousPoints(4, 4);
            homogeneousPoints << p1.x(), p1.y(), p1.z(), 1.0f,
                                 p2.x(), p2.y(), p2.z(), 1.0f,
                                 p3.x(), p3.y(), p3.z(), 1.0f,
                                 p4.x(), p4.y(), p4.z(), 1.0f;
            Eigen::Matrix<float, 4, 4> transformedPoints = homogeneousPoints * transform;

            // Perform perspective division (divide by w)
            for (int i = 0; i < transformedPoints.cols(); ++i) {
                transformedPoints.col(i) /= transformedPoints(3, i);
            }

            // Extract the transformed points from homogeneous coordinates
            Eigen::Matrix<float, 3, 4> resultPoints = transformedPoints.topRows(3);

            p1 = resultPoints.col(0);
            p2 = resultPoints.col(1);
            p3 = resultPoints.col(2);
            p4 = resultPoints.col(3); 
        }
        if (type == SPHERE)
        {
            Eigen::Matrix<float, 4, 1> homogeneousPoint;
            homogeneousPoint << center.x(), center.y(), center.z(), 1.0f;
            Eigen::Matrix<float, 4, 1> transformedPoint = transform * homogeneousPoint;
            transformedPoint /= transformedPoint(3);
            center = transformedPoint.head<3>();
        }
    }

    // Normal after transforms
    if (type == RECTANGLE)
    {
        Vec3 v12 = p2 - p1;
        Vec3 v14 = p4 - p1;

        normal = v12.cross(v14).normalized();
    }

}