/*
ENGGEN131 2024
C Project - Boxed In
Resource file - project2.c

You can use this program to test the functions that you write before grading them with CodeRunner
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#define ROWS 10
#define COLS 10

#define SPACE 0
#define WALL 1
#define TARGET 2
#define BOX 3
#define BOX_ON_TARGET 4
#define PERSON 5
#define PERSON_ON_TARGET 6

// Function prototype declarations
void MakeBox(char *design, int width, int height);
void PrintRoom(int room[ROWS][COLS]);
void InitialiseRoom(int room[ROWS][COLS], char *layout);
void LocatePerson(int room[ROWS][COLS], int *row, int *col);
void MakeMove(int room[ROWS][COLS], char move, char *allMoves);
int CheckGameOver(int room[ROWS][COLS]);
void UndoMove(int room[ROWS][COLS], char *allMoves);
void BackupRoom(int backup[ROWS][COLS], int room[ROWS][COLS]);

// Task One ///////////////////////////////////////////////////////////////
void MakeBox(char *design, int width, int height)
{
	int designLength = strlen(design);
	int designIndex = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			printf("%c", design[designIndex]);
			designIndex = (designIndex + 1) % designLength;
		}
		printf("\n");
	}
}

// Task Two ///////////////////////////////////////////////////////////////
void PrintRoom(int room[ROWS][COLS])
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			switch (room[i][j])
			{
			case SPACE:
				printf(" ");
				break;
			case WALL:
				printf("#");
				break;
			case TARGET:
				printf("*");
				break;
			case BOX:
				printf("O");
				break;
			case BOX_ON_TARGET:
				printf("@");
				break;
			case PERSON:
			case PERSON_ON_TARGET:
				printf("X");
				break;
			default:
				printf(" ");
				break;
			}
		}
		printf("\n"); // Move to the next row after finishing the current row
	}
}

// Task Three ///////////////////////////////////////////////////////////////
void InitialiseRoom(int room[ROWS][COLS], char *layout)
{
	int index = 0;
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (layout[index] != '\0')
			{
				room[i][j] = layout[index] - '0';
				index++;
			}
			else
			{
				room[i][j] = WALL; // Fill remaining spaces with WALL if layout is too short
			}
		}
	}
}

// Task Four ///////////////////////////////////////////////////////////////
void LocatePerson(int room[ROWS][COLS], int *row, int *col)
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (room[i][j] == PERSON || room[i][j] == TARGET + PERSON)
			{ // Handle person on target
				*row = i;
				*col = j;
				return;
			}
		}
	}
}

// Task Five - Task Eight ////////////////////////////////////////////////////////
void MakeMove(int room[ROWS][COLS], char move, char *allMoves)
{
	static int backup[ROWS][COLS]; // Backup for undo
	int personRow, personCol;

	// Find the person's current position
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (room[i][j] == PERSON || room[i][j] == PERSON_ON_TARGET)
			{
				personRow = i;
				personCol = j;
				break;
			}
		}
	}

	// Backup current state before move
	BackupRoom(backup, room);

	// Handle 'z' for undo
	if (move == 'z')
	{
		UndoMove(room, allMoves);
		return;
	}

	int newRow = personRow;
	int newCol = personCol;

	// Determine the new position based on the move
	if (move == 'w')
		newRow--; // up
	else if (move == 'a')
		newCol--; // left
	else if (move == 's')
		newRow++; // down
	else if (move == 'd')
		newCol++; // right

	// Check if the new position is valid
	if (room[newRow][newCol] == SPACE || room[newRow][newCol] == TARGET)
	{
		// Move person
		room[personRow][personCol] = (room[personRow][personCol] == PERSON_ON_TARGET) ? TARGET : SPACE;
		room[newRow][newCol] = (room[newRow][newCol] == TARGET) ? PERSON_ON_TARGET : PERSON;

		// Record move
		sprintf(allMoves + strlen(allMoves), "%c0", move); // Person moved, no box moved
	}
	else if (room[newRow][newCol] == BOX || room[newRow][newCol] == BOX_ON_TARGET)
	{
		// Check if box can be moved
		int boxNewRow = newRow + (move == 'w' ? -1 : (move == 's' ? 1 : 0));
		int boxNewCol = newCol + (move == 'a' ? -1 : (move == 'd' ? 1 : 0));

		if (room[boxNewRow][boxNewCol] == SPACE || room[boxNewRow][boxNewCol] == TARGET)
		{
			// Move box
			room[boxNewRow][boxNewCol] = (room[boxNewRow][boxNewCol] == TARGET) ? BOX_ON_TARGET : BOX;
			room[newRow][newCol] = (room[newRow][newCol] == BOX_ON_TARGET) ? PERSON_ON_TARGET : PERSON;
			room[personRow][personCol] = (room[personRow][personCol] == PERSON_ON_TARGET) ? TARGET : SPACE;

			// Record move
			sprintf(allMoves + strlen(allMoves), "%c1", move); // Person and box moved
		}
		else
		{
			// Invalid move; revert the backup state
			for (int i = 0; i < ROWS; i++)
			{
				for (int j = 0; j < COLS; j++)
				{
					room[i][j] = backup[i][j];
				}
			}
		}
	}
}

// Task Nine /////////////////////////////////////////////////////////////////
int CheckGameOver(int room[ROWS][COLS])
{
	int targetFound = 0; // Flag to check if any target exists
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			// Check if there are uncovered targets or person standing on a target
			if (room[i][j] == TARGET || room[i][j] == PERSON_ON_TARGET)
			{
				return 0; // Game is not over if any target is uncovered
			}
			// If BOX_ON_TARGET is found, that target is covered
			if (room[i][j] == BOX_ON_TARGET)
			{
				targetFound = 1; // Mark that a target exists and is covered
			}
		}
	}
	return targetFound;
}

// Task Ten /////////////////////////////////////////////////////////////////
void UndoMove(int room[ROWS][COLS], char *allMoves)
{
	// Check if there are moves to undo
	if (strlen(allMoves) == 0)
	{
		return; // No moves to undo
	}

	// Remove the last move recorded
	allMoves[strlen(allMoves) - 1] = '\0';

	// Restore the previous state of the room
	int previous[ROWS][COLS];
	BackupRoom(previous, room); // Backup current state

	// Restore the room to the state before the last move
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			room[i][j] = previous[i][j];
		}
	}
}

// Backup the state of the room
void BackupRoom(int backup[ROWS][COLS], int room[ROWS][COLS])
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			backup[i][j] = room[i][j];
		}
	}
}

/***********************************************************/
/***********************************************************/
/********* DO NOT MODIFY ANY CODE BELOW THIS POINT *********/
/***********************************************************/
/***********************************************************/

