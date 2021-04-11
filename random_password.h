#pragma once

#include<string>
#include<time.h>
#include <random>


/**
 * @brief Get a Random String object, you need to `srand()` by yourself for changing seed
 * 
 * @param length length of the returned string
 * @return std::string the random string
 */
std::string getRandomString(int length)
{
    std::string res;
    res.resize(length);

    for (size_t i = 0; i < length; i++)
    {
        res[i] = rand()%256;
    }
    
    return res;
}