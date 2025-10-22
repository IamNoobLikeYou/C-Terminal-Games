// 3D Console Racing Game For Windows :) 
// For Windows: gcc racing_3d.c -o racing_3d.exe
// If you Done it by your self I mean compile THE code ..! then use the gcc 
// Enjoy, YOUR Three Dimensional CAR racing GAME wrinten in C program hehe.... 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/select.h>
    #define CLEAR_SCREEN "clear"
#endif

#define SCR_W 80
#define SCR_H 24
#define ROAD_CENTER (SCR_W/2)
#define MAX_OBSTACLES 32

typedef struct {
    int x, y;
    int active;
    int type; // 0=car, 1=tree, 2=cone
} Obstacle;

typedef struct {
    char name[30];
    int color;
    int speed_bonus;
    int handling_bonus;
    int cost;
    int unlocked;
    char specialty[50];
} CarType;

// Global variables
#ifdef _WIN32
HANDLE hConsole;
#endif

CarType cars[10];
int current_car = 0;
int total_points = 0;
int high_score = 0;
char screen_buffer[SCR_H][SCR_W + 1];

// Cross-platform functions
void init_terminal() {
#ifdef _WIN32
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &info);
#else
    printf("\033[?25l");
    fflush(stdout);
#endif
}

void restore_terminal() {
#ifdef _WIN32
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &info);
#else
    printf("\033[?25h");
    printf("\033[0m");
    fflush(stdout);
#endif
}

void set_color(int color) {
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, color);
#else
    const char* colors[] = {
        "\033[0m", "\033[0m", "\033[32m", "\033[0m", "\033[0m",
        "\033[35m", "\033[0m", "\033[37m", "\033[90m", "\033[94m",
        "\033[92m", "\033[96m", "\033[91m", "\033[95m", "\033[93m", "\033[97m"
    };
    if (color >= 0 && color <= 15) printf("%s", colors[color]);
#endif
}

void set_cursor(int x, int y) {
#ifdef _WIN32
    COORD pos = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hConsole, pos);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
    fflush(stdout);
#endif
}

void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

#ifndef _WIN32
struct termios orig_termios;
void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}
int getch_nonblock() {
    int c = 0;
    if (read(STDIN_FILENO, &c, 1) < 0) return -1;
    return c;
}
#endif

int get_key() {
#ifdef _WIN32
    if (!_kbhit()) return -1;
    return _getch();
#else
    if (!kbhit()) return -1;
    return getch_nonblock();
#endif
}

void type_text(const char* text, int delay) {
    for (int i = 0; text[i] != '\0'; i++) {
        putchar(text[i]);
        fflush(stdout);
        sleep_ms(delay);
    }
}

