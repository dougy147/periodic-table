#include <raylib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG  0
#define WIDTH  1600
#define HEIGHT 900
#define FONT_SIZE WIDTH/32
#define NB_ELEMENTS 118

#define NB_CELL_WIDTH  19
#define NB_CELL_HEIGHT 10

/*View mode: whole table OR single element*/
typedef enum {
    TABLE_VIEW,
    SINGLE_VIEW,
} View;

/* Elements' used properties */
typedef struct element {
    int atomic_number;
    float atomic_mass;
    char symbol[3];
    char name[14];
    char color[10];
    int x_position_in_table;
    int y_position_in_table;
} Element;

/* Box containing elements */
typedef struct box {
    Element element;
    Rectangle rectangle;
} Box;

void display_single_element(Element element, Font font) {
    int ho = HEIGHT / 8; // height offset

    /* Display name */
    // DrawTextEx(fonts[i], messages[i], positions[i], fonts[i].baseSize*2.0f, (float)spacings[i], colors[i]);
    DrawTextEx(font, TextFormat("%s", element.name),(Vector2){WIDTH/2 - (strlen(element.name) * FONT_SIZE / 2.0) / 2.0,FONT_SIZE},FONT_SIZE,0,YELLOW);

    /* Surrounding square */
    int square_side = HEIGHT - 2 * ho;
    int wo = (WIDTH - square_side) / 2;
    int border_size = FONT_SIZE / 4;
    DrawRectangleLinesEx((Rectangle){wo, ho, square_side, square_side}, border_size, YELLOW);

    /* Display atomic number, symbol, atomic mass inside of square */
    char atomic_number_string[3];
    sprintf(atomic_number_string,"%d",element.atomic_number);
    DrawTextEx(font,TextFormat("%d", element.atomic_number),
	    (Vector2){WIDTH/2 - (strlen(atomic_number_string) * FONT_SIZE / 2.0) / 2, ho + FONT_SIZE},
	    FONT_SIZE,
	    0,
	    BLUE);

    int symbol_font_size_factor = 4;
    DrawTextEx(font,TextFormat("%s", element.symbol),
	    (Vector2){WIDTH/2 - (strlen(element.symbol) * (FONT_SIZE * symbol_font_size_factor) / 2.0) / 2, ho + square_side / 2 - (symbol_font_size_factor * FONT_SIZE) / 2},
	    FONT_SIZE * symbol_font_size_factor,
	    0,
	    YELLOW);

    char mass_string[15];
    sprintf(mass_string,"%.6f",element.atomic_mass);
    DrawTextEx(font,
	    mass_string,
	    (Vector2){WIDTH/2 - (strlen(mass_string) * FONT_SIZE / 2.0) / 2, ho + square_side - 2 * FONT_SIZE},
	    FONT_SIZE,
	    0,
	    BLUE);
}

