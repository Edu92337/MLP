#pragma once
#include"../Data/Data.h"
typedef struct Subsequencia Subsequencia;
struct Subsequencia{
    Subsequencia() : data(nullptr), T(0), C(0), W(0), primeiro(0), ultimo(0) {}
    Subsequencia(Data* data);
    Data* data;
    double T,C;
    int W;
    int primeiro, ultimo; // Primeiro e ultimo nos da subsequencia
    inline double dist(int i,int j) {return data->getDistance(i,j);};
    inline Subsequencia& Concatenar(const Subsequencia& s1, const Subsequencia& s2){
        double tmp = dist(s1.ultimo, s2.primeiro);
        this->C = s1.C + s2.W * (s1.T + tmp) + s2.C;
        this->T = s1.T + tmp + s2.T;
        this->W = s1.W + s2.W;
        this->primeiro = s1.primeiro;
        this->ultimo = s2.ultimo;
        return *this;
    }
};