#include"Data.h"
#include"iterated_local_search.hpp"
#include"solucao.hpp"
#include <chrono>
#include <random>



int main(int arg,char* argv[]){
    if(arg < 2){
        cerr << "Uso: ./a <instancia.tsp>" << endl;
        return 1;
    }
    srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    Data* data = new Data(arg,argv[1]);
    data->read();
    //std::cout << "Instância: " << data->getInstanceName() << std::endl;
    //std::cout << "Dimensão: " << data->getDimension() << std::endl;
    ILS heuristica(data);
    int max_iter = 50;
    int max_iter_ils;
    if(data->getDimension() >= 150){
        max_iter_ils = data->getDimension()/2;
    }else max_iter_ils = data->getDimension();
    //Solucao s = heuristica.construcao();
    std::chrono::duration<double> tempo_medio = std::chrono::duration<double>::zero();
    Solucao s(data);
    for(int i = 0;i<10;i++){
        auto inicio = std::chrono::high_resolution_clock::now();
        s = heuristica.solver(max_iter,max_iter_ils);
        auto fim = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tempo = fim - inicio;
        tempo_medio += tempo;
    }
    
    std::cout << "Tempo: " << tempo_medio.count()/(double)10.0 << " segundos\n";
    std::cout <<"Custo: "<<s.valor_obj<<'\n';


    return 0;
}