void init_cars() {
    strcpy(cars[0].name, "Classic Racer");
    cars[0].color = 14;
    cars[0].speed_bonus = 0;
    cars[0].handling_bonus = 0;
    cars[0].cost = 0;
    cars[0].unlocked = 1;
    strcpy(cars[0].specialty, "Balanced - Perfect for beginners");

    strcpy(cars[1].name, "Speed Demon");
    cars[1].color = 12;
    cars[1].speed_bonus = 3;
    cars[1].handling_bonus = 0;
    cars[1].cost = 500;
    cars[1].unlocked = 0;
    strcpy(cars[1].specialty, "30% Faster acceleration");

    strcpy(cars[2].name, "Drift King");
    cars[2].color = 11;
    cars[2].speed_bonus = 0;
    cars[2].handling_bonus = 3;
    cars[2].cost = 450;
    cars[2].unlocked = 0;
    strcpy(cars[2].specialty, "Superior handling and control");

    strcpy(cars[3].name, "Golden Cruiser");
    cars[3].color = 14;
    cars[3].speed_bonus = 1;
    cars[3].handling_bonus = 1;
    cars[3].cost = 600;
    cars[3].unlocked = 0;
    strcpy(cars[3].specialty, "Balanced speed and handling");

    strcpy(cars[4].name, "Turbo Boost");
    cars[4].color = 13;
    cars[4].speed_bonus = 5;
    cars[4].handling_bonus = -1;
    cars[4].cost = 800;
    cars[4].unlocked = 0;
    strcpy(cars[4].specialty, "Extreme speed, harder to control");

    strcpy(cars[5].name, "Street Racer");
    cars[5].color = 10;
    cars[5].speed_bonus = 2;
    cars[5].handling_bonus = 2;
    cars[5].cost = 750;
    cars[5].unlocked = 0;
    strcpy(cars[5].specialty, "Great all-rounder");

    strcpy(cars[6].name, "Neon Flash");
    cars[6].color = 9;
    cars[6].speed_bonus = 4;
    cars[6].handling_bonus = 1;
    cars[6].cost = 900;
    cars[6].unlocked = 0;
    strcpy(cars[6].specialty, "Fast with good control");

    strcpy(cars[7].name, "Shadow Racer");
    cars[7].color = 8;
    cars[7].speed_bonus = 3;
    cars[7].handling_bonus = 3;
    cars[7].cost = 1000;
    cars[7].unlocked = 0;
    strcpy(cars[7].specialty, "Elite performance");

    strcpy(cars[8].name, "Hyper Beast");
    cars[8].color = 12;
    cars[8].speed_bonus = 6;
    cars[8].handling_bonus = 0;
    cars[8].cost = 1200;
    cars[8].unlocked = 0;
    strcpy(cars[8].specialty, "Maximum speed - Expert only");

    strcpy(cars[9].name, "Ultimate Champion");
    cars[9].color = 5;
    cars[9].speed_bonus = 5;
    cars[9].handling_bonus = 5;
    cars[9].cost = 1500;
    cars[9].unlocked = 0;
    strcpy(cars[9].specialty, "LEGENDARY - Perfect in every way");
}

void show_intro() {
    system(CLEAR_SCREEN);
    
    set_color(12);
    set_cursor(15, 5);
    type_text("=====================================================", 8);
    set_cursor(15, 6);
    type_text("               ATTENTION WARNING                     ", 8);
    set_cursor(15, 7);
    type_text("=====================================================", 8);
    
    set_color(14);
    set_cursor(12, 9);
    type_text("For the best racing experience and to avoid bugs:", 25);
    
    set_color(11);
    set_cursor(15, 11);
    type_text("Please MAXIMIZE your terminal window to", 25);
    set_cursor(20, 12);
    type_text("FULL SCREEN before continuing!", 25);
    
    set_color(10);
    set_cursor(18, 14);
    type_text("Press F11 or maximize the window now", 30);
    
    set_color(8);
    set_cursor(23, 16);
    type_text("Press any key when ready...", 35);
    
    set_color(7);
    while (get_key() == -1) sleep_ms(50);
    while (get_key() != -1) sleep_ms(10);
    
    system(CLEAR_SCREEN);
    set_color(14);
    set_cursor(20, 7);
    type_text("================================================", 10);
    set_cursor(20, 8);
    type_text("                                                ", 10);
    set_cursor(20, 9);
    type_text("        3D CONSOLE RACING CHALLENGE 2025        ", 10);
    set_cursor(20, 10);
    type_text("                                                ", 10);
    set_cursor(20, 11);
    type_text("================================================", 10);
    
    set_color(13);
    set_cursor(15, 14);
    type_text("This game was created by r0se4U with love", 30);
    
    set_color(11);
    set_cursor(25, 17);
    type_text("Press any key to start racing...", 40);
    
    set_color(7);
    while (get_key() == -1) sleep_ms(50);
    while (get_key() != -1) sleep_ms(10);
}

