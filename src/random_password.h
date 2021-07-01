#pragma once


/**
 * Note that in all source files(at least in this and in RC4analytics.h) i use functions returning std::string
 * of course is not doable a return of a pointer to a local object(like `std::string &function.....`)
 * but returning string by value should not be bad for performance if compiler supports named return value optimization, 
 * aka NRVO, (which is likely). Not so sure of nesting returns of std::strings by value though (ie inside lambda:
    []()->std::string{
                    return getRandomString(32);
                };
 * for reference https://stackoverflow.com/a/6388758/13281961.
*/


#include<string>
#include<time.h>
#include <random>



/**
 * @brief Get a Random String object, you need to `srand()` by yourself for changing seed
 * 
 * @param length length of the returned string
 * @return std::string the random string
 */
std::string getRandomString(const int length)
{
    std::string res;
    res.resize(length);

    for (size_t i = 0; i < length; i++)
    {
        res[i] = rand()%256;
    }
    
    return res;
}


/**
 * @brief Get a Random String using a Custom Distribution, you need to `srand()` by yourself for changing seed
 * 
 * @param length length of the returned string
 * @return std::string the random string
 */
std::string getRandomStringCustomDistribution(const int length)
{
    std::string res;
    res.resize(length);

    res[0] = 1;
    res[1] = 0;

    uint8_t tempChar;
    for (size_t i = 2; i < length; i++)
    {
        tempChar = rand()%256;
        if(i % 2 == 0)
        {
            if ( (tempChar % 2) == 1) res[i] = tempChar;
            else res[i] = (tempChar + 1);                       //not needed module operation because greater par number module 256 is 254, and plus 1 is 255
        }
        else
        {
            if ( (tempChar % 2) == 0) res[i] = tempChar;
            else res[i] = (tempChar + 1)%256;                   //do something here to avoid module operation
        }

    }
    
    return res;
}
