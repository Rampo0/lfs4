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
 
pthread_t tid_search, tid2, tid_run, tid_lullaby, tid_net;
pthread_t tid_ap[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// Declare shop item
int *item_pokeball;
int *item_lullaby;
int *item_net;
int *item_berry;

// Declare my item
int my_pokeball = 20;
int my_lullaby = 0;
int my_net = 0;
int my_berry = 0;
int my_pokedollar = 100;

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

//Declare locking type and shinny pokemon
char lock_pokemon[100];
int lock_type;
int lock_shiny;

//Declare Game Config
int is_running = 0;
// 0 = Normal Mode ; 1 Catch Mode
int game_mode = 0;
int is_searching = 0;
char *my_pokemon[] = { "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" };
int my_type[] = { -1, -1, -1, -1, -1, -1, -1 };
int my_shiny[] = { -1, -1, -1, -1, -1, -1, -1 };
int my_ap[] = { 200, 200, 200, 200, 200, 200, 200 };
char *type_pokemon0[] = { "Bulbasaur", "Charmander", "Squirtle", "Rattata", "Caterpie"};
char *type_pokemon1[] = { "Pikachu", "Eevee", "Jigglypuff", "Snorlax", "Dragonite"};
char *type_pokemon2[] = { "Mew", "Mewtwo", "Moltres", "Zapdos", "Articuno"};
int poke_run = 0;
int status_lullaby = 0;
int status_net = 0;
int is_catch = 0;


void createShareMemory(){

    key_t key_pokeball = 1234;
    shmid_pokeball = shmget(key_pokeball, sizeof(int), IPC_CREAT | 0666);
    item_pokeball = shmat(shmid_pokeball, NULL, 0);

    key_t key_lullaby = 1235;
    shmid_lullaby = shmget(key_lullaby, sizeof(int), IPC_CREAT | 0666);
    item_lullaby = shmat(shmid_lullaby, NULL, 0);

    key_t key_net = 1236;
    shmid_net = shmget(key_net, sizeof(int), IPC_CREAT | 0666);
    item_net = shmat(shmid_net, NULL, 0);

    key_t key_berry = 1237;
    shmid_berry = shmget(key_berry, sizeof(int), IPC_CREAT | 0666);
    item_berry = shmat(shmid_berry, NULL, 0);

    key_t key_type = 1238;
    shmid_type = shmget(key_type, sizeof(int), IPC_CREAT | 0666);
    pokemon_type = shmat(shmid_type, NULL, 0);

    key_t key_shiny = 1239;
    shmid_shiny = shmget(key_shiny, sizeof(int), IPC_CREAT | 0666);
    pokemon_shiny = shmat(shmid_shiny, NULL, 0);

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

void* searchPokemon(void *arg){
    while(is_searching){
        sleep(4);
        int _search = rand()%100;

        if(_search<60){
            is_searching = 0;
            game_mode = 1;
            printf("\nPokemon Liar ditemukan!\n");
        }
    }

    return 0;
}

void releasePokemon(int poke_id){
    pthread_cancel(tid_ap[poke_id]);

    if(my_type[poke_id]==0){
        my_pokedollar += 80;
    } else if(my_type[poke_id]==1){
        my_pokedollar += 100;
    } else {
        my_pokedollar += 200;
    }

    if(my_shiny[poke_id]==1){
        my_pokedollar += 5000;
    }

    my_pokemon[poke_id] = "NULL";
    my_type[poke_id] = -1;
    my_shiny[poke_id] = -1;
    my_ap[poke_id] = 200;
}

void giveBerry(){
    for(int i=0;i<7;i++){
        if(my_ap[i]<90){
            my_ap[i] += 10;
        } else if(my_ap[i]<101){
            my_ap[i] = 100;
        }
    }
}

void showPokedex(){
    int player_choose;
    printf("My Pokemon:\n");
    for(int i=0; i<7;i++){
        if(my_type[i]!=-1){
            printf("%d. %s => %d AP\n",i+1,my_pokemon[i],my_ap[i]);
        }
    }
    printf("Menu:\n1. Release\n2. Give Berry\n3. Back\nChoose: ");
    scanf("%d",&player_choose);
    if(player_choose==1){
        int poke_choose;
        printf("Masukan nomor pokemon: ");
        scanf("%d",&poke_choose);
        releasePokemon(poke_choose-1);
    } else if (player_choose==2){
        giveBerry();
    } else {
        return;
    }
}

void getPokemon(){
    lock_type = *pokemon_type;
    lock_shiny = *pokemon_shiny;

    int _poke = rand()%5;

    if(lock_type == 0){
        strcpy(lock_pokemon,type_pokemon0[_poke]);
    } else if(lock_type == 1){
        strcpy(lock_pokemon,type_pokemon1[_poke]);
    } else {
        strcpy(lock_pokemon,type_pokemon2[_poke]);
    }
}

void* runPokemon(void *arg){
    while(!poke_run){
        sleep(20);
        int _run = rand()%100;
        if(status_lullaby == 1 || status_net == 1){
            printf("\nPokemon mencoba lari tapi item masih aktif!\n");
        } else {
            if(lock_type == 0){
                if(lock_shiny){
                    if(_run<10){
                        poke_run = 1;
                        game_mode = 0;
                        printf("\nPokemon Liar Lari!\n");
                    }
                } else {
                    if(_run<5){
                        poke_run = 1;
                        game_mode = 0;
                        printf("\nPokemon Liar Lari!\n");
                    }
                }
            } else if(lock_type == 1){
                if(lock_shiny){
                    if(_run<15){
                        poke_run = 1;
                        game_mode = 0;
                        printf("\nPokemon Liar Lari!\n");
                    }
                } else {
                    if(_run<10){
                        poke_run = 1;
                        game_mode = 0;
                        printf("\nPokemon Liar Lari!\n");
                    }
                }
            } else {
                if(lock_shiny){
                    if(_run<25){
                        poke_run = 1;
                        game_mode = 0;
                        printf("\nPokemon Liar Lari!\n");
                    }
                } else {
                    if(_run<20){
                        poke_run = 1;
                        game_mode = 0;
                        printf("\nPokemon Liar Lari!\n");
                    }
                }
            }
        }
    }
    return 0;
}

void* lullabyPowder(void *arg){
    sleep(10);
    status_lullaby = 0;
    return 0;
}

void* invincibleNet(void *arg){
    sleep(20);
    status_net = 0;
    return 0;
}

void useItem(int item_id){
    if(item_id == 1){
        status_lullaby = 1;
        pthread_create(&(tid_lullaby), NULL, lullabyPowder, NULL);
    } else if(item_id == 2){
        status_net = 1;
        pthread_create(&(tid_net), NULL, invincibleNet, NULL);
    }
}

void* decreaseAP(void *arg){
    int poke_id = *((int *) arg);
    free(arg);
    while(my_type[poke_id] != -1){
        sleep(10);
        my_ap[poke_id] -= 10;
        if (my_ap[poke_id] <= 0){
            int _run = rand()%100;
            if(_run < 90){
                my_pokemon[poke_id] = "NULL";
                my_type[poke_id] = -1;
                my_shiny[poke_id] = -1;
                my_ap[poke_id] = 200;
                printf("\nPokemon lepas\n");
            } else {
                my_ap[poke_id] = 50;
                printf("\nPokemon ke reset\n");
            }
        }
    }
    return 0;
}
 
int main(void){
    createShareMemory();
    is_running = 1;

    int player_choose;
    // *item_pokeball = 0;

    while(is_running){
        if(game_mode == 0){
            if(is_searching == 0){
                printf("Menu:\n1. Search\n2. Pokedex\n3. Beli\nChoose: ");
                scanf("%d",&player_choose);
                if(player_choose==1){
                    is_searching = 1;
                    pthread_create(&(tid_search), NULL, searchPokemon, NULL);
                } else if (player_choose==2){
                    showPokedex();
                } else if (player_choose==3){

                } else {

                }
            } else {
                printf("Menu:\n1. Cancel\n2. Pokedex\n3. Beli\n4. Go to Battle\nChoose: ");
                scanf("%d",&player_choose);
                if(player_choose==1){
                    pthread_cancel(tid_search);
                    is_searching = 0;
                } else if (player_choose==2){
                    showPokedex();
                } else if (player_choose==3){
                    //Logic beli.. ini ez lah ini nggak perlu dibuat hehe...cuma mainin kurang tambah kurang tambah integer.
                } else {
                    if(game_mode == 0){
                        printf("\nMasih Belum Menemukan Pokemon Liar\n");
                    }

                }
            }
        } else {
            getPokemon();
            printf("\nMenemukan %s type %d shiny %d\n", lock_pokemon, lock_type, lock_shiny);
            pthread_create(&(tid_run), NULL, runPokemon, NULL);
            while(!poke_run){
                printf("Menu:\n1. Tangkap\n2. Item\n3. Lari\nChoose: ");
                scanf("%d",&player_choose);
                if(player_choose==1){
                    int _capture = rand()%100;
                    int _limit;
                    if(my_pokeball>0){
                        my_pokeball -= 1;
                        if(lock_type == 0){
                            _limit = 80;
                        } else if (lock_type == 1){
                            _limit = 70;
                        } else {
                            _limit = 60;
                        }

                        if(lock_shiny == 1){
                            _limit = _limit - 20;
                        }

                        if(status_lullaby == 1){
                            _limit = _limit + 20;
                        }

                        if(_capture < _limit){
                            for(int i=0; i<7;i++){
                                if(my_type[i]==-1){
                                    if(status_lullaby == 1){
                                        pthread_cancel(tid_lullaby);
                                    }
                                    if(status_net == 1){
                                        pthread_cancel(tid_net);
                                    }
                                    my_type[i] = lock_type;
                                    my_shiny[i] = lock_shiny;
                                    my_pokemon[i] = lock_pokemon;
                                    int *arg = malloc(sizeof(*arg));
                                    if ( arg == NULL ) {
                                        fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
                                        exit(EXIT_FAILURE);
                                    }
                                    *arg = i;
                                    pthread_create(&tid_ap[i], NULL, decreaseAP, arg);
                                    game_mode = 0;
                                    poke_run = 1;
                                    break;
                                }
                            }
                        } else {
                            printf("\nPokemon gagal ditangkap. Coba Lagi!\n");
                        }
                    } else {
                        printf("Pokeball habis!\n");
                    }
                } else if (player_choose==2){
                    // logic if if doang ini setelah milih item mana yang digunakan bakal ke useItem()
                } else if (player_choose==3){
                    poke_run = 1;
                    game_mode = 0;
                    pthread_cancel(tid_run);
                    if(status_lullaby == 1){
                        pthread_cancel(tid_lullaby);
                    }
                    if(status_net == 1){
                        pthread_cancel(tid_net);
                    }
                } else {
                    if(game_mode == 0){
                        printf("\nMasih Belum Menemukan Pokemon Liar\n");
                    }

                }
            }
            poke_run = 0;
        }
    }

    return 0;
}