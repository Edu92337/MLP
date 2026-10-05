#include<iostream>
#include<vector>
#include<algorithm>
#include"solucao.hpp"

Solucao::Solucao(Data* data_original){
    valor_obj = 0;
    data = data_original;
}

void Solucao::exibe_solucao(){
    for(int i = 0;i<sequencia.size();i++){
        std::cout<<sequencia[i]<<" -> ";
    } std::cout<<'\n';
}

void Solucao::add_no(double x){
    sequencia.push_back(x);
}

void Solucao::calcula_valor_obj(){
    double t = 0, c = 0;
    int n = sequencia.size() - 1;          
    for(int i = 1; i < n; i++){           
        t += data->getDistance(sequencia[i-1], sequencia[i]);
        c += t;
    }
    valor_obj = c;
}

void Solucao::ordena(vector<int>&CL, int r){
    sort(CL.begin(),CL.end(),
        [&](int a,int b){
            return dist(a,r) < dist(b,r);
        });
}