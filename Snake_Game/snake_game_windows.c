// Simple terminal Based snake game for "computer freaks" :) 
// Compile with: gcc snake_enhanced.c -o snake_enhanced.exe
// or with MSVC: cl /EHsc snake_enhanced.c
// This game is only run inside terminal ( In this code , its run only on Windows )

// Do all the above things and Enjoy ! your LIFE ...



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>

#define WIDTH 50
#define HEIGHT 25
#define MAX_SNAKE (WIDTH * HEIGHT)

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef enum { GAME, MENU, SHOP, PAUSED } GameState;

typedef struct {
    int x, y;
} Point;

typedef struct {
    char name[30];
    int color;
    int speed_bonus;      // negative = faster
    int point_multiplier; // 1=normal, 2=double points
    int cost;
    int unlocked;
    char specialty[50];
} SnakeType;

// Global variables
HANDLE hConsole;
SnakeType snakes[10];
int current_snake = 0;
int total_points = 0;
int game_time = 0;
int high_score = 0;

// Color codes
#define COLOR_RESET 7
#define COLOR_SNAKE_HEAD 14  // Yellow
#define COLOR_SNAKE_BODY 10  // Green
#define COLOR_FOOD 12        // Red
#define COLOR_BORDER 11      // Cyan
#define COLOR_TEXT 15        // White
#define COLOR_MENU 13        // Magenta

void set_color(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

void set_cursor_position(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, pos);
}

void hide_cursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &info);
}

void show_cursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &info);
}

void type_text(const char* text, int delay_ms) {
    for (int i = 0; text[i] != '\0'; i++) {
        putchar(text[i]);
        fflush(stdout);
        Sleep(delay_ms);
    }
}

void init_snakes() {
    // Snake 0: Default (unlocked)
    strcpy(snakes[0].name, "Classic Green");
    snakes[0].color = 10;
    snakes[0].speed_bonus = 0;
    snakes[0].point_multiplier = 1;
    snakes[0].cost = 0;
    snakes[0].unlocked = 1;
    strcpy(snakes[0].specialty, "Balanced - Perfect for beginners");

    // Snake 1: Speed Demon
    strcpy(snakes[1].name, "Speed Demon");
    snakes[1].color = 12;
    snakes[1].speed_bonus = -20;
    snakes[1].point_multiplier = 1;
    snakes[1].cost = 100;
    snakes[1].unlocked = 0;
    strcpy(snakes[1].specialty, "30% Faster movement");

    // Snake 2: Golden Snake
    strcpy(snakes[2].name, "Golden Fortune");
    snakes[2].color = 14;
    snakes[2].speed_bonus = 0;
    snakes[2].point_multiplier = 2;
    snakes[2].cost = 150;
    snakes[2].unlocked = 0;
    strcpy(snakes[2].specialty, "2x Points per food");

    // Snake 3: Azure Glider
    strcpy(snakes[3].name, "Azure Glider");
    snakes[3].color = 9;
    snakes[3].speed_bonus = -10;
    snakes[3].point_multiplier = 1;
    snakes[3].cost = 80;
    snakes[3].unlocked = 0;
    strcpy(snakes[3].specialty, "15% Faster, smooth control");

    // Snake 4: Ruby Collector
    strcpy(snakes[4].name, "Ruby Collector");
    snakes[4].color = 13;
    snakes[4].speed_bonus = 10;
    snakes[4].point_multiplier = 3;
    snakes[4].cost = 250;
    snakes[4].unlocked = 0;
    strcpy(snakes[4].specialty, "3x Points but 10% slower");

    // Snake 5: Emerald Swift
    strcpy(snakes[5].name, "Emerald Swift");
    snakes[5].color = 2;
    snakes[5].speed_bonus = -15;
    snakes[5].point_multiplier = 1;
    snakes[5].cost = 120;
    snakes[5].unlocked = 0;
    strcpy(snakes[5].specialty, "20% Faster speed boost");

    // Snake 6: Platinum Pro
    strcpy(snakes[6].name, "Platinum Pro");
    snakes[6].color = 7;
    snakes[6].speed_bonus = -10;
    snakes[6].point_multiplier = 2;
    snakes[6].cost = 300;
    snakes[6].unlocked = 0;
    strcpy(snakes[6].specialty, "2x Points + 15% faster");

    // Snake 7: Shadow Runner
    strcpy(snakes[7].name, "Shadow Runner");
    snakes[7].color = 8;
    snakes[7].speed_bonus = -25;
    snakes[7].point_multiplier = 1;
    snakes[7].cost = 180;
    snakes[7].unlocked = 0;
    strcpy(snakes[7].specialty, "Ultra fast - Expert mode");

    // Snake 8: Rainbow Master
    strcpy(snakes[8].name, "Rainbow Master");
    snakes[8].color = 11;
    snakes[8].speed_bonus = 0;
    snakes[8].point_multiplier = 4;
    snakes[8].cost = 500;
    snakes[8].unlocked = 0;
    strcpy(snakes[8].specialty, "4x Points - Ultimate collector");

    // Snake 9: Cosmic Legend
    strcpy(snakes[9].name, "Cosmic Legend");
    snakes[9].color = 5;
    snakes[9].speed_bonus = -15;
    snakes[9].point_multiplier = 3;
    snakes[9].cost = 600;
    snakes[9].unlocked = 0;
    strcpy(snakes[9].specialty, "3x Points + 20% faster - LEGENDARY");
}

