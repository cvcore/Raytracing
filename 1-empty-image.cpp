#include <iostream>
#include <cmath>
#include <fstream>

int main()
{
    std::ofstream outfile("outfile.ppm");

    int height = 512;
    int width = 512;

    outfile << "P6 " << height << " " << width << " " << "255 ";

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            outfile << static_cast<unsigned char>(255)
                << static_cast<unsigned char>(0)
                << static_cast<unsigned char>(0);
        }
    }

    return EXIT_SUCCESS;
}