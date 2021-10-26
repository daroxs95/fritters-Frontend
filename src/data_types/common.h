#pragma once


#include <cstddef>


struct jArrayStruct
{
    float values[256];
    float isOdd[256];//set to float cause ImGui histogram does not support bool

    float getValue(const int & i)
    {
        if (i < 256 ) return values[i];
        else return -1;
    }
    float getIsOdd(const int & i)
    {
        if (i < 256 ) return isOdd[i];
        else return -1;
    }
};

/**
 * @brief Holds the probabilities of a output byte of PRGA to be any of posibles bytes.
 * 
 */
struct Single256ArrayOutputProb
{
    ///number of each byte ocurrence for some K (K = index)
    long double realOcurrences[256];

    ///probability of each byte ocurrence for some K
    float occurrenceProbability[256];    
    ///number of experiments executed
    size_t experimentsNumber = 0;

    float getOccurrenceProbability(const int &u)
    {
        return occurrenceProbability[u];
    }
};



/**
 * @brief Fill the 2-dimensin array with value
 * 
 * @tparam T Type of array
 * @param array array to fill
 * @param value value to fill array with
 * @param xsize size of firs index of array
 * @param ysize size of second index of array
 */
template<typename T>
inline void InitArrayTo(T array[256][256],const T value)
{
    for (size_t i = 0; i < 256; i++)
    {
        for (size_t ii = 0; ii < 256; ii++)
        {
            array[i][ii] = value;
        }
    }
}


/**
 * @brief Fill the 1-dimensin array with value
 * 
 * @tparam T Type of array
 * @param array array to fill
 * @param value value to fill array with
 * @param xsize size of firs index of array
 * @param ysize size of second index of array
 */
template<typename T>
inline void InitArrayTo(T array[256],const T value)
{
    for (size_t i = 0; i < 256; i++)
    {
        for (size_t ii = 0; ii < 256; ii++)
        {
            array[i][ii] = value;
        }
    }
}

template<typename T>
void arrayOccurrences2probabilities(const T occurrences[], float probabilities[],const int arr_size, const int number_of_experiments)
{
    for (size_t i = 0; i < arr_size; i++)
    {
        probabilities[i] = static_cast<float>(occurrences[i]) / number_of_experiments;
    }
}