#pragma once

#include <fritters/RC4.h>
#include <fritters/utils.h>
#include <list>
#include <cmath>

void arrayOccurrences2probabilities(int occurrences[], float probabilities[], int arr_size, int number_of_experiments)
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
    RC4 cipher("password");//just for initialize a cipher instance, not really using that password, well if is in file yes,lol
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
 * @param occurrence_probability 
 * @param number_of_experiments 
 */
void GetProbabilitiesRC4afterKSA(const std::list<std::string> &passwords, float occurrence_probability[256][256],const int number_of_experiments)
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
        arrayOccurrences2probabilities(occurrences[i], occurrence_probability[i], 256, number_of_experiments);
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
 * @param occurrence_probability 
 */
void GetRealProbabilitiesRC4afterKSA(float occurrence_probability[256][256])
{
    for (int i = 0; i < 256; i++)
    {
        for (int ii = 0; ii < 256; ii++)
        {
            occurrence_probability[i][ii] = ProbAfterRC4sKSA(i,ii);
        }
    }
}


/**
 * @brief Get the Probabilities after KSA, based on theoretical formulae by Sarkar and using matrix of transition probabilities 
 * 
 * @param occurrence_probability 
 */
void GetRealProbabilitiesRC4afterKSA_SARKAR(float occurrence_probability[256][256])
{
    for (int i = 0; i < 256; i++)
    {
        for (int ii = 0; ii < 256; ii++)
        {
            occurrence_probability[i][ii] = ProbAfterRC4sKSA_SARKAR(i,ii);
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