#pragma once

#include <fritters/RC4.h>
#include <fritters/utils.h>
#include <list>
#include <cmath>

template<typename T>
void arrayOccurrences2probabilities(const T occurrences[], float probabilities[],const int arr_size, const int number_of_experiments)
{
    for (size_t i = 0; i < arr_size; i++)
    {
        probabilities[i] = static_cast<float>(occurrences[i]) / number_of_experiments;
    }
}

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


/**
 * @brief Fills the `occurrences` with the probabilities
 * 
 * @param passwords List of passwords to calc KSA on
 * @param occurrences 256x256 matrix that holds number of occurrences of second_index in first_index at S_0(first state array of RC4)(ie: occurrences[0][3] is how many times the value 3 end up in position 0 at S_0)
 */
void RC4statsS_0(const std::list<std::string> &passwords, int occurrences[256][256])
{
    RC4 cipher("password");//just for initialize a cipher instance, not really using that password, well if is in list yes,lol
    uint8_t temp_state_array[256];
    
    for(auto password: passwords)
    {
        cipher.KSA(password);
        cipher.getStateArray(temp_state_array);
        for (size_t i = 0; i < 256; i++)
        {
            occurrences[i][temp_state_array[i]]++;
        }
    }
}

/**
 * @brief Get the Probabilities after KSA  
 * 
 * @param passwords 
 * @param occurrenceProbability 
 * @param number_of_experiments 
 */
void GetProbabilitiesRC4afterKSA(const std::list<std::string> &passwords, float occurrenceProbability[256][256],const int number_of_experiments)
{
    int occurrences[256][256];//static arrays are initialized to 0

    for (int i = 0; i < 256; i++)
    {
        for (int ii = 0; ii < 256; ii++)
        {
            occurrences[i][ii] = 0;
        }
    }
    
    RC4statsS_0(passwords,occurrences);
    
    for (int i = 0; i < 256; i++)
    {
        arrayOccurrences2probabilities(occurrences[i], occurrenceProbability[i], 256, number_of_experiments);
    }
}

/**
 * @brief Fills the array `occurrenceProbability` with the number of ocurrences for each value after 
 * the KSA step of RC4 using `password`
 * 
 * @param occurrenceProbability 
 * @param password 
 */
template<typename T>
void FillOcurrencesafterKSA(T occurrenceProbability[256][256],const std::string password)
{
        RC4 cipher(password);
        uint8_t temp_state_array[256];
        cipher.getStateArray(temp_state_array);
        for (size_t i = 0; i < 256; i++)
        {
            occurrenceProbability[i][temp_state_array[i]]++;
        }
}


/**
 * @brief Fills the array `occurrenceProbability` with the number of ocurrences for each value after 
 * the KSA step of RC4 using `password`, and returns string containing the first chars outputted by PRGA 
 * 
 * @tparam T 
 * @param occurrenceProbability 
 * @param password 
 * @param PRGAoutputsNumber size of returned string 
 * @return std::string 
 */
template<typename T>
std::string FillOcurrencesAfterKSAreturnPRGAstream(T occurrenceProbability[256][256],const std::string password, const int PRGAoutputsNumber)
{
        std::string res;
        res.resize(PRGAoutputsNumber);
        for (size_t i = 0; i < res.size(); i++)
        {
            res[i] = (uint8_t) 0;
        }
        
        RC4 cipher(password);
        uint8_t temp_state_array[256];
        cipher.getStateArray(temp_state_array);
        for (size_t i = 0; i < 256; i++)
        {
            occurrenceProbability[i][temp_state_array[i]]++;
        }

        cipher.inplaceCipher(res);
        return res;
}

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
void InitArrayTo(T array[256][256],const T value)
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
 * @brief Calculates the probability of v to end in position u after the end of RC4's KSA 
 * 
 * The implementation is not optimal in performance because is written for clarity. 
 * 
 * @param N 
 * @return float probability
 */
float ProbAfterRC4sKSA(int u, int v, int N=256)//u,v can be uint8_t but for more general approach are bigger
{
    double pok = 1.0 / N;//probability of occurrence of each posible element in key, ie, P(j_i = v)
    double kop = 1 - pok;
    int t;
    double pstuv;//P_t[u]=v

    if(u==0 && v==1)
    {
        t = v+1;
        pstuv = 2*pok*kop;
    }
    else if((u+1) <= v)
    {
        t = v+1;
        pstuv = pok*(pow(kop,v-u) + pow(kop,v) - pok*pow(kop,2*v - u -1));
    }
    else
    {
        return pok*(pow(kop,N-u-1) + pow(kop,v+1) - pow(kop,N-u+v));
    }

    

    return pstuv*pow(kop, N - t) + (1-pstuv)*pok*pow(kop,v)*(1-pow(kop,N - t));
}


/**
 * @brief Calculates the probability of v to end in position u after the end of RC4's KSA, suing
 * Sarkar formulae(matrixes of transition probabilities)
 * 
 * The implementation is not optimal in performance because is written for clarity. 
 * 
 * @param N 
 * @return float probability
 */
float ProbAfterRC4sKSA_SARKAR(int u, int v, int N=256)//u,v can be uint8_t but for more general approach are bigger
{
    double pok = 1.0 / N;//probability of occurrence of each posible element in key, ie, P(j_i = v)
    double kop = 1 - pok;

    if(u < v)//in one paper shows <= but seems te be wrong
    {
        return pok*(pow(kop,N - 1 - u) + pow(kop,v));
    }
    else
    {
        return pok*(pow(kop,N-u-1) + pow(kop,v) - pow(kop,N-u+v - 1));//varies in some papers of Sarkar
    }
}


/**
 * @brief Get the Probabilities after KSA, based on theoretical formulae
 * 
 * @param occurrenceProbability 
 */
void GetRealProbabilitiesRC4afterKSA(float occurrenceProbability[256][256])
{
    for (int i = 0; i < 256; i++)
    {
        for (int ii = 0; ii < 256; ii++)
        {
            occurrenceProbability[i][ii] = ProbAfterRC4sKSA(i,ii);
        }
    }
}


/**
 * @brief Get the Probabilities after KSA, based on theoretical formulae by Sarkar and using matrix of transition probabilities 
 * 
 * @param occurrenceProbability 
 */
void GetRealProbabilitiesRC4afterKSA_SARKAR(float occurrenceProbability[256][256])
{
    for (int i = 0; i < 256; i++)
    {
        for (int ii = 0; ii < 256; ii++)
        {
            occurrenceProbability[i][ii] = ProbAfterRC4sKSA_SARKAR(i,ii);
        }
    }
}

/**
 * @brief Calculate the mean square error in a set of calues(points)
 * 
 * @param observed_values array containing the observed values after realization of phenomenom 
 * @param predicted_values array containing the predicted values of realization of phenomenom
 * @param size size of the arrays(points count)
 * @return double The mean square error
 */
long double calcMSE (float observed_values[], float predicted_values[], size_t size )
{
    long double MSE = 0;
    for (size_t i = 0; i < size; i++)
    {
        MSE += pow(observed_values[i] - predicted_values[i],2);
    }
    
    return MSE/size;
}