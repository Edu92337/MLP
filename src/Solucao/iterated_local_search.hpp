#pragma once
#include"solucao.hpp"

typedef struct  Insertion_info Insertion_info;
struct Insertion_info{
    int no_inserido;
    int aresta_removida;
    double custo;
};

typedef struct ILS ILS;
struct ILS {
    ILS(Data* data_original);
    Data* data;
    //int arg1;
    //char** arg2;
    double alfa;
    Solucao construcao();
    vector<Insertion_info> calcular_custo_insercao(Solucao& s, vector<int>&CL);
    bool best_improvement_swap(Solucao* s);
    bool best_improvement_2_opt(Solucao* s);
    bool best_improvement_or_opt(Solucao* s,int i);
    void busca_local(Solucao* s);
    Solucao perturbacao(Solucao* s);
    Solucao solver(int max_iter, int max_iter_ils);
};

