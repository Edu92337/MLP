#pragma once
#include"Data.h"
#include<vector>

typedef struct Solucao Solucao;

struct Solucao {
    Solucao(Data *data_original);
    Data* data; // ponteiro para o objeto Data que vai ser passado para todas as soluções
    std::vector<int>sequencia;
    double valor_obj; // custo da solução 
    void exibe_solucao();
    void add_no(double x);
    void calcula_valor_obj();
    //double dist(int i,int j);
    inline double dist(int i,int j) {return data->getDistance(i,j);};
};


