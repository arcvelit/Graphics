#include "BufferUtils.h"
#include "Types.h"

const RGB COLOR_BLUE  = { 0, 0, 1 };
const RGB COLOR_GREEN = { 0, 1, 0 };
const RGB COLOR_RED   = { 1, 0, 0 };
const RGB COLOR_BLACK = { 0, 0, 0 };
const RGB COLOR_WHITE = { 1, 1, 1 };


void rt_fillBuffer(Buffer &buffer, RGB color)
{
    const int BUFFER_SIZE = buffer.size();
    for (int i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = color[i%3];
}

void rt_rectangleFill(Buffer &buffer, RGB color, int X_DIM, std::vector<int> x_bounds, std::vector<int> y_bounds)
{
    for (int i = x_bounds[0]; i <= x_bounds[1]; i++)
        for (int j =  y_bounds[0]; j <= y_bounds[1]; j++)
            for (int k = 0; k < 3; k++)
                buffer[3*(i+X_DIM*j)+k] = color[k];
}