void draw_border() {
    set_color(COLOR_BORDER);
    // Top border
    set_cursor_position(0, 0);
    printf("%c", 201); // ╔
    for (int x = 1; x < WIDTH - 1; x++) printf("%c", 205); // ═
    printf("%c", 187); // ╗

    // Side borders
    for (int y = 1; y < HEIGHT - 1; y++) {
        set_cursor_position(0, y);
        printf("%c", 186); // ║
        set_cursor_position(WIDTH - 1, y);
        printf("%c", 186); // ║
    }

    // Bottom border
    set_cursor_position(0, HEIGHT - 1);
    printf("%c", 200); // ╚
    for (int x = 1; x < WIDTH - 1; x++) printf("%c", 205); // ═
    printf("%c", 188); // ╝
    set_color(COLOR_RESET);
}

void draw_scoreboard(int score, int time_sec) {
    set_color(COLOR_TEXT);
    set_cursor_position(WIDTH + 3, 1);
    printf("╔════════════════════════════╗");
    set_cursor_position(WIDTH + 3, 2);
    printf("║     SNAKE GAME 2025        ║");
    set_cursor_position(WIDTH + 3, 3);
    printf("╠════════════════════════════╣");
    
    set_cursor_position(WIDTH + 3, 4);
    set_color(14);
    printf("║ Score: %-19d║", score);
    
    set_cursor_position(WIDTH + 3, 5);
    set_color(11);
    printf("║ Points: %-18d║", total_points);
    
    set_cursor_position(WIDTH + 3, 6);
    set_color(13);
    printf("║ High Score: %-14d║", high_score);
    
    set_cursor_position(WIDTH + 3, 7);
    set_color(10);
    printf("║ Time: %02d:%02d               ║", time_sec / 60, time_sec % 60);
    
    set_cursor_position(WIDTH + 3, 8);
    set_color(COLOR_TEXT);
    printf("╠════════════════════════════╣");
    
    set_cursor_position(WIDTH + 3, 9);
    set_color(snakes[current_snake].color);
    printf("║ Snake: %-19s║", snakes[current_snake].name);
    
    set_cursor_position(WIDTH + 3, 10);
    set_color(COLOR_TEXT);
    printf("╠════════════════════════════╣");
    
    set_cursor_position(WIDTH + 3, 11);
    printf("║ Controls:                  ║");
    set_cursor_position(WIDTH + 3, 12);
    printf("║ W/A/S/D - Move             ║");
    set_cursor_position(WIDTH + 3, 13);
    printf("║ P - Pause                  ║");
    set_cursor_position(WIDTH + 3, 14);
    printf("║ ESC - Menu                 ║");
    set_cursor_position(WIDTH + 3, 15);
    printf("╚════════════════════════════╝");
    
    set_color(COLOR_RESET);
}