void display_full_table(Element *elements, Font font) {
    int wo = WIDTH  / 16;  /* width offset */
    int ho = HEIGHT / 16; /* height offset */
    int w_table = (WIDTH  - 2.0 * wo);
    int h_table = (HEIGHT - 2.0 * ho);
    int w_cell = w_table / NB_CELL_WIDTH; // cell width
    int h_cell = h_table / NB_CELL_HEIGHT;

    /*fix excessive pixels (when table width not dividible by nb_cell_*/
    wo = wo + (w_table - NB_CELL_WIDTH  * w_cell) / 2.0;
    ho = ho + (h_table - NB_CELL_HEIGHT * h_cell) / 2.0;
    w_table = w_table - (w_table - NB_CELL_WIDTH  * w_cell);
    h_table = h_table - (h_table - NB_CELL_HEIGHT * h_cell);

    /*draw borders and grid*/
    //DrawRectangleLines(wo, ho, w_table, h_table, YELLOW);
    //for (int i = 0; i < NB_CELL_WIDTH; i++) {
    //    for (int j = 0; j < NB_CELL_HEIGHT; j++) {
    //        DrawRectangleLines(wo + i * w_cell, ho + j * h_cell, w_cell, h_cell, DARKGRAY);
    //    }
    //}

    /* Place elements inside their respective box */
    Color color;
    for (int i = 0; i < NB_ELEMENTS; i++) {
	if      (strcmp("RED",    elements[i].color) == 0) { color = RED; }
	else if (strcmp("YELLOW", elements[i].color) == 0) { color = YELLOW; }
	else if (strcmp("BLUE",   elements[i].color) == 0) { color = BLUE; }
	else if (strcmp("GREEN",  elements[i].color) == 0) { color = GREEN; }
	int box_size_x = w_cell / 1.2;
	int box_size_y = h_cell / 1.2;
        DrawRectangle(
        	wo + (w_cell / 2 - box_size_x / 2) + elements[i].x_position_in_table * w_cell,
        	ho + (h_cell / 2 - box_size_y / 2) + elements[i].y_position_in_table * h_cell,
        	box_size_x,
        	box_size_y,
        	color
        	);
	DrawText(
		TextFormat("%d", elements[i].atomic_number),
        	wo + (w_cell / 2 - FONT_SIZE/2) + elements[i].x_position_in_table * w_cell,
        	ho + (0.1 * h_cell) + elements[i].y_position_in_table * h_cell,
		w_cell/3.5,
		DARKGRAY
		);
	DrawTextEx(
		font,
		TextFormat("%s", elements[i].symbol),
        	(Vector2){wo + (w_cell / 2 - (strlen(elements[i].symbol) * FONT_SIZE / 3) / 2) + elements[i].x_position_in_table * w_cell, ho + (h_cell / 2 - (FONT_SIZE / 3) / 2) + elements[i].y_position_in_table * h_cell},
		FONT_SIZE/1.5,
		0,
		BLACK
		);
    }
}

void parse_periodic_table_csv(char *filename, Element *elements, Box *boxes) {
    FILE *csv_file = fopen(filename, "r");
    bool csv_has_headers = true;
    int buffer_size = 20000;
    char buffer[buffer_size];
    int current_atom = 0;
    int characteristics_index = 0;
    Element element;
    Box box;
    /* AtomicNumber,Element,Symbol,AtomicMass,TableRow,TableCol,BoxColor, */
    for(;;) {
	int res = fread(buffer, 1, buffer_size, csv_file); // read the file in one go
	if (ferror(csv_file)) {
	    printf("ERROR: cannot read file \"%s\"\n", filename);
	    return;
	}

	for (int i = 0; i < res; i++) {
	    if (csv_has_headers && i == 0) {
		while (buffer[i] != '\n') i++;
		i++;
	    }

	    if (buffer[i] == '\n') {
		elements[current_atom] = element;
#if 1
		printf("-------\n");
		printf("Imported \"%s\"\n", element.name);
		printf("    Atomic number: %d\n", element.atomic_number);
		printf("    Symbol: %s\n", element.symbol);
		printf("    Atomic mass: %f\n", element.atomic_mass);
		printf("    Table position: x=%d, y=%d\n", element.x_position_in_table,element.y_position_in_table);
		printf("    Box color: %s\n", element.color);
#endif

		Rectangle box_rectangle;
		int wo = WIDTH  / 16;  /* width offset */
    		int ho = HEIGHT / 16; /* height offset */
    		int w_table = (WIDTH  - 2.0 * wo);
    		int h_table = (HEIGHT - 2.0 * ho);
    		int w_cell = w_table / NB_CELL_WIDTH; // cell width
    		int h_cell = h_table / NB_CELL_HEIGHT;

    		/*fix excessive pixels (when table width not dividible by nb_cell_*/
    		wo = wo + (w_table - NB_CELL_WIDTH  * w_cell) / 2.0;
    		ho = ho + (h_table - NB_CELL_HEIGHT * h_cell) / 2.0;
    		w_table = w_table - (w_table - NB_CELL_WIDTH  * w_cell);
    		h_table = h_table - (h_table - NB_CELL_HEIGHT * h_cell);
		box_rectangle = (Rectangle){
		    wo + (w_cell / 2 - w_cell / 1.2 / 2) + element.x_position_in_table * w_cell,
		    ho + (h_cell / 2 - h_cell / 1.2 / 2) + element.y_position_in_table * h_cell,
		    w_cell / 1.2,
		    h_cell / 1.2};
		boxes[current_atom].rectangle = box_rectangle;
		boxes[current_atom].element = elements[current_atom];

		current_atom++;
		characteristics_index = 0;
		Element element;
	    }

	    if (characteristics_index > 6) {
	        continue;
	    }

	    char current_characteristic[15];
	    int  index = 0; // pretty inefficient way i guess
	    while (buffer[i] != ',' && buffer[i] != '\n') {
		current_characteristic[index] = buffer[i];
		i++;
		index++;
	    }
	    current_characteristic[index] = '\0';

	    switch (characteristics_index) {
		case 0:
		    element.atomic_number = atoi(current_characteristic);
		    break;
		case 1:
		    strcpy(element.name, current_characteristic);
		    break;
		case 2:
		    strcpy(element.symbol, current_characteristic);
		    break;
		case 3:
		    element.atomic_mass = atof(current_characteristic);
		    break;
		case 4:
		    element.y_position_in_table = atoi(current_characteristic);
		    break;
		case 5:
		    element.x_position_in_table = atoi(current_characteristic);
		    break;
		case 6:
		    strcpy(element.color,current_characteristic);
		    break;
	    }

	    if (buffer[i] == ',') characteristics_index++;
	}

	if (feof(csv_file)) {
	    printf("INFO: Successfully parsed \"%s\"\n", filename);
	    return;
	}
    }
}

