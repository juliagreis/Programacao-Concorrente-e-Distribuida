/*Uma nova modalidade de corrida foi organizada de modo que os competidores devem concluir V voltas e,
 em cada volta, eles obtém uma pontuação dada pela ordem em que a volta foi concluída. Assim, o primeiro a 
 completar a primeira volta fica com 0; o segundo, com 1; o terceiro com 2; etc. Essa pontuação é dada a cada 
 uma das V voltas e, ao final, a soma das pontuações definirá o valor obtido por cada competidor de modo que 
 uma pontuação baixa é melhor do que uma pontuação alta.

Além disso uma competição é formada por E etapas, onde cada etapa é uma corrida completa (com as V voltas). 
A pontuação final de cada competidor é dada pela soma das pontuações obtidas em cada uma das E etapas da competição.

Implemente um programa em C/C++ para modelar o comportamento de N competidores atendendo os seguintes requisitos:

Cada competidor deve ser implementado por uma thread com relação de 1:1, ou seja, 1 thread não deve processar mais 
que um competidor e 1 único competidor não deve utilizar mais que uma thread.
Um competidor rápido pode dar várias voltas de vantagem em competidores lentos em uma corrida, mas uma nova etapa 
(corrida) só pode começar depois que a anterior foi totalmente concluída.
Para incentivar a concorrência, no trecho que representa uma volta do competidor, você pode utilizar o comando 
this_thread::yield(), que incentiva a troca de contexto entre threads do processo.
A sua implementação não deve usar espera ocupada.
A sua implementação não deve sofrer com problemas gerados por corrida crítica.
Todas as threads devem terminar normalmente, sem erros.
A sua implementação pode ser realizada utilizando a biblioteca pthread.h ou as bibliotecas específicas para C++,
 como a biblioteca thread.*/

#include <iostream>
#include <thread> 
#include <mutex>
#include <barrier>
#include <vector>
#include <map>

std::map<std::thread::id,long long > map_pontuacao_id;
std::map<std::thread::id, long long> thread_para_id;
int cont=0;

std::mutex m;


void correr(int id,int num_voltas,int num_etapas,int num_corredores,std::barrier<> &b){ //meu c++ ta pedindo <> 

    //inicializando maps

    m.lock();

    map_pontuacao_id[std::this_thread::get_id()]=0;
    thread_para_id[std::this_thread::get_id()] = id;

    m.unlock();
    

    for (int etapa = 0; etapa < num_etapas; etapa++){
        for (int volta = 0; volta < num_voltas; volta++){

            // Simula o tempo de corrida
            for (int i = 0; i < 100; i++) { //pra gerar mais concorrencia
                std::this_thread::yield();
            }
            
            // registrar a ordem de chegada dessa volta
            m.lock();

            long long minha_pontuacao=cont;
            cont++;
            map_pontuacao_id[std::this_thread::get_id()]+=minha_pontuacao;

            m.unlock();
            
            //espera todas as threads terminarem a volta
            b.arrive_and_wait();

            m.lock();
            if(cont!=0) cont=0;  //só a primeira que chegar precisa executar
            m.unlock();

            b.arrive_and_wait(); //pra ninguem sair na frente para a proxima etapa e alterar cont simultaneamente

        }
    }
}

int main(){

    
    int num_etapas;
    int num_voltas;
    int num_corredores;
    

    std::cout<<"Digite o numero de corredores: ";
    std::cin>>num_corredores;
    std::cout<<"Digite o número de etapas: ";
    std::cin>>num_etapas;
    std::cout<<"Digite o número de voltas :";
    std::cin>>num_voltas;

    std::barrier b(num_corredores);

    std::vector<std::thread> v_threads;

    for(int i=0;i<num_corredores;i++){
        v_threads.push_back(std::thread(correr,i,num_voltas,num_etapas,num_corredores,std::ref(b)));
    }

    //aguardar termino de todas as etapas
    for(int i=0;i<num_corredores;i++){
        v_threads[i].join();
    }

    //todos os resultados
    std::cout << "\nPontuação final:\n";
    for (std::map<std::thread::id,long long>::iterator it=map_pontuacao_id.begin();it != map_pontuacao_id.end(); ++it) {
        long long id_competidor = thread_para_id[it->first];
        std::cout << "Competidor " <<id_competidor<< " -> "<<it->second<<" pontos\n";
    }

   // verifica quem foi o vencedor
    long long menor=1000000000000;
    std::thread::id id;
    for (std::map<std::thread::id,long long>::iterator it=map_pontuacao_id.begin();it != map_pontuacao_id.end();++it) {
        if (it->second<menor) {
            menor=it->second;
            id=it->first;
        }
    }

    //anunciar vencedor
    std::cout<<"O competidor que venceu foi a thread de número "<<thread_para_id[id]<<" com "<<menor<<" pontos!!!"<<std::endl;
    
}