/* GetMove() returns the entered character. Invalid characters ignored */
char GetMove(void)
{
	char move;
	printf("\nEnter move: ");
	scanf("%c", &move);
	// Ignore any characters that are invalid
	while ((move != 'w') && (move != 'a') && (move != 's') && (move != 'd') && (move != 'z'))
	{
		scanf("%c", &move);
	}
	return move;
}

/* Boxed In Game */
int main(void)
{
	char layout[200] = "0011111000111005100010032011001002320100111043010000100011000011111000000000000000000000000000000000";
	int room[ROWS][COLS] = {0};
	char allMoves[1000] = {0};

	printf("ENGGEN131 - C Project 2024\n");
	printf("                          ... presents ...\n");
	printf("   +------+  ____                    _   _____         _ \n");
	printf("  /      /| |  _ \\                  | | |_   _|       | |\n");
	printf(" +------+ | | |_) | _____  _____  __| |   | |  _ __   | |\n");
	printf(" |      | + |  _ < / _ \\ \\/ / _ \\/ _` |   | | | '_ \\  | |\n");
	printf(" |      |/  | |_) | (_) >  <  __/ (_| |  _| |_| | | | |_|\n");
	printf(" +------+   |____/ \\___/_/\\_\\___|\\__,_| |_____|_| |_| (_)\n");
	printf("\n");
	printf("\nMove the player (X) using the keys 'w', 'a', 's', 'd'\n");
	printf("Good luck!\n\n\n");

	/* Main game loop */
	InitialiseRoom(room, layout);
	PrintRoom(room);
	while (!CheckGameOver(room))
	{
		MakeMove(room, GetMove(), allMoves);
		PrintRoom(room);
	}
	printf("\n\nCONGRATULATIONS!\nGame over!\n");
	printf("Moves: %s", allMoves);
	return 0;
}