void show_intro() {
    system("cls");
    
    // Attention message
    set_color(12); // Red
    set_cursor_position(15, 6);
    type_text("╔═════════════════════════════════════════════════╗", 8);
    set_cursor_position(15, 7);
    type_text("║                 ⚠ ATTENTION ⚠                  ║", 8);
    set_cursor_position(15, 8);
    type_text("╚═════════════════════════════════════════════════╝", 8);
    
    set_color(14); // Yellow
    set_cursor_position(12, 10);
    type_text("For the best gaming experience and to avoid bugs:", 25);
    
    set_color(11); // Cyan
    set_cursor_position(18, 12);
    type_text("Please MAXIMIZE your terminal window to", 25);
    set_cursor_position(22, 13);
    type_text("FULL SCREEN before continuing!", 25);
    
    set_color(10); // Green
    set_cursor_position(20, 15);
    type_text("Press F11 or maximize the window now", 30);
    
    set_color(8); // Gray
    set_cursor_position(25, 17);
    type_text("(Press any key when ready...)", 35);
    
    set_color(COLOR_RESET);
    _getch();
    
    // Clear and show main intro
    system("cls");
    set_color(14);
    set_cursor_position(20, 8);
    type_text("╔════════════════════════════════════════╗", 10);
    set_cursor_position(20, 9);
    type_text("║                                        ║", 10);
    set_cursor_position(20, 10);
    type_text("║        ULTIMATE SNAKE GAME 2025        ║", 10);
    set_cursor_position(20, 11);
    type_text("║                                        ║", 10);
    set_cursor_position(20, 12);
    type_text("╚════════════════════════════════════════╝", 10);
    
    set_color(13);
    set_cursor_position(15, 15);
    type_text("This game was created by r0se4U with love ♥", 30);
    
    set_color(11);
    set_cursor_position(25, 18);
    type_text("Press any key to continue...", 40);
    
    set_color(COLOR_RESET);
    _getch();
}

int show_menu() {
    system("cls");
    set_color(COLOR_MENU);
    set_cursor_position(30, 5);
    printf("╔════════════════════════╗");
    set_cursor_position(30, 6);
    printf("║      GAME MENU         ║");
    set_cursor_position(30, 7);
    printf("╚════════════════════════╝");
    
    set_color(COLOR_TEXT);
    set_cursor_position(30, 10);
    printf("1. Resume Game");
    set_cursor_position(30, 12);
    printf("2. New Game");
    set_cursor_position(30, 14);
    printf("3. Shop");
    set_cursor_position(30, 16);
    printf("4. Quit Game");
    
    set_color(11);
    set_cursor_position(30, 19);
    printf("Select (1-4): ");
    set_color(COLOR_RESET);
    
    while (1) {
        if (_kbhit()) {
            char c = _getch();
            if (c >= '1' && c <= '4') return c - '0';
        }
        Sleep(50);
    }
}

void show_shop() {
    system("cls");
    set_color(COLOR_MENU);
    set_cursor_position(25, 1);
    printf("╔═══════════════════════════════════════════════╗");
    set_cursor_position(25, 2);
    printf("║              SNAKE SHOP                       ║");
    set_cursor_position(25, 3);
    printf("╠═══════════════════════════════════════════════╣");
    set_cursor_position(25, 4);
    set_color(14);
    printf("║  Your Points: %-31d║", total_points);
    set_color(COLOR_MENU);
    set_cursor_position(25, 5);
    printf("╚═══════════════════════════════════════════════╝");
    
    int y = 7;
    for (int i = 0; i < 10; i++) {
        set_cursor_position(5, y);
        set_color(snakes[i].color);
        printf("[%d] %s", i + 1, snakes[i].name);
        
        set_cursor_position(35, y);
        if (snakes[i].unlocked) {
            set_color(10);
            printf("✓ OWNED");
        } else {
            set_color(12);
            printf("Cost: %d pts", snakes[i].cost);
        }
        
        set_cursor_position(5, y + 1);
        set_color(8);
        printf("    %s", snakes[i].specialty);
        
        y += 3;
    }
    
    set_color(COLOR_TEXT);
    set_cursor_position(5, y + 1);
    printf("Press 1-9, 0 for snake 10 to buy/select | ESC to return");
    set_color(COLOR_RESET);
    
    while (1) {
        if (_kbhit()) {
            char c = _getch();
            if (c == 27) return; // ESC
            
            int idx = -1;
            if (c >= '1' && c <= '9') idx = c - '1';
            else if (c == '0') idx = 9;
            
            if (idx >= 0 && idx < 10) {
                if (snakes[idx].unlocked) {
                    current_snake = idx;
                    set_cursor_position(5, y + 3);
                    set_color(10);
                    printf("Selected: %s                                    ", snakes[idx].name);
                    Sleep(1000);
                } else {
                    if (total_points >= snakes[idx].cost) {
                        total_points -= snakes[idx].cost;
                        snakes[idx].unlocked = 1;
                        current_snake = idx;
                        set_cursor_position(5, y + 3);
                        set_color(14);
                        printf("Purchased: %s!                                  ", snakes[idx].name);
                        Sleep(1500);
                        show_shop(); // Refresh
                        return;
                    } else {
                        set_cursor_position(5, y + 3);
                        set_color(12);
                        printf("Not enough points!                                    ");
                        Sleep(1000);
                    }
                }
            }
        }
        Sleep(50);
    }
}

