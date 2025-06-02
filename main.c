#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "battleships.h"

int main() {
    srand(time(NULL));    printf("=== BATTLE SHIPS ===\n");
    printf("1. Two player game\n");
    printf("2. Single player game\n");
    printf("3. Exit\n");
    printf("Choose option: ");
    
    int choice;
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            start_multiplayer_game();
            break;
        case 2:
            start_singleplayer_game();
            break;        case 3:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice!\n");
    }
    
    return 0;
}
