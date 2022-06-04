#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>

typedef struct Position{
	int x;
	int y;
} Position;

typedef struct Room{
	Position position;
	int height;
	int width;
	Position** door;
} Room;

typedef struct Player{
	Position position;
	int health;
} Player;

int setup_screen(char* name);
Room** setup_map();
Room* create_room(int y, int x, int height, int width);
int draw_room(Room* room);
Player* setup_player();
int input_handler(int input, Player* player);
int move_player(int y, int x, Player* player);
int check_position(int y, int x, Player* player);
int connect_doors(Position* door_one, Position* door_two);

int total_gold = 0;
int collected_gold = 0;
int total_steps = 0;
int exit_open = 0;
int game_over = 0;

int main(int argc, char** argv){
	// -----MAIN VARIABLES-----
	Player* player;
	int input_char = 0;
	FILE* f_ptr;

	if(argc != 2){
        printf("Expected a character name!\n");
        return 1;
	}

	// -----INITIALIZATIONS-----
	setup_screen(argv[1]);
	setup_map();
	player = setup_player();

	// -----MAIN GAME LOOP-----
	while(((input_char = getch()) != 'q') && (game_over == 0)){
		input_handler(input_char, player);
	}

	if(game_over == 1){
        f_ptr = fopen("score.txt", "w");
        fprintf(f_ptr, "%d\n%d\n%d", total_steps * 2,
                collected_gold * 30, game_over * 10);
	}

	// -----CLEAN UP-----
	endwin();
	fclose(f_ptr);

	return 0;
}

int setup_screen(char* name){
	// ncurses functions to initialize play area
	initscr();
	printw("Press h to move left, j to move down,");
    printw(" k to move up, l to move right");
    printw("or press q to quit\n");
	printw("Welcome %s. Collect all the (G)old and move to the (E)xit to win!"
        , name);

	srand(time(NULL));
	noecho();
	refresh();

	return 1;
}

Room** setup_map(){
	Room** rooms;
    int i = 0;

	// each **rooms = one floor
	rooms = (Room**)malloc(sizeof(Room*) * 6);

	// starting room
	rooms[0] = create_room(4, 4, 5, 5);
    draw_room(rooms[0]);

	// top row
	for(i = 1; i < 3; i++){
        rooms[i] = create_room(rand() % ((i+1)*(i+1)*3) + ((i+1)*(i+1)*3),
                                4, (rand() % 6) + 5, (rand() % 8) + 5);
        draw_room(rooms[i]);
	}
	connect_doors(rooms[0]->door[3], rooms[1]->door[1]);
	connect_doors(rooms[1]->door[3], rooms[2]->door[1]);

	// bottom row
	for(i = 0; i < 3; i++){
        rooms[i+3] = create_room(rand() % ((i+1)*(i+1)*3) + ((i+1)*(i+1)*3),
                                  15, (rand() % 6) + 5, (rand() % 8) + 5);
        draw_room(rooms[i+3]);
	}
	connect_doors(rooms[3]->door[3], rooms[4]->door[1]);
	connect_doors(rooms[4]->door[3], rooms[5]->door[1]);

	// connect rows
	connect_doors(rooms[0]->door[2], rooms[3]->door[0]);
	connect_doors(rooms[1]->door[2], rooms[4]->door[0]);
	connect_doors(rooms[2]->door[2], rooms[5]->door[0]);

	return rooms;
}

Room* create_room(int y, int x, int height, int width){
	Room* new_room;

	new_room = (Room*)malloc(sizeof(Room));

	// initialize struct values to store room data
	new_room->position.x = y;
	new_room->position.y = x;
	new_room->height = height;
	new_room->width = width;

	new_room->door = malloc(sizeof(Position) * 4);

	// top door
	new_room->door[0] = malloc(sizeof(Position));
	new_room->door[0]->x = rand() % (width - 2) + new_room->position.x + 1;
	new_room->door[0]->y = new_room->position.y;

	// left door
	new_room->door[1] = malloc(sizeof(Position));
	new_room->door[1]->x = new_room->position.x;
	new_room->door[1]->y = rand() % (height - 2) + new_room->position.y + 1;

	// bottom door
	new_room->door[2] = malloc(sizeof(Position));
	new_room->door[2]->x = rand() % (width - 2) + new_room->position.x + 1;
	new_room->door[2]->y = new_room->position.y + new_room->height - 1;

	// right door
	new_room->door[3] = malloc(sizeof(Position));
	new_room->door[3]->x = new_room->position.x + width - 1;
	new_room->door[3]->y = rand() % (height - 2) + new_room->position.y + 1;

	return new_room;
}