void show_pause() {
    set_color(14);
    set_cursor_position(WIDTH / 2 - 8, HEIGHT / 2 - 2);
    printf("╔════════════════╗");
    set_cursor_position(WIDTH / 2 - 8, HEIGHT / 2 - 1);
    printf("║  GAME PAUSED   ║");
    set_cursor_position(WIDTH / 2 - 8, HEIGHT / 2);
    printf("╠════════════════╣");
    set_cursor_position(WIDTH / 2 - 8, HEIGHT / 2 + 1);
    printf("║ P - Resume     ║");
    set_cursor_position(WIDTH / 2 - 8, HEIGHT / 2 + 2);
    printf("║ ESC - Menu     ║");
    set_cursor_position(WIDTH / 2 - 8, HEIGHT / 2 + 3);
    printf("╚════════════════╝");
    set_color(COLOR_RESET);
}

void show_goodbye() {
    system("cls");
    set_color(13);
    set_cursor_position(20, 10);
    type_text("Thank you for playing Ultimate Snake Game!", 40);
    set_cursor_position(25, 12);
    set_color(14);
    type_text("Hope you had an amazing time!", 40);
    set_cursor_position(18, 14);
    set_color(11);
    type_text("Come back soon for more slithering adventures!", 35);
    set_cursor_position(30, 17);
    set_color(10);
    type_text("- r0se4U ♥", 50);
    printf("\n\n");
    set_color(COLOR_RESET);
}

int point_equals(Point a, Point b) {
    return a.x == b.x && a.y == b.y;
}

void place_food(Point *food, Point snake[], int snake_len) {
    Point p;
    int ok;
    do {
        ok = 1;
        p.x = (rand() % (WIDTH - 2)) + 1;
        p.y = (rand() % (HEIGHT - 2)) + 1;
        for (int i = 0; i < snake_len; i++) {
            if (point_equals(p, snake[i])) { ok = 0; break; }
        }
    } while (!ok);
    *food = p;
}

