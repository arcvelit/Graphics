#include "Light.h"

#include "json.hpp"
#include <Eigen/Dense>


Light::Light(const nlohmann::json &light)
{
    id = Color(light["id"][0], light["id"][1], light["id"][2]);
    is = Color(light["is"][0], light["is"][1], light["is"][2]);

    // Point specifics
    if (light["type"] == "point")
    {
        type = POINT;
        center = Eigen::Vector3f(light["centre"][0], light["centre"][1], light["centre"][2]);
    }

    // Area specifics
    if (light["type"] == "area")
    {
        type = AREA;

        p1 = Eigen::Vector3f(light["p1"][0], light["p1"][1], light["p1"][2]);
        p2 = Eigen::Vector3f(light["p2"][0], light["p2"][1], light["p2"][2]);
        p3 = Eigen::Vector3f(light["p3"][0], light["p3"][1], light["p3"][2]);
        p4 = Eigen::Vector3f(light["p4"][0], light["p4"][1], light["p4"][2]);

        // Make sure p3 and p1 make the diagonal
        if ((p3-p1).norm() < (p2-p1).norm()) { std::swap(p3, p2); }
        if ((p3-p1).norm() < (p4-p1).norm()) { std::swap(p3, p4); }

        center = (p1 + p2 + p3 + p4) / 4.0;

    }

    // Optional

    // Default values
    usecenter = false;
    use = true;
    
    //
    if (light.contains("n")) { n = light["n"]; } else { n = 10; }
    if (light.contains("usecenter")) { usecenter = light["usecenter"]; }
    if (light.contains("use")) { use = light["use"]; }
    if (light.contains("transform"))
    {
        auto json_array = light["transform"];
        for (int i = 0; i < 4; ++i) 
            for (int j = 0; j < 4; ++j)
            transform(i, j) = json_array[i][j];

        if (type == AREA)
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

            center = (p1 + p2 + p3 + p4) / 4.0;
        }
        if (type == POINT)
        {
            Eigen::Matrix<float, 4, 1> homogeneousPoint;
            homogeneousPoint << center.x(), center.y(), center.z(), 1.0f;
            Eigen::Matrix<float, 4, 1> transformedPoint = transform * homogeneousPoint;
            transformedPoint /= transformedPoint(3);
            center = transformedPoint.head<3>();
        } 
    }


}