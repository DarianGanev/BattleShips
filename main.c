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
        case 3: {
            char filename[256];
            char password[64];
            printf("Enter filename to load (e.g. save1.dat): ");
            scanf("%255s", filename);
            printf("Enter password: ");
            scanf("%63s", password);
            Game loaded_game;
            if (load_game_with_password(&loaded_game, filename, password)) {
                preview_game(&loaded_game);
            } else {
                printf("Failed to load or decrypt the game.\n");
            }
            printf("\nPress Enter to return to main menu...");
            while(getchar() != '\n');
            getchar();
            clear_input_buffer();
            break;
        }
        case 4:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice!\n");
    }
    
    return 0;
}