int main(void) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned)time(NULL));
    hide_cursor();
    
    init_snakes();
    show_intro();
    
    int quit_game = 0;
    
    while (!quit_game) {
        int choice = show_menu();
        
        if (choice == 4) {
            quit_game = 1;
            break;
        } else if (choice == 3) {
            show_shop();
            continue;
        } else if (choice == 2 || choice == 1) {
            // Game loop
            system("cls");
            
            Point snake[MAX_SNAKE];
            int snake_len = 4;
            Direction dir = RIGHT;
            Point food;
            int score = 0;
            int base_speed = 100;
            int speed_ms = base_speed + snakes[current_snake].speed_bonus;
            int start_time = (int)time(NULL);
            
            int cx = WIDTH / 2, cy = HEIGHT / 2;
            for (int i = 0; i < snake_len; i++) {
                snake[i].x = cx - i;
                snake[i].y = cy;
            }
            
            place_food(&food, snake, snake_len);
            
            draw_border();
            draw_scoreboard(score, 0);
            
            int game_over = 0;
            int paused = 0;
            
            while (!game_over) {
                game_time = (int)time(NULL) - start_time;
                
                // Input
                if (_kbhit()) {
                    int c = _getch();
                    
                    if (c == 27) { // ESC
                        break; // Back to menu
                    } else if (c == 'p' || c == 'P') {
                        paused = !paused;
                        if (paused) {
                            show_pause();
                        } else {
                            // Redraw game
                            system("cls");
                            draw_border();
                        }
                    }
                    
                    if (!paused) {
                        Direction new_dir = dir;
                        if (c == 'w' || c == 'W') new_dir = UP;
                        else if (c == 's' || c == 'S') new_dir = DOWN;
                        else if (c == 'a' || c == 'A') new_dir = LEFT;
                        else if (c == 'd' || c == 'D') new_dir = RIGHT;
                        
                        if (!((dir == UP && new_dir == DOWN) || (dir == DOWN && new_dir == UP) ||
                              (dir == LEFT && new_dir == RIGHT) || (dir == RIGHT && new_dir == LEFT))) {
                            dir = new_dir;
                        }
                    }
                }
                
                if (paused) {
                    Sleep(50);
                    continue;
                }
                
                // Move
                Point new_head = snake[0];
                switch (dir) {
                    case UP:    new_head.y -= 1; break;
                    case DOWN:  new_head.y += 1; break;
                    case LEFT:  new_head.x -= 1; break;
                    case RIGHT: new_head.x += 1; break;
                }
                
                // Collision with walls
                if (new_head.x <= 0 || new_head.x >= WIDTH - 1 || 
                    new_head.y <= 0 || new_head.y >= HEIGHT - 1) {
                    game_over = 1;
                    break;
                }
                
                // Collision with self
                for (int i = 0; i < snake_len; i++) {
                    if (point_equals(new_head, snake[i])) {
                        game_over = 1;
                        break;
                    }
                }
                if (game_over) break;
                
                // Move body
                for (int i = snake_len; i > 0; i--) {
                    snake[i] = snake[i - 1];
                }
                snake[0] = new_head;
                
                // Food
                int ate_food = 0;
                if (point_equals(snake[0], food)) {
                    snake_len++;
                    ate_food = 1;
                    int points_earned = 10 * snakes[current_snake].point_multiplier;
                    score += points_earned;
                    total_points += points_earned;
                    
                    if (score > high_score) high_score = score;
                    
                    if (snake_len < MAX_SNAKE) place_food(&food, snake, snake_len);
                    if (speed_ms > 30) speed_ms -= 2;
                } else {
                    // Clear old tail only if didn't eat food
                    if (snake_len < MAX_SNAKE) {
                        set_cursor_position(snake[snake_len].x, snake[snake_len].y);
                        printf(" ");
                    }
                }
                
                // Draw snake
                set_color(snakes[current_snake].color);
                set_cursor_position(snake[0].x, snake[0].y);
                printf("@");
                
                for (int i = 1; i < snake_len; i++) {
                    set_cursor_position(snake[i].x, snake[i].y);
                    printf("o");
                }
                
                // Draw food
                set_color(COLOR_FOOD);
                set_cursor_position(food.x, food.y);
                printf("*");
                
                draw_scoreboard(score, game_time);
                
                Sleep(speed_ms);
            }
            
            // Game over
            if (game_over) {
                set_color(12);
                set_cursor_position(WIDTH / 2 - 10, HEIGHT / 2);
                printf("╔════════════════════╗");
                set_cursor_position(WIDTH / 2 - 10, HEIGHT / 2 + 1);
                printf("║   GAME OVER!       ║");
                set_cursor_position(WIDTH / 2 - 10, HEIGHT / 2 + 2);
                printf("╠════════════════════╣");
                set_cursor_position(WIDTH / 2 - 10, HEIGHT / 2 + 3);
                printf("║ Score: %-11d║", score);
                set_cursor_position(WIDTH / 2 - 10, HEIGHT / 2 + 4);
                printf("║ Press any key...   ║");
                set_cursor_position(WIDTH / 2 - 10, HEIGHT / 2 + 5);
                printf("╚════════════════════╝");
                set_color(COLOR_RESET);
                
                while (!_kbhit()) Sleep(50);
                _getch();
            }
        }
    }
    
    show_goodbye();
    show_cursor();
    return 0;
}
