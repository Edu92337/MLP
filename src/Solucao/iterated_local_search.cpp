#include"iterated_local_search.hpp"
#include"solucao.hpp"
#include"Data.h"
#include"auxiliares.hpp"
#include<random>



ILS::ILS(Data* data_original){
    data = data_original;
}

vector<Insertion_info> ILS::calcular_custo_insercao(Solucao& s, vector<int>&CL){
    vector<Insertion_info>custo_insercao((s.sequencia.size()-1)*CL.size());
    int l = 0;
    for(int a = 0;a<s.sequencia.size()-1;a++){
        int i = s.sequencia[a];
        int j = s.sequencia[a+1];
        for(int k : CL){
            custo_insercao[l].custo = data->getDistance(i,k) + data->getDistance(j,k) - data->getDistance(i,j);
            custo_insercao[l].no_inserido = k;
            custo_insercao[l].aresta_removida = a;
            l++;
        }
    }
    return custo_insercao;
}

Solucao ILS::construcao(){
    Solucao s(data); // Recebe o mesmo ponteiro de data que a solução
    //std::cout << "Construindo uma solução inicial..." << std::endl;
    s.sequencia = tres_nos_aleatorios(s);
    vector<int>CL = nos_restantes(&s);
    while(!CL.empty()){
        vector<Insertion_info> custo_insercao = calcular_custo_insercao(s,CL);
        ordena(custo_insercao);
        alfa = (double) rand()/RAND_MAX;
        int selecionado = rand()%((int)ceil(alfa * custo_insercao.size()));
        s.add_no(custo_insercao[selecionado].no_inserido);
        CL = nos_restantes(&s);
    }
    s.add_no(s.sequencia[0]); // fecha o ciclo
    s.calcula_valor_obj(); // Atualiza o custo dessa solução criada
    //std::cout<<"Solução inicial construída com custo :"<<s.valor_obj<<std::endl;
    return s;
}


bool ILS::best_improvement_swap(Solucao* s){
    double best_delta = 0;
    int best_i, best_j; // Melhores candidatos a troca
    for(int i = 1;i<s->sequencia.size()-1;i++){
        // Itera por todos os pares, comparando o custo novo
        // mantendo os Nós adjacentes
        int vi = s->sequencia[i];
        int vi_next = s->sequencia[i+1];
        int vi_prev = s->sequencia[i-1];
        for(int j = i+1;j<s->sequencia.size()-1;j++){
            double delta;
            int vj = s->sequencia[j];
            int vj_next = s->sequencia[j+1];
            int vj_prev = s->sequencia[j-1];
            // O  cálculo muda para Nós adjacentes
            if(j == i+1 || j == i-1){
                delta =-s->dist(vi_prev, vi) - s->dist(vj, vj_next) - s->dist(vi,vi_next)
                        +s->dist(vi_prev, vj) +s->dist(vj, vi) +s->dist(vi, vj_next);
            }
            else{
                delta = -s->dist(vi_prev,vi) -s->dist(vi,vi_next) +s->dist(vi_prev,vj)
                        +s->dist(vj,vi_next) -s->dist(vj_prev,vj) -s->dist(vj,vj_next) + s->dist(vj_prev,vi) 
                        +s->dist(vi,vj_next);
            }

            if(delta < best_delta){
                best_delta = delta;
                best_i = i;
                best_j = j;
            }
        
        }
    }
    // Melhor custo novo é menor que o custo anterior 
    // -> indica melhora na solução
    if(best_delta < 0){
        std::swap(s->sequencia[best_i],s->sequencia[best_j]);
        s->valor_obj += best_delta;
        //s->calcula_valor_obj();
        return true;
    }
    return false; // Não achou na vizinhança um vizinho com custo menor
}