int draw_room(Room* room){
	int y = 0;
	int x = 0;
	int gold = 0;
	int rand_check = 0;

	for(x = room->position.x; x < room->position.x + room->width; x++){
		// top of room
		mvprintw(room->position.y, x, "-");
		// bottom of room
		mvprintw(room->position.y + room->height - 1, x, "-");
	}

	// walls of room
	for(y = room->position.y + 1; y < room->position.y + room->height - 1;
	 y++){
		mvprintw(y, room->position.x, "|");
		mvprintw(y, room->position.x + room->width - 1, "|");
		// floor of room
		for(x = room->position.x + 1; x < room->position.x + room->width - 1;
		 x++){
			mvprintw(y, x, ".");
			if(gold == 0){
                rand_check = rand() % 5;
                if(rand_check == 1){
                    mvprintw(y, x, "G");
                    gold++;
                    total_gold++;
                }
			}
		}
	}

	mvprintw(room->door[0]->y, room->door[0]->x, "+");
	mvprintw(room->door[1]->y, room->door[1]->x, "+");
	mvprintw(room->door[2]->y, room->door[2]->x, "+");
	mvprintw(room->door[3]->y, room->door[3]->x, "+");

	return 1;
}

int connect_doors(Position* door_one, Position* door_two){
    Position temp;
    Position prev;
    int count = 0;

    temp.x = door_one->x;
    temp.y = door_one->y;

    prev = temp;

    // essentially a very simple shortest path algorithm
    while(1){
        if((abs((temp.x - 1) - door_two->x) < abs(temp.x - door_two->x))
            && (mvinch(temp.y, temp.x - 1) == ' ')){
            prev.x = temp.x;
            temp.x = temp.x - 1;
        }
        else if((abs((temp.x + 1) - door_two->x) < abs(temp.x - door_two->x))
            && (mvinch(temp.y, temp.x + 1) == ' ')){
            prev.x = temp.x;
            temp.x = temp.x + 1;
        }
        else if((abs((temp.y + 1) - door_two->y) < abs(temp.y - door_two->y))
            && (mvinch(temp.y + 1, temp.x) == ' ')){
            prev.y = temp.y;
            temp.y = temp.y + 1;
        }
        else if((abs((temp.y - 1) - door_two->y) < abs(temp.y - door_two->y))
            && (mvinch(temp.y - 1, temp.x) == ' ')){
            prev.y = temp.y;
            temp.y = temp.y - 1;
        }
        else{
            if(count == 0){
                temp = prev;
                count++;
                continue;
            }
            else{
                return 0;
            }
        }

        mvprintw(temp.y, temp.x, "#");
    }

    return 1;
}

Player* setup_player(){
	// initialize new player
	Player* new_player;
	new_player = malloc(sizeof(Player));

	// set initial x and y positions
	new_player->position.x = 6;
	new_player->position.y = 6;
	new_player->health = 50;

	// draw player and cursor
	mvprintw(new_player->position.y, new_player->position.x, "@");
	mvprintw(5, 7, "E");
	move(new_player->position.y, new_player->position.x);

	return new_player;
}

int input_handler(int input, Player* player){
	int y_new = 0;
	int x_new = 0;
	switch(input){
		// move left
		case 'h' :
			y_new = player->position.y;
			x_new = player->position.x - 1;
			total_steps++;
			break;
		// move down
		case 'j' :
			y_new = player->position.y + 1;
			x_new = player->position.x;
			total_steps++;
			break;
		// move up
		case 'k' :
			y_new = player->position.y - 1;
			x_new = player->position.x;
			total_steps++;
			break;
		// move right
		case 'l' :
			y_new = player->position.y;
			x_new = player->position.x + 1;
			total_steps++;
			break;
		default:
			break;
	}

	check_position(y_new, x_new, player);
}

int check_position(int y, int x, Player* player){
	int tile = 0;
	switch(mvinch(y, x)){
		case '.':
        case '+':
        case '#':
			move_player(y, x, player);
			break;
        case 'G':
            move_player(y, x, player);
            collected_gold++;
            if(collected_gold == total_gold){
                exit_open = 1;
            }
            break;
        case 'E':
            if(exit_open == 1){
                game_over = 1;
            }
		default:
			move(player->position.y, player->position.x);
			break;
	}
}

int move_player(int y, int x, Player* player){
	// replace current player pos with floor tile
	mvprintw(player->position.y, player->position.x, ".");

	// update player pos with new x and y
	player->position.y = y;
	player->position.x = x;

	// reprint player on screen nd move cursor to match
	mvprintw(player->position.y, player->position.x, "@");
	move(player->position.y, player->position.x);
}
