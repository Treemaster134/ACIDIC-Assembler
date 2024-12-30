//standard c++ headers
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <stdfloat>
//raylib headers
#include "raylib.h"


Image img;

uint16_t color565Converter(Color col)
{
    uint16_t newCol;
    uint16_t r = (col.r/255.0f) * 31;
    uint16_t g = (col.g/255.0f) * 63;
    uint16_t b = (col.b/255.0f) * 31;

    newCol = r | g << 5 | b << 11;

    return newCol;
}

void exportImage(std::string name)
{
    

    std::ofstream outputStream;

    outputStream.open(name.c_str(), std::ofstream::out);
    if(outputStream.is_open())
    {

        //outputStream << (int)img.width;

        for(int y = 0; y < img.height; y++)
        {   
            for(int x = 0; x < img.width; x++)
            {   
                uint16_t dataToWrite = color565Converter(GetImageColor(img, x, y));

                if(dataToWrite == 0xFFFF)
                {
                    dataToWrite = 0xFFFE;
                }

                char highBits = (dataToWrite >> 8) & 0xFF;
                char lowBits = dataToWrite & 0xFF;

                outputStream<<highBits;
                outputStream<<lowBits;
                
            }
        }
        
        outputStream.close();
        std::cout << "Finished writing to file: \"" << name.c_str() << "\"";
    }
    else{std::cout<<"Failed to write to file: \""<< name.c_str() <<"\"";}
    
    
}

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        std::cout << "NOT ENOUGH ARGUMENTS";
        return -1;
    }
    else if (argc > 3)
    {
        std::cout << "TOO MANY ARGUMENTS";
        return -1;
    }

    img = LoadImage(argv[1]);

    exportImage(argv[2]);


    return 0;
    
}