int mouse_hovers_element(Box *boxes, Element *elements) {
    int mx = GetMousePosition().x;
    int my = GetMousePosition().y;
    for (int i = 0; i < NB_ELEMENTS; i++) {
	if (mx >= boxes[i].rectangle.x && mx <= (boxes[i].rectangle.x + boxes[i].rectangle.width) &&
		my >= boxes[i].rectangle.y && my <= (boxes[i].rectangle.y + boxes[i].rectangle.height)) {
	    return elements[i].atomic_number;
	}
    }
    return -1;
}

int main(void) {
    Element elements[NB_ELEMENTS];
    Box boxes[NB_ELEMENTS]; // each element resides in its own box

    //char filename[] = "Periodic Table of Elements.csv";
    char filename[] = "elements.csv";

    parse_periodic_table_csv(filename, elements, boxes);

    int view_mode = TABLE_VIEW;
    int hovered_element;
    int element_index;

    InitWindow(WIDTH, HEIGHT, "Periodic table");
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(30);

    Font font = LoadFontEx("fonts/LinBiolinum_Rah.ttf",FONT_SIZE*5,NULL,0);

    while (!WindowShouldClose()) {
	BeginDrawing();
	ClearBackground(BLACK);

	if (view_mode == TABLE_VIEW) {
	    DrawText(
		    "Periodic table",
		    WIDTH/2 - (strlen("periodic table") * FONT_SIZE / 2) / 2, FONT_SIZE,
		    FONT_SIZE,
		    DARKGRAY);
	    display_full_table(elements, font);
	} else if (view_mode == SINGLE_VIEW) {
	    display_single_element(elements[element_index], font);
	}

	hovered_element = mouse_hovers_element(boxes, elements);
	if (hovered_element >= 0 && view_mode == TABLE_VIEW) {
	    DrawRectangleLinesEx(
		    boxes[hovered_element-1].rectangle,
		    5,
		    BLACK);
	    DrawRectangleLinesEx(
		    boxes[hovered_element-1].rectangle,
		    2,
		    LIGHTGRAY);
	    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && view_mode == TABLE_VIEW) {
		view_mode = SINGLE_VIEW;
		element_index = hovered_element - 1;
	    }
	}

	/* In single view, rotate through elements when clicking. */
	if (view_mode == SINGLE_VIEW && (IsKeyPressed(KEY_D) || GetMouseWheelMove() > 0)) {
	    element_index++;
	    if (element_index >= NB_ELEMENTS - 1) element_index = 0;
	}
	if (view_mode == SINGLE_VIEW && (IsKeyPressed(KEY_A) || GetMouseWheelMove() < 0)) {
	    element_index--;
	    if (element_index < 0) element_index = NB_ELEMENTS - 1;
	}
	/* To go back to full table */
	if (view_mode == SINGLE_VIEW && IsKeyPressed(KEY_S)) {
	    view_mode = TABLE_VIEW;
	}

	EndDrawing();
    }
    return 0;
}
