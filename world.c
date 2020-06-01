/*
*
* Create By LERUfic for Soal Shift Modul 3
* Thread dan IPC
* 
*/
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h> 
 
pthread_t tid1, tid2;

// Declare shop item
int *item_pokeball;
int *item_lullaby;
int *item_net;
int *item_berry;

//Declare pokemon
int *pokemon_type;
int *pokemon_shiny;

//Declare share memory
int shmid_pokeball;
int shmid_lullaby;
int shmid_net;
int shmid_berry;
int shmid_type;
int shmid_shiny;

//Declare Game Config
int is_running = 0;
int store_max = 200;

void createShareMemory(){

    key_t key_pokeball = 1234;
    shmid_pokeball = shmget(key_pokeball, sizeof(int), IPC_CREAT | 0666);
    item_pokeball = shmat(shmid_pokeball, NULL, 0);
    *item_pokeball = 100;

    key_t key_lullaby = 1235;
    shmid_lullaby = shmget(key_lullaby, sizeof(int), IPC_CREAT | 0666);
    item_lullaby = shmat(shmid_lullaby, NULL, 0);
    *item_lullaby = 100;

    key_t key_net = 1236;
    shmid_net = shmget(key_net, sizeof(int), IPC_CREAT | 0666);
    item_net = shmat(shmid_net, NULL, 0);
    *item_net = 100;

    key_t key_berry = 1237;
    shmid_berry = shmget(key_berry, sizeof(int), IPC_CREAT | 0666);
    item_berry = shmat(shmid_berry, NULL, 0);
    *item_berry = 100;

    key_t key_type = 1238;
    shmid_type = shmget(key_type, sizeof(int), IPC_CREAT | 0666);
    pokemon_type = shmat(shmid_type, NULL, 0);
    *pokemon_type = 0;

    key_t key_shiny = 1239;
    shmid_shiny = shmget(key_shiny, sizeof(int), IPC_CREAT | 0666);
    pokemon_shiny = shmat(shmid_shiny, NULL, 0);
    *pokemon_shiny = 0;

}

void closeShareMemory(){
    shmdt(item_pokeball);
    shmctl(shmid_pokeball, IPC_RMID, NULL);

    shmdt(item_lullaby);
    shmctl(shmid_lullaby, IPC_RMID, NULL);

    shmdt(item_net);
    shmctl(shmid_net, IPC_RMID, NULL);

    shmdt(item_berry);
    shmctl(shmid_berry, IPC_RMID, NULL);

    shmdt(pokemon_type);
    shmctl(shmid_type, IPC_RMID, NULL);

    shmdt(pokemon_shiny);
    shmctl(shmid_shiny, IPC_RMID, NULL);

}
 
void* restockItem(void *arg){
    while(is_running){
        sleep(10);
        if(*item_pokeball<=(store_max-10)){
            *item_pokeball += 10;
        } else if(*item_pokeball>(store_max-10) && *item_pokeball<store_max){
            *item_pokeball = store_max;
        }
        
        if(*item_lullaby<=(store_max-10)){
            *item_lullaby += 10;
        } else if(*item_lullaby>(store_max-10) && *item_lullaby<store_max){
            *item_lullaby = store_max;
        }
        
        if(*item_net<=(store_max-10)){
            *item_net += 10;
        } else if(*item_net>(store_max-10) && *item_net<store_max){
            *item_net = store_max;
        }
        
        if(*item_berry<=(store_max-10)){
            *item_berry += 10;
        } else if(*item_berry>(store_max-10) && *item_berry<store_max){
            *item_berry = store_max;
        }
    }

    return 0;
}

void* randomPokemon(void *arg){
    while(is_running){
        int _type = rand()%100;
        int _shiny = rand()%8000;
        
        //Pokemon type
        if(_type>=20){
            //Normal
            *pokemon_type = 0;
        } else if(_type>=5){
            //Rare
            *pokemon_type = 1;
        } else{
            //Legendary
            *pokemon_type = 2;
        }

        //Pokemon shiny
        if(_shiny == 24){
            *pokemon_shiny = 1;
        } else {
            *pokemon_shiny = 0;
        }
        sleep(1);
        // printf("%d dan %d\n",*pokemon_type,*pokemon_shiny);
    }

    return 0;
}

void shutdownGame(){
    pid_t child_id;
    child_id = fork();
    if(child_id == 0){
        char *argv[] = {"pkill", "trainer", NULL};
        execv("/usr/bin/pkill", argv);
    }
    printf("[*]Trainer Killed Done\n");
    is_running=0;
    printf("[*]Change State to Stop Done\n");
    closeShareMemory();
    printf("[*]Close Shared Memory Done\n");
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("[*]Joining Thread Done\n");
}
 
int main(void){
    createShareMemory();
    is_running = 1;
    pthread_create(&(tid1), NULL, restockItem, NULL);
    pthread_create(&(tid2), NULL, randomPokemon, NULL);

    int player_choose;
    while(is_running){
        printf("Menu:\n1.Shutdown Poke*Zone\nChoose: ");
        scanf("%d",&player_choose);
        if(player_choose==1){
            shutdownGame();
        }
    }

    return 0;
}