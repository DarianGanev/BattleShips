#include "battleships.h"

int main() {
    srand((unsigned int)time(NULL));
      printf("=== BATTLE SHIPS ===\n");
    printf("1. Two player game\n");
    printf("2. Single player game\n");
    printf("3. Load saved game\n");
    printf("4. Exit\n");
    
    int choice;
    while (1) {
        printf("Choose option: ");
        if (scanf("%d", &choice) == 1) {
            break;
        } else {
            printf("Invalid input! Please enter a number.\n");
            clear_input_buffer();
        }
    }
      switch(choice) {
        case 1:
            start_multiplayer_game();
            break;
        case 2:
            start_singleplayer_game();
            break;
        case 3:
            printf("Load game functionality not yet implemented.\n");
            break;
        case 4:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice!\n");
    }
    
    return 0;
}