int show_menu() {
    system(CLEAR_SCREEN);
    set_color(13);
    set_cursor(28, 4);
    printf("================================");
    set_cursor(28, 5);
    printf("         RACING MENU            ");
    set_cursor(28, 6);
    printf("================================");
    
    set_color(15);
    set_cursor(30, 9);
    printf("1. Start Race");
    set_cursor(30, 11);
    printf("2. Car Shop");
    set_cursor(30, 13);
    printf("3. High Score: %d", high_score);
    set_cursor(30, 15);
    printf("4. Quit Game");
    
    set_color(11);
    set_cursor(30, 18);
    printf("Select (1-4): ");
    set_color(7);
    fflush(stdout);
    
    while (1) {
        int c = get_key();
        if (c >= '1' && c <= '4') return c - '0';
        sleep_ms(50);
    }
}

void show_shop() {
    system(CLEAR_SCREEN);
    set_color(13);
    set_cursor(20, 1);
    printf("===================================================");
    set_cursor(20, 2);
    printf("                 CAR SHOP                          ");
    set_cursor(20, 3);
    printf("===================================================");
    set_cursor(20, 4);
    set_color(14);
    printf("    Your Points: %d                                ", total_points);
    set_color(13);
    set_cursor(20, 5);
    printf("===================================================");
    
    int y = 7;
    for (int i = 0; i < 10; i++) {
        set_cursor(5, y);
        set_color(cars[i].color);
        printf("[%d] %s", (i + 1) % 10, cars[i].name);
        
        set_cursor(35, y);
        if (cars[i].unlocked) {
            set_color(10);
            printf("OWNED");
        } else {
            set_color(12);
            printf("Cost: %d pts", cars[i].cost);
        }
        
        set_cursor(5, y + 1);
        set_color(8);
        printf("    %s", cars[i].specialty);
        
        y += 2;
    }
    
    set_color(15);
    set_cursor(5, y + 1);
    printf("Press 1-9, 0 for car 10 to buy/select | Q to return");
    set_color(7);
    fflush(stdout);
    
    while (1) {
        int c = get_key();
        if (c == 'q' || c == 'Q' || c == 27) return;
        
        int idx = -1;
        if (c >= '1' && c <= '9') idx = c - '1';
        else if (c == '0') idx = 9;
        
        if (idx >= 0 && idx < 10) {
            if (cars[idx].unlocked) {
                current_car = idx;
                set_cursor(5, y + 3);
                set_color(10);
                printf("Selected: %s!                                       ", cars[idx].name);
                fflush(stdout);
                sleep_ms(1000);
            } else {
                if (total_points >= cars[idx].cost) {
                    total_points -= cars[idx].cost;
                    cars[idx].unlocked = 1;
                    current_car = idx;
                    set_cursor(5, y + 3);
                    set_color(14);
                    printf("Purchased: %s!                                    ", cars[idx].name);
                    fflush(stdout);
                    sleep_ms(1500);
                    show_shop();
                    return;
                } else {
                    set_cursor(5, y + 3);
                    set_color(12);
                    printf("Not enough points!                                       ");
                    fflush(stdout);
                    sleep_ms(1000);
                }
            }
        }
        sleep_ms(50);
    }
}

void show_goodbye() {
    system(CLEAR_SCREEN);
    set_color(13);
    set_cursor(18, 9);
    type_text("Thank you for playing 3D Racing Challenge!", 40);
    set_cursor(22, 11);
    set_color(14);
    type_text("Hope you enjoyed the ride!", 40);
    set_cursor(15, 13);
    set_color(11);
    type_text("Come back soon for more high-speed thrills!", 35);
    set_cursor(28, 16);
    set_color(10);
    type_text("- r0se4U", 50);
    printf("\n\n");
    set_color(7);
}

void clear_buffer() {
    for (int y = 0; y < SCR_H; y++) {
        for (int x = 0; x < SCR_W; x++) screen_buffer[y][x] = ' ';
        screen_buffer[y][SCR_W] = '\0';
    }
}

void draw_buffer() {
    set_cursor(0, 0);
    for (int y = 0; y < SCR_H; y++) {
        printf("%s\n", screen_buffer[y]);
    }
    fflush(stdout);
}

