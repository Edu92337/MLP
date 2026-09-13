#pragma once
#include"Data.h"    
typedef struct Subsequencia Subsequencia;
struct Subsequencia{
    Subsequencia(Data* data);
    Data* data;
    double T,C;
    int W;
    int primeiro, ultimo; // Primeiro e ultimo nos da subsequencia
    inline double dist(int i,int j) {return data->getDistance(i,j);};
    inline Subsequencia Concatenar(Subsequencia& sigma_1, Subsequencia& sigma_2){
        Subsequencia sigma(data);
        double tmp = dist(sigma_1.primeiro,sigma_2.primeiro);
        sigma.W = sigma_1.W + sigma_2.W;
        sigma.T = sigma_1.T + sigma_2.T;
        sigma.C = sigma_1.C + sigma_2.W*(sigma_1.T + tmp) + sigma_2.C;
        sigma.primeiro = sigma_1.primeiro;
        sigma.ultimo = sigma_2.ultimo;
        return sigma;
    }
};