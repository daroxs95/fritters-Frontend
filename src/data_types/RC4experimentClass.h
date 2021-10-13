#pragma once

#include "common.h"

#include <functional>
#include <cstddef>
#include <cstring>



/** Class of base calculation type of probabilities after KSA in practice, any type of calculation 
 * needed experimentally should be doable using this
 * 
 */
class RC4calcInstanceInPractice
{
    public:
    RC4calcInstanceInPractice();

    RC4calcInstanceInPractice(const char *id,const std::function<std::string()>& getPassword);

    ///id for each calculation
    char id[200];
    ///to activate/deactivate the calculations/use of a instance without destroying it                                                             
    bool isActive = true;
    ///number of experiments executed
    size_t experimentsNumber = 0;
    /**holds number of of times that `second_index`(as byte) ends on `first_index` position 
     * at `S`(state array of RC4) after KSA, after the experiments.
     */ 
    long double realOccurrenceProbability[256][256];    
    /**holds the probability of `second_index`(as byte) ending on `first_index` position 
     * at `S`(state array of RC4) after KSA, after the experiments.
     * ie: `occurrenceProbability[x][y] = realOccurrenceProbability[x][y] / experimentsNumber`
     * is in `floats` because vanilla plotter used only supports `float`.
     */                  
    float occurrenceProbability[256][256];      

    /**
     * @brief Get the Occurrence Probability P[u] = v, just implemented to temporal use from Lua
     * 
     * @param u 
     * @param v 
     * @return float 
     */
    float getOccurrenceProbability(const int &u, const int &v);

    /**
     * @brief Initialices all arrays, structs, etc, to a clean state of class instance
     * 
     * @return int Error code `0` if succeded. 
     */
    int clean();
    void runExperiment(const int & passwordsNumber);

    ///function to know distribution of key(it generates keys with determined distribution)                           
    std::function<std::string()> getPassword;
    ///function to execute on RunExperiment(), does the calculations, fills the arrays, etc
    std::function<int(RC4calcInstanceInPractice&)> fillOcurrencesAfterKSAreturnPRGAstream;




    int outputBytesNumberPRGA = 100;
    /**vector to store probabilities of a secuence of outputs bytes from PRGA, 
     * ie: position 0 is for first byte outputed, position 1 for the second byte and so on   
     */                             
    std::vector<Single256ArrayOutputProb> PRGAoutputsProbabilities;
    std::vector<jArrayStruct> *jArrays4eachPass = nullptr;


    /**vector to store probabilities of a secuence of outputs bytes from PRGA, 
     * but using only data from passwords that generated a state array after KSA with `0` on the position `1`
     * ie: `S[1]=0`
     */ 
    std::vector<Single256ArrayOutputProb> PRGAoutputsProbabilitiesS1eq0;
    /**vector to store probabilities of a secuence of outputs bytes from PRGA, 
     * but using only data from passwords that generated a state array after KSA with the position `1` diferent than `0`
     * ie: `S[1]!=0`, `S[1] \neq 0`
     */ 
    std::vector<Single256ArrayOutputProb> PRGAoutputsProbabilitiesS1neq0;
};


int emptyFillOcurrencesAfterKSAreturnPRGAstream(RC4calcInstanceInPractice&);
