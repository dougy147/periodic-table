#include <raylib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define WIDTH 1600
#define HEIGHT 900
#define FONT_SIZE WIDTH/32

typedef struct element {
    int atomic_number;
    float atomic_mass;
    char symbol[3]; // symbols have 2char max
    char name[14];
} Element;


void display_element(Element element) {
    int ho = HEIGHT / 8; // height offset

    // Display name
    DrawText(TextFormat("%s", element.name),WIDTH/2 - (strlen(element.name) * FONT_SIZE / 2.0) / 2.0,FONT_SIZE,FONT_SIZE,YELLOW);

    // Draw square
    int square_side = HEIGHT - 2 * ho;
    int wo = (WIDTH - square_side) / 2; // width offset
    DrawRectangleLines(wo, ho, square_side, square_side, YELLOW);

    // Display atomic number, symbol, atomic mass inside of square
    char atomic_number_string[3];
    sprintf(atomic_number_string,"%d",element.atomic_number);
    DrawText(TextFormat("%d", element.atomic_number),
	    WIDTH/2 - (strlen(atomic_number_string) * FONT_SIZE / 2.0) / 2,
	    ho + FONT_SIZE,
	    FONT_SIZE,
	    BLUE);

    int symbol_font_size_factor = 8;
    DrawText(TextFormat("%s", element.symbol),
	    WIDTH/2 - (strlen(element.symbol) * (FONT_SIZE * symbol_font_size_factor) / 2.0) / 2,
	    ho + square_side / 2 - (symbol_font_size_factor * FONT_SIZE) / 2,
	    FONT_SIZE * symbol_font_size_factor,
	    YELLOW);

    char mass_string[15];
    sprintf(mass_string,"%.6f",element.atomic_mass); // count characters in atomic mass float for correct display
    DrawText(TextFormat("%.6f", element.atomic_mass),
	    WIDTH/2 - (strlen(mass_string) * FONT_SIZE / 2.0) / 2,
	    ho + square_side - 2 * FONT_SIZE,
	    FONT_SIZE,
	    BLUE);
}

void parse_periodic_table_csv(char *filename, Element *elements) {
    FILE *list_file = fopen(filename, "r");

    /* https://gist.githubusercontent.com/GoodmanSciences/c2dd862cd38f21b0ad36b8f96b4bf1ee/raw/1d92663004489a5b6926e944c1b3d9ec5c40900e/Periodic%2520Table%2520of%2520Elements.csv */
    /* How things are organized in CSV file*/
    /* AtomicNumber,Element,Symbol,AtomicMass,NumberofNeutrons,NumberofProtons,NumberofElectrons,Period,Group,Phase,Radioactive,Natural,Metal,Nonmetal,Metalloid,Type,AtomicRadius,Electronegativity,FirstIonization,Density,MeltingPoint,BoilingPoint,NumberOfIsotopes,Discoverer,Year,SpecificHeat,NumberofShells,NumberofValence */

    int buffer_size = 15000;
    char buffer[buffer_size];

    int current_atom = 0;
    int characteristics_index = 0;
    Element element;
    for(;;) {
	int res = fread(buffer, 1, buffer_size, list_file); // read the file in one go
	if (ferror(list_file)) {
	    printf("ERROR: cannot read file \"%s\"\n", filename);
	    return;
	}

	for (int i = 0; i < res; i++) {

	    if (buffer[i] == '\n') {
		elements[current_atom] = element;
		printf("-------\n");
		printf("Storing element %s\n", element.name);
		printf("    atomic num %d\n", element.atomic_number);
		printf("    symbol %s\n", element.symbol);
		printf("    atomic mass %f\n", element.atomic_mass);
		current_atom++;
		characteristics_index = 0;
		Element element;
	    }

	    if (characteristics_index > 3) continue;

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
		default:
		    continue; // i guess
		    break;
	    }

	    if (buffer[i] == ',') characteristics_index++;
	}

	if (feof(list_file)) {
	    printf("INFO: successfully parsed \"%s\"\n", filename);
	    return;
	}
    }
}

int main() {

    Element elements[118];
    char filename[] = "Periodic Table of Elements.csv";
    parse_periodic_table_csv(filename, elements);

    int element_index = 0; // start with hydrogen

    InitWindow(WIDTH,HEIGHT,"Periodic table");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
	BeginDrawing();
	ClearBackground(BLACK);

	display_element(elements[element_index]);

	// for now we "rotate" through elements; later we will show the whole table and be able to click
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
	    element_index++;
	    if (element_index >= 118 - 1) element_index = 0;
	}
	if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
	    element_index--;
	    if (element_index < 0) element_index = 118 - 1;
	}

	EndDrawing();
    }
    return 0;
}
