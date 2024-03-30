#include "GUIUtils.h"

#include "Types.h"

#include <iostream>

void guiutils_TellAboutModel(Model* model)
{
    std::string model_name = model->name == "" ? "???" : model->name;
    std::string model_desc = model->description == "" ? "..." : model->description;

    std::cout << model_name << "\n";
    std::cout << model_desc << "\n";
    std::cout << std::endl;
}