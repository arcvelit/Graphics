#ifndef BUFFERUTILS_H
#define BUFFERUTILS_H

#include "Types.h"

#include <vector>

void rt_fillBuffer(Buffer &buffer, RGB color);
void rt_rectangleFill(Buffer &buffer, RGB color, int X_DIM, std::vector<int> x_bounds, std::vector<int> y_bounds);



#endif