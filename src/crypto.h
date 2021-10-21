#pragma once

#include <fritters/RC4.h>
#include <fritters/utils.h>

#include <vector>
#include <list>
#include <cmath>

#include "data_types/common.h"
#include "data_types/RC4experimentClass.h"




int FillOcurrencesAfterKSAreturnPRGAstream( RC4calcInstanceInPractice& experimentInstance);
int FillOcurrencesAfterKSAreturnPRGAstreamSSX( RC4calcInstanceInPractice& experimentInstance);
int FillOcurrencesAfterKSAreturnPRGAstreamSSXS1neq0( RC4calcInstanceInPractice& experimentInstance);
int FillOcurrencesAfterKSAreturnPRGAstreamS1neq0( RC4calcInstanceInPractice& experimentInstance);



template<typename T>
T get_max(const T array[], int array_size)
{
    T max = array[0];
    for (size_t i = 1; i < array_size; i++)
    {
        if(array[i] > max) max = array[i];
    }
    
    return max;
}


/**@file
 * @brief Fills the array `occurrenceProbability` with the number of ocurrences for each value after 
 * the KSA step of RC4 using `password`
 * 
 * @param occurrenceProbability 
 * @param password 
 */
template<typename T>
void FillOcurrencesAfterKSA(T occurrenceProbability[256][256],const std::string password)
{
    RC4 cipher(password);
    uint8_t temp_state_array[256];
    cipher.getStateArray(temp_state_array);
    for (size_t i = 0; i < 256; i++)
    {
        occurrenceProbability[i][temp_state_array[i]]++;
    }
}

///Fills with the occurrences of S[S[u]]=v]
template<typename T>
void FillOcurrencesSxAfterKSA(T occurrenceProbability[256][256],const std::string password)
{
    RC4 cipher(password);
    uint8_t temp_state_array[256];
    cipher.getStateArray(temp_state_array);
    for (size_t i = 0; i < 256; i++)
    {
        occurrenceProbability[i][temp_state_array[temp_state_array[i]]]++;
    }
}


/**
 * @brief Calculates the probability of v to end in position u after the end of RC4's KSA 
 * 
 * The implementation is not optimal in performance because is written for clarity. 
 * 
 * @param N 
 * @return float probability
 */
float ProbAfterRC4sKSA(int u, int v, int N=256);//u,v can be uint8_t but for more general approach are bigger


/**
 * @brief Calculates the probability of v to end in position u after the end of RC4's KSA 
 * 
 * The implementation is not optimal in performance because is written for clarity. 
 * 
 * @param N 
 * @return float probability
 */
float ProbAfterRC4sKSA2(int u, int v, int N=256);//u,v can be uint8_t but for more general approach are bigger


/**
 * @brief Calculates the probability of v to end in position u after the end of RC4's KSA, suing
 * Sarkar formulae(matrixes of transition probabilities)
 * 
 * The implementation is not optimal in performance because is written for clarity. 
 * 
 * @param N 
 * @return float probability
 */
float ProbAfterRC4sKSA_SARKAR(int u, int v, int N=256);//u,v can be uint8_t but for more general approach are bigger


float ProbAfterRC4sKSA_SARKAR2(int u, int v, int N=256);//u,v can be uint8_t but for more general approach are bigger


/**
 * @brief Get the Probabilities after KSA, based on theoretical formulae
 * 
 * @param occurrenceProbability 
 */
void GetRealProbabilitiesRC4afterKSA(float occurrenceProbability[256][256]);


/**
 * @brief Get the Probabilities after KSA, based on theoretical formulae
 * 
 * @param occurrenceProbability 
 */
void GetRealProbabilitiesRC4afterKSA2(float occurrenceProbability[256][256]);


/**
 * @brief Get the Probabilities after KSA, based on theoretical formulae by Sarkar and using matrix of transition probabilities 
 * 
 * @param occurrenceProbability 
 */
void GetRealProbabilitiesRC4afterKSA_SARKAR(float occurrenceProbability[256][256]);


/**
 * @brief Get the Probabilities after KSA, based on theoretical formulae by Sarkar and using matrix of transition probabilities 
 * 
 * @param occurrenceProbability 
 */
void GetRealProbabilitiesRC4afterKSA_SARKAR2(float occurrenceProbability[256][256]);


/**
 * @brief Calculate the mean square error in a set of calues(points)
 * 
 * @param observed_values array containing the observed values after realization of phenomenom 
 * @param predicted_values array containing the predicted values of realization of phenomenom
 * @param size size of the arrays(points count)
 * @return double The mean square error
 */
long double calcMSE (float observed_values[], float predicted_values[], size_t size );