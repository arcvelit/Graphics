#include "BufferUtils.h"
#include "Types.h"


void rt_fillBuffer(Buffer &buffer, Color color)
{
    const int BUFFER_SIZE = buffer.size();
    for (int i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = color[i%3];
}

void rt_rectangleFill(Buffer &buffer, Color color, int X_DIM, std::vector<int> x_bounds, std::vector<int> y_bounds)
{
    for (int i = x_bounds[0]; i <= x_bounds[1]; i++)
        for (int j =  y_bounds[0]; j <= y_bounds[1]; j++)
            for (int k = 0; k < 3; k++)
                buffer[3*(i+X_DIM*j)+k] = color[k];
}
