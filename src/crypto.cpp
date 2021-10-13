#include "crypto.h"





float ProbAfterRC4sKSA(int u, int v, int N)//u,v can be uint8_t but for more general approach are bigger
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


float ProbAfterRC4sKSA2(int u, int v, int N)//u,v can be uint8_t but for more general approach are bigger
{
    double pok = 1.0 / N;//probability of occurrence of each posible element in key, ie, P(j_i = v)
    double kop = 1 - pok;
    int t;
    double pstuv;//P_t[u]=v

    if(u < v)
    {
        t = v+1;
        pstuv = pok*(pow(kop,v-u) + pow(kop,v) );
    }
    else
    {
        return pok*(pow(kop,N-u-1) + pow(kop,v+1) - pow(kop,N-u+v));
    }

    

    return pstuv*pow(kop, N - t) + (1-pstuv)*pok*pow(kop,v)*(1-pow(kop,N - t));
}


float ProbAfterRC4sKSA_SARKAR(int u, int v, int N)//u,v can be uint8_t but for more general approach are bigger
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

float ProbAfterRC4sKSA_SARKAR2(int u, int v, int N)//u,v can be uint8_t but for more general approach are bigger
{
    double pok = 1.0 / N;//probability of occurrence of each posible element in key, ie, P(j_i = v)
    double kop = 1 - pok;

    if(u < v)//in one paper shows <= but seems te be wrong
    {
        return pok*(pow(kop,N - 1 - u) + pow(kop,v));
    }
    else
    {
        return pok*(pow(kop,N-u-1) + pow(kop,v) - pow(kop,N-u+v));//varies in some papers of Sarkar
    }
}


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


void GetRealProbabilitiesRC4afterKSA2(float occurrenceProbability[256][256])
{
    for (int i = 0; i < 256; i++)
    {
        for (int ii = 0; ii < 256; ii++)
        {
            occurrenceProbability[i][ii] = ProbAfterRC4sKSA2(i,ii);
        }
    }
}



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


void GetRealProbabilitiesRC4afterKSA_SARKAR2(float occurrenceProbability[256][256])
{
    for (int i = 0; i < 256; i++)
    {
        for (int ii = 0; ii < 256; ii++)
        {
            occurrenceProbability[i][ii] = ProbAfterRC4sKSA_SARKAR2(i,ii);
        }
    }
}


long double calcMSE (float observed_values[], float predicted_values[], size_t size )
{
    long double MSE = 0;
    for (size_t i = 0; i < size; i++)
    {
        MSE += pow(observed_values[i] - predicted_values[i],2);
    }
    
    return MSE/size;
}


int FillOcurrencesAfterKSAreturnPRGAstream( RC4calcInstanceInPractice& experimentInstance)
{   
    RC4 cipher(experimentInstance.getPassword() );
    uint8_t temp_state_array[256];
    cipher.getStateArray(temp_state_array);
    for (size_t i = 0; i < 256; i++)
    {
        experimentInstance.realOccurrenceProbability[i][temp_state_array[i]]++;
    }
    

    for (size_t ii = 0; ii < experimentInstance.PRGAoutputsProbabilities.size() ; ii++)
    {
        uint8_t retrievedJindexValue = cipher.getJindex();
        uint8_t k = cipher.getKeystreamValueDEBUG();



        if(temp_state_array[1] == 0 && ii < experimentInstance.PRGAoutputsProbabilitiesS1eq0.size() ) 
        {
            experimentInstance.PRGAoutputsProbabilitiesS1eq0[ii].realOcurrences[k]++;
            experimentInstance.PRGAoutputsProbabilitiesS1eq0[ii].experimentsNumber++;
        }
        else if( ii < experimentInstance.PRGAoutputsProbabilitiesS1neq0.size() )
        {
            experimentInstance.PRGAoutputsProbabilitiesS1neq0[ii].realOcurrences[k]++;
            experimentInstance.PRGAoutputsProbabilitiesS1neq0[ii].experimentsNumber++;
        }
        experimentInstance.PRGAoutputsProbabilities[ii].realOcurrences[k]++;
        experimentInstance.PRGAoutputsProbabilities[ii].experimentsNumber++;

        //check vs the size of arrays inside jArrayStruct 
        if (experimentInstance.jArrays4eachPass 
            &&
            ii < 256
            &&
            (*experimentInstance.jArrays4eachPass).size() > experimentInstance.experimentsNumber
        )
        {
            (*experimentInstance.jArrays4eachPass)[experimentInstance.experimentsNumber].values[ii] = retrievedJindexValue;
            (*experimentInstance.jArrays4eachPass)[experimentInstance.experimentsNumber].isOdd[ii] = (retrievedJindexValue % 2 != 0)? true : false;
        }
    }

    return 0;
}



int FillOcurrencesAfterKSAreturnPRGAstreamSSX( RC4calcInstanceInPractice& experimentInstance)
{   
    RC4 cipher(experimentInstance.getPassword() );
    uint8_t temp_state_array[256];
    cipher.getStateArray(temp_state_array);
    for (size_t i = 0; i < 256; i++)
    {
        experimentInstance.realOccurrenceProbability[i][ temp_state_array[temp_state_array[i] ] ]++;
    }
    

    for (size_t ii = 0; ii < experimentInstance.PRGAoutputsProbabilities.size() ; ii++)
    {
        uint8_t retrievedJindexValue = cipher.getJindex();
        uint8_t k = cipher.getKeystreamValueDEBUG();



        if(temp_state_array[1] == 0 && ii < experimentInstance.PRGAoutputsProbabilitiesS1eq0.size() ) 
        {
            experimentInstance.PRGAoutputsProbabilitiesS1eq0[ii].realOcurrences[k]++;
            experimentInstance.PRGAoutputsProbabilitiesS1eq0[ii].experimentsNumber++;
        }
        else if( ii < experimentInstance.PRGAoutputsProbabilitiesS1neq0.size() )
        {
            experimentInstance.PRGAoutputsProbabilitiesS1neq0[ii].realOcurrences[k]++;
            experimentInstance.PRGAoutputsProbabilitiesS1neq0[ii].experimentsNumber++;
        }
        experimentInstance.PRGAoutputsProbabilities[ii].realOcurrences[k]++;
        experimentInstance.PRGAoutputsProbabilities[ii].experimentsNumber++;

        //check vs the size of arrays inside jArrayStruct 
        if (experimentInstance.jArrays4eachPass 
            &&
            ii < 256
            &&
            (*experimentInstance.jArrays4eachPass).size() > experimentInstance.experimentsNumber
        )
        {
            (*experimentInstance.jArrays4eachPass)[experimentInstance.experimentsNumber].values[ii] = retrievedJindexValue;
            (*experimentInstance.jArrays4eachPass)[experimentInstance.experimentsNumber].isOdd[ii] = (retrievedJindexValue % 2 != 0)? true : false;
        }
    }

    return 0;
}
