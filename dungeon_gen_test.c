#include "user.h"
#include "dungeon_gen.h"

#include <stdio.h>

int seed = 100;
uchar field[FIELD_SZ][FIELD_SZ] = {{ID_WALL}};

int main(){
    int i, j;

    for(i=0; i<FIELD_SZ; i++){
        for(j=0; j<FIELD_SZ; j++){
            field[i][j] = ID_WALL;
        }
    }

    genDungeon(field);

    printf("  ");
    for(i=0; i<FIELD_SZ; i++){
        printf("%x", i);
    }
    printf("\n");
    for(i=0; i<FIELD_SZ; i++){
        printf("%x ", i);
        for(j=0; j<FIELD_SZ; j++){
            printf("%d", field[i][j]);
        }
        printf("\n");
    }

}