int clamp(int v, int min, int max) {
    return v < min ? min : (v > max ? max : v);
}

void run_race() {
    system(CLEAR_SCREEN);
    
    int car_pos = 0;
    int speed = 2 + cars[current_car].speed_bonus / 2;
    int max_speed = 12 + cars[current_car].speed_bonus;
    int handling = 3 + cars[current_car].handling_bonus;
    int score = 0;
    int distance = 0;
    float road_curve = 0.0f;
    float curve_speed = 0.0f;
    int running = 1;
    int paused = 0;
    int frames = 0;
    
    Obstacle obs[MAX_OBSTACLES];
    for (int i = 0; i < MAX_OBSTACLES; i++) obs[i].active = 0;
    
    for (int i = 0; i < 8; i++) {
        obs[i].active = 1;
        obs[i].y = rand() % (SCR_H - 8) + 2;
        obs[i].x = (rand() % 40) - 20;
        obs[i].type = rand() % 3;
    }
    
    for (int y = 0; y < SCR_H + 2; y++) {
        for (int x = 0; x < SCR_W; x++) putchar(' ');
        putchar('\n');
    }
    set_cursor(0, 0);
    
    while (running) {
        int c = get_key();
        if (c != -1) {
            if (c == 'q' || c == 'Q' || c == 27) break;
            if (c == 'p' || c == 'P') paused = !paused;
            
            if (!paused) {
                if (c == 'a' || c == 'A') car_pos -= handling;
                if (c == 'd' || c == 'D') car_pos += handling;
                if (c == 'w' || c == 'W') speed = clamp(speed + 1, 1, max_speed);
                if (c == 's' || c == 'S') speed = clamp(speed - 1, 1, max_speed);
            }
        }
        
        if (paused) {
            set_color(14);
            set_cursor(ROAD_CENTER - 8, SCR_H / 2);
            printf("================");
            set_cursor(ROAD_CENTER - 8, SCR_H / 2 + 1);
            printf("  GAME PAUSED  ");
            set_cursor(ROAD_CENTER - 8, SCR_H / 2 + 2);
            printf("  P - Resume   ");
            set_cursor(ROAD_CENTER - 8, SCR_H / 2 + 3);
            printf("  Q - Quit     ");
            set_cursor(ROAD_CENTER - 8, SCR_H / 2 + 4);
            printf("================");
            set_color(7);
            fflush(stdout);
            sleep_ms(100);
            continue;
        }
        
        car_pos = clamp(car_pos, -18, 18);
        
        curve_speed += ((rand() % 100) - 50) * 0.001f;
        curve_speed *= 0.95f;
        road_curve += curve_speed;
        road_curve = clamp((int)road_curve, -15, 15);
        
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (!obs[i].active) continue;
            obs[i].y += 1 + speed / 4;
            
            if (obs[i].y >= SCR_H - 4) {
                int car_x = ROAD_CENTER + car_pos;
                int obs_x = ROAD_CENTER + obs[i].x + (int)road_curve;
                
                if (abs(obs_x - car_x) < 3 && obs[i].type == 0) {
                    running = 0;
                    break;
                }
                
                obs[i].active = 1;
                obs[i].y = rand() % 8;
                obs[i].x = (rand() % 40) - 20;
                obs[i].type = rand() % 3;
            }
        }
        
        if (rand() % 15 < speed / 2) {
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (!obs[i].active) {
                    obs[i].active = 1;
                    obs[i].y = rand() % 8;
                    obs[i].x = (rand() % 40) - 20;
                    obs[i].type = rand() % 3;
                    break;
                }
            }
        }
        
        score += speed;
        distance += speed;
        frames++;
        
        // Gradually increase speed over time
        if (frames % 100 == 0 && speed < max_speed) {
            speed++;
        }
        
        clear_buffer();
        
        // Draw sky with gradient
        for (int y = 0; y < 6; y++) {
            for (int x = 0; x < SCR_W; x++) {
                screen_buffer[y][x] = (y % 2 == 0) ? '.' : ' ';
            }
        }
        
        // Draw road with 3D perspective effect
        for (int y = 6; y < SCR_H - 3; y++) {
            float perspective = (float)(y - 6) / (float)(SCR_H - 9);
            perspective = perspective * perspective; // Quadratic for more dramatic effect
            
            int half_width = (int)(perspective * 32.0f) + 3;
            int center = ROAD_CENTER + (int)(road_curve * perspective);
            int left = center - half_width;
            int right = center + half_width;
            
            // Determine row color based on depth
            int row_color = (y % 8 < 4) ? 1 : 0; // Alternating stripes
            
            for (int x = 0; x < SCR_W; x++) {
                if (x < left - 3 || x > right + 3) {
                    // Far background - grass with trees
                    if (x % 8 == (y % 5) && (rand() % 100) > 90) {
                        screen_buffer[y][x] = 'Y'; // Tree marker
                    } else {
                        screen_buffer[y][x] = ((x + y) % 3 == 0) ? ',' : ' ';
                    }
                } else if (x >= left - 3 && x < left) {
                    // Left roadside - grass
                    screen_buffer[y][x] = ((x + y) % 2 == 0) ? '\'' : ',';
                } else if (x > right && x <= right + 3) {
                    // Right roadside - grass
                    screen_buffer[y][x] = ((x + y) % 2 == 0) ? '\'' : ',';
                } else if (x == left || x == right) {
                    // Road edges - white lines
                    screen_buffer[y][x] = row_color ? '=' : '#';
                } else if (abs(x - center) < 1) {
                    // Center line - dashed
                    if ((y + (int)(road_curve)) % 6 < 3) {
                        screen_buffer[y][x] = ':';
                    } else {
                        screen_buffer[y][x] = (row_color && (x + y) % 4 == 0) ? '.' : ' ';
                    }
                } else {
                    // Road surface with texture
                    if (row_color && (x + y) % 4 == 0) {
                        screen_buffer[y][x] = '.';
                    } else {
                        screen_buffer[y][x] = ' ';
                    }
                }
            }
        }
        
        // Draw obstacles with 3D scaling
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (!obs[i].active) continue;
            int oy = obs[i].y;
            if (oy < 6 || oy >= SCR_H - 3) continue;
            
            float perspective = (float)(oy - 6) / (float)(SCR_H - 9);
            perspective = perspective * perspective;
            
            int center_y = ROAD_CENTER + (int)(road_curve * perspective);
            int ox = center_y + (int)(obs[i].x * perspective);
            
            // Scale obstacle size based on distance
            int size = (int)(perspective * 3.0f) + 1;
            
            if (ox >= 0 && ox < SCR_W) {
                char symbol = obs[i].type == 0 ? 'H' : (obs[i].type == 1 ? 'Y' : 'A');
                
                // Draw obstacle with size
                for (int dy = 0; dy < size && (oy - dy) >= 0; dy++) {
                    for (int dx = -size/2; dx <= size/2; dx++) {
                        int px = ox + dx;
                        int py = oy - dy;
                        if (px >= 0 && px < SCR_W && py >= 0 && py < SCR_H) {
                            if (dy == 0 && dx == 0) {
                                screen_buffer[py][px] = symbol;
                            } else {
                                screen_buffer[py][px] = (obs[i].type == 1) ? '^' : 
                                                       (obs[i].type == 0) ? 'X' : 'V';
                            }
                        }
                    }
                }
            }
        }
        
        // Draw player car with 3D look
        int car_y = SCR_H - 3;
        int car_x = ROAD_CENTER + car_pos;
        car_x = clamp(car_x, 3, SCR_W - 4);
        
        if (car_x - 2 >= 0 && car_x + 2 < SCR_W) {
            // Top of car (smaller)
            screen_buffer[car_y - 2][car_x] = 'o';
            screen_buffer[car_y - 2][car_x - 1] = '_';
            screen_buffer[car_y - 2][car_x + 1] = '_';
            
            // Middle of car
            screen_buffer[car_y - 1][car_x] = '|';
            screen_buffer[car_y - 1][car_x - 1] = '/';
            screen_buffer[car_y - 1][car_x + 1] = '\\';
            screen_buffer[car_y - 1][car_x - 2] = ' ';
            screen_buffer[car_y - 1][car_x + 2] = ' ';
            
            // Bottom of car (wider - 3D effect)
            screen_buffer[car_y][car_x - 2] = '[';
            screen_buffer[car_y][car_x - 1] = '=';
            screen_buffer[car_y][car_x] = 'H';
            screen_buffer[car_y][car_x + 1] = '=';
            screen_buffer[car_y][car_x + 2] = ']';
        }
        
        char hud[100];
        snprintf(hud, sizeof(hud), " Speed:%d Distance:%d Score:%d | W/S:Speed A/D:Steer P:Pause Q:Quit ", 
                 speed, distance, score);
        int len = strlen(hud);
        for (int i = 0; i < len && i < SCR_W; i++) {
            screen_buffer[0][i] = hud[i];
        }
        
        draw_buffer();
        
        // Colored HUD at bottom
        set_cursor(0, SCR_H);
        set_color(11); // Cyan
        printf(" Speed:");
        set_color(14); // Yellow
        printf("%2d", speed);
        set_color(11);
        printf(" | Distance:");
        set_color(10); // Green
        printf("%d", distance);
        set_color(11);
        printf(" | Score:");
        set_color(13); // Magenta
        printf("%d", score);
        set_color(7);
        printf("                    ");
        
        set_cursor(0, SCR_H + 1);
        set_color(cars[current_car].color);
        printf(" Car: %s", cars[current_car].name);
        set_color(11);
        printf(" | Points:");
        set_color(14);
        printf("%d", total_points);
        set_color(8);
        printf(" | W/S:Speed A/D:Steer P:Pause Q:Quit");
        set_color(7);
        printf("      ");
        fflush(stdout);
        
        int delay = 90 - (speed * 4);
        if (delay < 25) delay = 25;
        sleep_ms(delay);
    }
    
    total_points += score / 10;
    if (score > high_score) high_score = score;
    
    set_color(12);
    set_cursor(ROAD_CENTER - 12, SCR_H / 2 - 1);
    printf("=========================");
    set_cursor(ROAD_CENTER - 12, SCR_H / 2);
    printf("     RACE FINISHED!      ");
    set_cursor(ROAD_CENTER - 12, SCR_H / 2 + 1);
    printf("=========================");
    set_cursor(ROAD_CENTER - 12, SCR_H / 2 + 2);
    printf(" Final Score: %d        ", score);
    set_cursor(ROAD_CENTER - 12, SCR_H / 2 + 3);
    printf(" Points Earned: %d      ", score / 10);
    set_cursor(ROAD_CENTER - 12, SCR_H / 2 + 4);
    printf(" Press any key...        ");
    set_cursor(ROAD_CENTER - 12, SCR_H / 2 + 5);
    printf("=========================");
    set_color(7);
    fflush(stdout);
    
    while (get_key() == -1) sleep_ms(50);
    while (get_key() != -1) sleep_ms(10);
}

int main() {
    srand((unsigned)time(NULL));
    
#ifndef _WIN32
    enable_raw_mode();
#endif
    
    init_terminal();
    init_cars();
    show_intro();
    
    int quit = 0;
    while (!quit) {
        int choice = show_menu();
        
        if (choice == 1) {
            run_race();
        } else if (choice == 2) {
            show_shop();
        } else if (choice == 4) {
            quit = 1;
        }
    }
    
    show_goodbye();
    restore_terminal();
    return 0;
}
