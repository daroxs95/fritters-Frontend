#include "RC4experimentClass.h"


RC4calcInstanceInPractice::RC4calcInstanceInPractice(){}

RC4calcInstanceInPractice::RC4calcInstanceInPractice(const char *id, const std::function<std::string()> &getPassword)
{
    strcpy( this->id, id);
    this->getPassword = getPassword;
    fillOcurrencesAfterKSAreturnPRGAstream = emptyFillOcurrencesAfterKSAreturnPRGAstream;
}

float RC4calcInstanceInPractice::getOccurrenceProbability(const int &u, const int &v)
{
    return occurrenceProbability[u][v];
    //return realOccurrenceProbability[u][v];
}

int RC4calcInstanceInPractice::clean()
{    
    InitArrayTo(realOccurrenceProbability,(long double)0);
    PRGAoutputsProbabilities.clear();
    PRGAoutputsProbabilities.resize(outputBytesNumberPRGA);
    PRGAoutputsProbabilitiesS1eq0.clear();
    PRGAoutputsProbabilitiesS1eq0.resize(outputBytesNumberPRGA);
    PRGAoutputsProbabilitiesS1neq0.clear();
    PRGAoutputsProbabilitiesS1neq0.resize(outputBytesNumberPRGA);
    experimentsNumber = 0;
    if(jArrays4eachPass) 
    {
        auto temp = (*jArrays4eachPass).size();
        (*jArrays4eachPass).clear();
        (*jArrays4eachPass).resize(temp);
    }
    for (size_t ii = 0; ii < PRGAoutputsProbabilitiesS1eq0.size(); ii++)
    {
        PRGAoutputsProbabilitiesS1eq0[ii].experimentsNumber    = 0;  
    }
    for (size_t ii = 0; ii < PRGAoutputsProbabilitiesS1neq0.size(); ii++)
    {
        PRGAoutputsProbabilitiesS1neq0[ii].experimentsNumber    = 0;  
    }

    return 0;
}

void RC4calcInstanceInPractice::runExperiment(const int &passwordsNumber)
{
    clean();
    for (size_t i = 0 ; i < passwordsNumber; i++)
    {
        fillOcurrencesAfterKSAreturnPRGAstream(*this);
    }  
    for (int i = 0; i < 256; i++)
    {
        arrayOccurrences2probabilities(
            realOccurrenceProbability[i], 
            occurrenceProbability[i],
            256,
            experimentsNumber
        );
    }
    for( int i = 0 ; i < PRGAoutputsProbabilities.size(); i++)
    {
        arrayOccurrences2probabilities(
            PRGAoutputsProbabilities[i].realOcurrences,
            PRGAoutputsProbabilities[i].occurrenceProbability,
            256,
            PRGAoutputsProbabilities[i].experimentsNumber
        );
        arrayOccurrences2probabilities(
            PRGAoutputsProbabilitiesS1eq0[i].realOcurrences,
            PRGAoutputsProbabilitiesS1eq0[i].occurrenceProbability,
            256,
            PRGAoutputsProbabilitiesS1eq0[i].experimentsNumber
        );
        arrayOccurrences2probabilities(
            PRGAoutputsProbabilitiesS1neq0[i].realOcurrences,
            PRGAoutputsProbabilitiesS1neq0[i].occurrenceProbability,
            256,
            PRGAoutputsProbabilitiesS1neq0[i].experimentsNumber
        );
    }
}


int emptyFillOcurrencesAfterKSAreturnPRGAstream(RC4calcInstanceInPractice& experimentInstance)
{
    return 0;
}