#pragma once
#include<random>
#include<vector>
#include<algorithm>
#include"solucao.hpp"


void ordena(vector<Insertion_info>&custo_insercao){
    sort(custo_insercao.begin(),custo_insercao.end(),
        [](Insertion_info a,Insertion_info b){
            return a.custo < b.custo; // ordem crescente
    });
}

vector<int> tres_nos_aleatorios(Solucao s){
    int n = s.data->getDimension();
    int no1,no2,no3;
    no1 = 1 + rand() % n;
    no2 = 1 + rand() % n;
    while(no2 == no1) no2 = 1 + rand() % n;
    no3 = 1 + rand() % n;
    while(no3 == no2 || no3 == no1) no3 = 1 + rand() % n;
    return {no1,no2,no3};
}

vector<int> nos_restantes(Solucao* s){
    vector<int>restantes;
    for(int i = 1;i<=s->data->getDimension();i++){
        if(count(s->sequencia.begin(),s->sequencia.end(),i) == 0)restantes.push_back(i);
    }return restantes;
}

/*
Vetor original : [1,14,5,4,8,7,13,6,12,3,10,2,11,9,1]
->coloca o bloco1 na nova posição
[1,14,5,4,8,7,13,6,12,5,4,3,10,2,11,9,1]
->remove primeiro bloco:
[1,14,8,7,13,6,12,5,4,3,10,2,11,9,1]
->coloca bloco2:
[1,14,3,10,2,11,5,4,8,7,13,6,12,5,4,3,10,2,11,9,1]
->remove bloco2 original
[1,14,3,10,2,11,5,4,8,7,13,6,12,5,4,9,1]

*/
void swap_intervalos(Solucao* s, int i, int j, int t_bloco1, int t_bloco2){
    
    if(i>j){
        std::swap(i,j);
        std::swap(t_bloco1,t_bloco2);
    }
    vector<int> bloco1(s->sequencia.begin() + i, s->sequencia.begin() + i + t_bloco1);
    vector<int> bloco2(s->sequencia.begin() + j, s->sequencia.begin() + j + t_bloco2);

    // erase do maior índice pro menor
    s->sequencia.erase(s->sequencia.begin() + j, s->sequencia.begin() + j + t_bloco2);
    s->sequencia.erase(s->sequencia.begin() + i, s->sequencia.begin() + i + t_bloco1);

    // insere bloco2 no lugar de bloco1 deslocado por (t_bloco2 - t_bloco1)
    s->sequencia.insert(s->sequencia.begin() + i, bloco2.begin(), bloco2.end());
    s->sequencia.insert(s->sequencia.begin() + j + (t_bloco2 - t_bloco1), bloco1.begin(), bloco1.end());
}