/*
Solução inicial :[1,7,6,4,10,3,9,2,8,5,1]
Solução final   :[1,9,3,10,4,6,7,2,8,5,1]

->[9,3,10,4,6,7] foi invertido
*/
bool ILS::best_improvement_2_opt(Solucao* s){
    double best_delta = 0;
    int best_i, best_j;
    for(int i = 1;i<s->sequencia.size()-1;i++){
        int vi = s->sequencia[i];
        int vi_next = s->sequencia[i+1];
        for(int j = i+1;j<s->sequencia.size()-1;j++){
            if(j == i+1 || j == i-1)continue;
            int vj = s->sequencia[j];
            int vj_next = s->sequencia[j+1];
            double delta = -s->dist(vi,vi_next) - s->dist(vj,vj_next) 
            + s->dist(vi_next,vj_next) + s->dist(vj,vi);

            if(delta < best_delta){
                best_delta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(best_delta < 0){
        // intervalo [i,j+1) -> [i,j]
        std::swap(s->sequencia[best_i + 1],s->sequencia[best_j]);
        reverse(s->sequencia.begin()+best_i + 2,s->sequencia.begin()+best_j); 
        s->valor_obj += best_delta;
        return true;
    }return false;
}


/*
t_bloco = 1
Solução inicial :[1,7,6,4,10,3,9,2,8,5,1]
Solução final   :[1,7,6,10,3,9,2,8,4,5,1]

t_bloco = 2
Solução inicial :[1,7,6,4,10,3,9,2,8,5,1]
Solução final   :[1,7,6,3,9,2,8,4,10,5,1]
*/
bool ILS::best_improvement_or_opt(Solucao* s, int t_bloco){
    double best_delta = 0;
    int best_i,best_j;
    for(int i = 1;i<s->sequencia.size()-t_bloco;i++){
        int vi = s->sequencia[i];
        //Precisa analisar o tamanho do bloco
        int vi_next = s->sequencia[i+t_bloco];
        int vi_prev = s->sequencia[i-1];
        int vi_fim = s->sequencia[i+t_bloco-1];
        
        for(int j = 1;j<s->sequencia.size()-1;j++){
            double delta;
            int vj = s->sequencia[j];
            int vj_next= s->sequencia[j+1];
            if(j == i || j == i-1) continue;
            if(j>=i && j<=i+ t_bloco-1) continue;
            if(t_bloco == 1){
                delta = -s->dist(vi_prev,vi) - s->dist(vi,vi_next) - s->dist(vj,vj_next)
                        + s->dist(vi_prev,vi_next) + s->dist(vj,vi) + s->dist(vi,vj_next);
            }
            else{
                delta = -s->dist(vi_prev,vi) - s->dist(vi_fim,vi_next) - s->dist(vj,vj_next)
                        + s->dist(vi,vj) + s->dist(vi_prev,vi_next) + s->dist(vi_fim,vj_next);
            }
            if(delta < best_delta){
                best_delta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }
    if(best_delta < 0){
        if(t_bloco == 1){
            s->sequencia.insert(s->sequencia.begin()+best_j+1,s->sequencia[best_i]);
            // remove o nó original 
            if(best_i > best_j) s->sequencia.erase(s->sequencia.begin()+best_i+1);
            
            // Se o nó removido estiver antes do nó inserido, o índice do nó removido aumenta em 1
            else s->sequencia.erase(s->sequencia.begin()+best_i);

        }else{
            //intervalo removido -> [i,i+t_bloco)
            std::vector<int>intervalo_removido(s->sequencia.begin()+best_i,s->sequencia.begin()+best_i+t_bloco);
            if(best_i > best_j){
                // coloca o intervalo removido na posição j+1
                s->sequencia.insert(s->sequencia.begin()+best_j+1,intervalo_removido.begin(),intervalo_removido.end());
                // remove o intervalo original
                s->sequencia.erase(s->sequencia.begin()+best_i+t_bloco,s->sequencia.begin()+best_i+t_bloco+t_bloco);
            }else{

                // remove o intervalo original
                s->sequencia.erase(s->sequencia.begin()+best_i,s->sequencia.begin()+best_i+t_bloco);
                // coloca o intervalo removido na posição j+1
                s->sequencia.insert(s->sequencia.begin()+best_j-t_bloco+1,intervalo_removido.begin(),intervalo_removido.end());
            }
        }
        s->valor_obj += best_delta;
        return true;
    }
    return false;
}

void ILS::busca_local(Solucao* s){
    // Vai buscar uma solução melhor na vizinhaça de um dos 
    // 5 métodos apresentados (swap,2-opt,or-opt,reinsertion,or-opt2)
    // Escolhido de forma aleatória
    std::vector<int>NL ={1,2,3,4,5};
    bool improved = false;
    
    while(!NL.empty()){
        int n = rand() % NL.size();
        switch(NL[n]){
            case 1:
                improved = best_improvement_swap(s);
                //std::cout << "Melhorando a solução com custo(swap):"<<s->valor_obj << std::endl;
                break;
            case 2:
                improved = best_improvement_2_opt(s);
                //std::cout << "Melhorando a solução com custo(2_opt) :"<<s->valor_obj << std::endl;
                break;
            case 3:
                improved = best_improvement_or_opt(s,1);
                //std::cout << "Melhorando a solução com custo(or_opt1) :"<<s->valor_obj << std::endl;
                break;
            case 4:
                improved = best_improvement_or_opt(s,2);
                //std::cout << "Melhorando a solução com custo(or_opt2) :"<<s->valor_obj << std::endl;
                break;
            case 5:
                improved = best_improvement_or_opt(s,3);
                //std::cout << "Melhorando a solução com custo(or_opt3) :"<<s->valor_obj << std::endl;
                break;
        }
        
        //Se melhorou,mantém todas para tentar novamente até não ser possível mais melhorar
        if(improved) NL = {1,2,3,4,5}; 
        else NL.erase(NL.begin()+n); // remove essa vizinhança
    }
}


Solucao ILS::perturbacao(Solucao* s){
    //std::cout << "Perturbando a solução com custo :"<<s->valor_obj << std::endl;
    Solucao sf(*s);
    int n = s->data->getDimension();
    int t_max = std::max(1, (int)ceil(s->data->getDimension() / 10.0));
    int t1 = 2 + rand() % t_max;
    int t2 = 2 + rand() % t_max;

    // posição final : p1 + t1 - 1
    int p1 = 1 + rand() % (n - t1 -t2 -1);
    //garantir que p2 não inicie no intervalo de p1
    // posição final não pode sair do vetor
    int p2 = p1 + t1 + rand() % (n - t2 -p1-t1 + 1);

    while(p2 + t2 - 1 >= n) p2 = p1 + t1 + rand() % (n - t2 -p1-t1 + 1);
    
    // Remove ligação do inicio e fim dos blocos 1 e 2
    // Adiciona as novas ligações entre os blocos 1 e 2
    double delta = 0.0;
    if(p2 == p1 + t1){
        // blocos adjacentes: s->sequencia[p1+t1] == s->sequencia[p2]
        delta = - s->dist(s->sequencia[p1-1],s->sequencia[p1]) - s->dist(s->sequencia[p1+t1-1],s->sequencia[p2])
                - s->dist(s->sequencia[p2+t2-1],s->sequencia[p2+t2])
                + s->dist(s->sequencia[p1-1],s->sequencia[p2]) + s->dist(s->sequencia[p2+t2-1],s->sequencia[p1])
                + s->dist(s->sequencia[p1+t1-1],s->sequencia[p2+t2]);
    } else {
        // blocos não adjacentes
        delta = - s->dist(s->sequencia[p1-1],s->sequencia[p1]) - s->dist(s->sequencia[p1+t1-1],s->sequencia[p1+t1])
                - s->dist(s->sequencia[p2-1],s->sequencia[p2]) - s->dist(s->sequencia[p2+t2-1],s->sequencia[p2+t2])
                + s->dist(s->sequencia[p1-1],s->sequencia[p2]) + s->dist(s->sequencia[p2+t2-1],s->sequencia[p1+t1])
                + s->dist(s->sequencia[p2-1],s->sequencia[p1]) + s->dist(s->sequencia[p1+t1-1],s->sequencia[p2+t2]);
    }

    swap_intervalos(&sf,p1,p2,t1,t2);
    
    // Precisa atualizar para calcular com delta
    //sf.calcula_valor_obj();
    sf.valor_obj += delta;
    //std::cout << "valor apos a perturbação solução com custo :"<<sf.valor_obj << std::endl;
    return sf;
}



Solucao ILS::solver(int max_iter, int max_iter_ils){
    Solucao melhor_de_todas(data);
    melhor_de_todas.valor_obj = INFINITY;

    for(int i = 0;i<max_iter;i++){
        Solucao s = construcao();
        Solucao melhor = s;

        int iter_ils = 0;

        while(iter_ils <= max_iter_ils){
            busca_local(&s);
            if(s.valor_obj < melhor.valor_obj){
                melhor = s;
                iter_ils = 0;
            }
            s = perturbacao(&melhor);
            iter_ils++;
        }
        if(melhor.valor_obj < melhor_de_todas.valor_obj){
            melhor_de_todas = melhor;
        }
    }
    return melhor_de_todas;
}