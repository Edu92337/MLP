#include"iterated_local_search.hpp"
#include"solucao.hpp"
#include"../Data/Data.h"
#include"../../utils/auxiliares.hpp"
#include"../Subsequencia/subsequencia.hpp"
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
    Solucao s(data); 
    alfa = (double) (rand() % 26)/100.0;
    s.add_no(1);
    int r = 1;
    vector<int>CL = nos_restantes(&s);
    while(!CL.empty()){
        vector<Insertion_info> custo_insercao = calcular_custo_insercao(s,CL);
        s.ordena(CL,r);
        int limite = std::max(1,(int)ceil(alfa * CL.size())); // nunca deixa o módulo ser 0
        int selecionado = rand()%limite;
        s.add_no(CL[selecionado]);
        r = CL[selecionado];
        CL = nos_restantes(&s);
    }
    s.calcula_valor_obj(); // Atualiza o custo dessa solução criada
    //std::cout<<"Solução inicial construída com custo :"<<s.valor_obj<<std::endl;
    return s;
}


bool ILS::best_improvement_swap(Solucao* s, vector<vector<Subsequencia>>& subseq_matrix){
    int n = s->sequencia.size() - 1;
    double melhor_custo = s->valor_obj;
    int best_i = -1, best_j = -1;

    for(int i = 1; i < n; i++){
        for(int j = i+1; j < n; j++){
            Subsequencia sigma(data);
            if(j == i+1){ // nós adjacentes: não existe "meio"
                Subsequencia s1(data), s2(data);
                s1.Concatenar(subseq_matrix[0][i-1], subseq_matrix[j][j]);
                s2.Concatenar(s1, subseq_matrix[i][i]);
                sigma.Concatenar(s2, subseq_matrix[j+1][n]);
            } else {
                Subsequencia s1(data), s2(data), s3(data);
                s1.Concatenar(subseq_matrix[0][i-1], subseq_matrix[j][j]);
                s2.Concatenar(s1, subseq_matrix[i+1][j-1]);
                s3.Concatenar(s2, subseq_matrix[i][i]);
                sigma.Concatenar(s3, subseq_matrix[j+1][n]);
            }
            if(sigma.C < melhor_custo){
                melhor_custo = sigma.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(best_i == -1) return false;
    std::swap(s->sequencia[best_i], s->sequencia[best_j]);
    s->valor_obj = melhor_custo;
    atualiza_todas_subsequencias(s, subseq_matrix);
    return true;
}
/*
Solução inicial :[1,7,6,4,10,3,9,2,8,5,1]
Solução final   :[1,9,3,10,4,6,7,2,8,5,1]

->[9,3,10,4,6,7] foi invertido
*/
bool ILS::best_improvement_2_opt(Solucao* s, vector<vector<Subsequencia>>& subseq_matrix){
    // A ideia é identificar dois pontos na sequência e inverter a subsequência
    // entre esses pontos, resultando em uma possível melhoria no custo total da solução.
    int n = s->sequencia.size() - 1;
    double melhor_custo = s->valor_obj;
    int best_i = -1, best_j = -1;

    for (int i = 1; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            Subsequencia sigma_1(data), sigma_2(data);
            sigma_1.Concatenar(subseq_matrix[0][i-1], subseq_matrix[i][j]);
            sigma_2.Concatenar(sigma_1, subseq_matrix[j+1][n]);

            if (sigma_2.C < melhor_custo) {
                melhor_custo = sigma_2.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    if (best_i == -1 || best_j == -1) return false;

    std::swap(s->sequencia[best_i], s->sequencia[best_j]);
    reverse(s->sequencia.begin() + best_i + 1, s->sequencia.begin() + best_j);
    s->valor_obj = melhor_custo;
    atualiza_todas_subsequencias(s, subseq_matrix);
    return true;
}

/*
t_bloco = 1
Solução inicial :[1,7,6,4,10,3,9,2,8,5,1]
Solução final   :[1,7,6,10,3,9,2,8,4,5,1]

t_bloco = 2
Solução inicial :[1,7,6,4,10,3,9,2,8,5,1]
Solução final   :[1,7,6,3,9,2,8,4,10,5,1]
*/
bool ILS::best_improvement_or_opt(Solucao* s, int t_bloco, vector<vector<Subsequencia>>& subseq_matrix){
    int n = s->sequencia.size() - 1;
    double melhor_custo = s->valor_obj;
    int best_i = -1, best_j = -1;

    for(int i = 1; i <= n - t_bloco; i++){
        int fim = i + t_bloco - 1;
        for(int j = 0; j < n; j++){
            if(j >= i-1 && j <= fim) continue; 

            Subsequencia sigma(data);
            if(j > fim){
                // bloco vai pra frente: prefixo + [fim+1..j] + bloco + sufixo
                Subsequencia s1(data), s2(data);
                s1.Concatenar(subseq_matrix[0][i-1], subseq_matrix[fim+1][j]);
                s2.Concatenar(s1, subseq_matrix[i][fim]);
                sigma.Concatenar(s2, subseq_matrix[j+1][n]);
            } else {
                // bloco vai pra trás: [0..j] + bloco + [j+1..i-1] + sufixo
                Subsequencia s1(data), s2(data);
                s1.Concatenar(subseq_matrix[0][j], subseq_matrix[i][fim]);
                s2.Concatenar(s1, subseq_matrix[j+1][i-1]);
                sigma.Concatenar(s2, subseq_matrix[fim+1][n]);
            }

            if(sigma.C < melhor_custo){
                melhor_custo = sigma.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(best_i == -1) return false;

    std::vector<int> bloco(s->sequencia.begin()+best_i, s->sequencia.begin()+best_i+t_bloco);
    s->sequencia.erase(s->sequencia.begin()+best_i, s->sequencia.begin()+best_i+t_bloco);
    int pos = (best_j > best_i) ? (best_j - t_bloco + 1) : (best_j + 1);
    s->sequencia.insert(s->sequencia.begin()+pos, bloco.begin(), bloco.end());

    s->valor_obj = melhor_custo;
    atualiza_todas_subsequencias(s, subseq_matrix);
    return true;
}

void ILS::busca_local(Solucao* s,vector<vector<Subsequencia>>& subseq_matrix){
    // Vai buscar uma solução melhor na vizinhaça de um dos 
    // 5 métodos apresentados (swap,2-opt,or-opt,reinsertion,or-opt2)
    // Escolhido de forma aleatória
    std::vector<int>NL ={1,2,3,4,5};
    bool improved = false;
    
    while(!NL.empty()){
        int n = rand() % NL.size();
        switch(NL[n]){
            case 1:
                improved = best_improvement_swap(s,subseq_matrix);
                //std::cout << "Melhorando a solução com custo(swap):"<<s->valor_obj << std::endl;
                break;
            case 2:
                improved = best_improvement_2_opt(s,subseq_matrix);
                //std::cout << "Melhorando a solução com custo(2_opt) :"<<s->valor_obj << std::endl;
                break;
            case 3:
                improved = best_improvement_or_opt(s,1,subseq_matrix);
                //std::cout << "Melhorando a solução com custo(or_opt1) :"<<s->valor_obj << std::endl;
                break;
            case 4:
                improved = best_improvement_or_opt(s,2,subseq_matrix);
                //std::cout << "Melhorando a solução com custo(or_opt2) :"<<s->valor_obj << std::endl;
                break;
            case 5:
                improved = best_improvement_or_opt(s,3,subseq_matrix);
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

    int t1 = 0, t2 = 0, p1 = 0, p2 = 0;
    bool valido = false;
    const int MAX_TENTATIVAS = 1000;
    for(int tentativa = 0; tentativa < MAX_TENTATIVAS && !valido; tentativa++){
        t1 = 2 + rand() % t_max;
        t2 = 2 + rand() % t_max;

        int faixa_p1 = n - t1 - t2 - 1;
        if(faixa_p1 <= 0) continue; //instância pequena demais para esses t1/t2,

        // posição final : p1 + t1 - 1
        p1 = 1 + rand() % faixa_p1;

        int faixa_p2 = n - t2 - p1 - t1 + 1;
        if(faixa_p2 <= 0) continue;

        // posição final não pode sair do vetor
        p2 = p1 + t1 + rand() % faixa_p2;
        if(p2 + t2 - 1 >= n) continue;

        valido = true;
    }

    if(!valido){
        return sf;
    }

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
        vector<vector<Subsequencia>> subseq_matrix(data->getDimension()+1, vector<Subsequencia>(data->getDimension()+1));
        atualiza_todas_subsequencias(&s,subseq_matrix);
        int iter_ils = 0;

        while(iter_ils <= max_iter_ils){
            busca_local(&s,subseq_matrix);
            if(s.valor_obj < melhor.valor_obj){
                melhor = s;
                iter_ils = 0;
            }
            s = perturbacao(&melhor);
            atualiza_todas_subsequencias(&s, subseq_matrix);
            iter_ils++;
        }
        if(melhor.valor_obj < melhor_de_todas.valor_obj){
            melhor_de_todas = melhor;
        }
    }
    return melhor_de_todas;
}