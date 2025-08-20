#include <pthread.h>
#include <iostream>
#include <cmath>

void* thread_func(void* n){
    //é primo?
    bool primo=true;
    for(int i=2;i<=sqrt(*(long*)n);i++)
        if(*(long*)n %i==0){
            std::cout<<*(long*)n<<" nao é primo\n";
            primo=false;
            break;
        }
    if(primo)
        std::cout<<*(long*)n<<"é primo\n";
    
    pthread_exit(NULL);              //impoe o retorno como um ponteiro pra null
}

int main(){
    pthread_t t[4];
    long valores[4]={1379455703, 1988595883, 1482377683, 24562922401};

    for(int i=0;i<4;i++){
        pthread_create(&t[i], nullptr, thread_func, &valores[i]);  //passando o valor i
    }
    void* value;
    for(int i=0;i<4;i++)
        pthread_join(t[i],&value);  //esperando fim da thread
    
    std::cout<<"fim da main\n";

}

//interpretando o código
// se o processador tem 4 nucleos ou mais, temos um paralelismo real, onde cada thread é
//designada para um nucleo diferente e as operacoes da funcao sao executadas  -> PARALELISMO

// buffer do cout demonstra uma CONCORRENCIA, onde as 4 threads estao competindo para 
//escrever na tela

//observacoes:
// *(long*)n é usado pois passamos um ponteiro void em create
