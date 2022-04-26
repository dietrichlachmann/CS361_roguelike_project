#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

typedef struct Room{
	// top left coord for room
	int x_pos;
	int y_pos;
	int height;
	int width;

	// entity arrays to be added later
	//Monster** monsters;
	//Item** items;

} Room;

typedef struct Player{
	int x_pos;
	int y_pos;
	int health;
} Player;

int setup_screen();
Room** setup_map();
Room* create_room(int y, int x, int height, int width);
int draw_room(Room* room);
Player* setup_player();
int input_handler(int input, Player* player);
int move_player(int y, int x, Player* player);
int check_position(int y, int x, Player* player);

int main(){
	// -----MAIN VARIABLES-----
	Player* player;
	int input_char = 0;

	// -----INITIALIZATIONS-----
	setup_screen();
	setup_map();
	player = setup_player();

	// -----MAIN GAME LOOP-----
	while((input_char = getch()) != 'q'){
		input_handler(input_char, player);
	}

	// -----CLEAN UP-----
	endwin();

	return 0;
}

int setup_screen(){
	// ncurses functions to initialize play area
	initscr();
	printw("Press h to move left, j to move down, k to move up, l to move right");
	printw("\tPress q to quit");
	noecho();
	refresh();

	return 1;
}

Room** setup_map(){
	Room** rooms;

	// each **rooms = one floor
	rooms = (Room**)malloc(sizeof(Room*) * 6);

	// temporary rooms to generate for MVP
	rooms[0] = create_room(13, 13, 6, 8);
	draw_room(rooms[0]);
	rooms[1] = create_room(5, 2, 6, 8);
	draw_room(rooms[1]);
	rooms[2] = create_room(20, 7, 6, 8);
	draw_room(rooms[2]);

	return rooms;
}

Room* create_room(int y, int x, int height, int width){
	Room* new_room;

	new_room = (Room*)malloc(sizeof(Room));
	
	// initialize struct values to store room data
	new_room->y_pos = y;
	new_room->x_pos = x;
	new_room->height = height;
	new_room->width = width;

	return new_room;
}

int draw_room(Room* room){
	int y = 0;
	int x = 0;
	
	for(x = room->x_pos; x < room->x_pos + room->width; x++){
		// top of room
		mvprintw(room->y_pos, x, "-");
		// bottom of room
		mvprintw(room->y_pos + room->height - 1, x, "-");
	}

	// walls of room
	for(y = room->y_pos + 1; y < room->y_pos + room->height - 1; y++){
		mvprintw(y, room->x_pos, "|");
		mvprintw(y, room->x_pos + room->width - 1, "|");
		// floor of room
		for(x = room->x_pos + 1; x < room->x_pos + room->width - 1; x++){
			mvprintw(y, x, ".");
		}
	}

	// temp path for MVP
	mvprintw(13, 14, ".");
	mvprintw(12, 14, ".");
	mvprintw(11, 14, ".");
	mvprintw(10, 14, ".");
	mvprintw(9, 14, ".");
	mvprintw(9, 13, ".");
	mvprintw(9, 12, ".");
	mvprintw(9, 11, ".");
	mvprintw(9, 10, ".");
	mvprintw(9, 9, ".");

	return 1;
} 

Player* setup_player(){
	// initialize new player
	Player* new_player;
	new_player = malloc(sizeof(Player));

	// set initial x and y positions
	new_player->x_pos = 17;
	new_player->y_pos = 17;
	new_player->health = 50;

	// draw player and cursor
	mvprintw(new_player->y_pos, new_player->x_pos, "@");
	move(new_player->y_pos, new_player->x_pos);

	return new_player;
}

int input_handler(int input, Player* player){
	int y_new = 0;
	int x_new = 0;
	switch(input){
		// move left
		case 'h' :
			y_new = player->y_pos;
			x_new = player->x_pos - 1;
			break;
		// move down
		case 'j' :
			y_new = player->y_pos + 1;
			x_new = player->x_pos;
			break;
		// move up
		case 'k' :
			y_new = player->y_pos - 1;
			x_new = player->x_pos;
			break;
		// move right
		case 'l' :
			y_new = player->y_pos;
			x_new = player->x_pos + 1;
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
			move_player(y, x, player);
			break;
		default:
			move(player->y_pos, player->x_pos);
			break;
	}
}

int move_player(int y, int x, Player* player){
	// replace current player pos with floor tile
	mvprintw(player->y_pos, player->x_pos, ".");
	
	// update player pos with new x and y
	player->y_pos = y;
	player->x_pos = x;

	// reprint player on screen nd move cursor to match
	mvprintw(player->y_pos, player->x_pos, "@");
	move(player->y_pos, player->x_pos);
}
