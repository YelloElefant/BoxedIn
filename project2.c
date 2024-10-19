/*
ENGGEN131 2024
C Project - Boxed In
Resource file - project2.c

You can use this program to test the functions that you write before grading them with CodeRunner
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#define ROWS 5
#define COLS 7

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
void TestLocatePerson(void);
void TestGameOver(void);

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

/***************************************************************************************
Test functions.
You should define your own test functions here as you progress through the project.
***************************************************************************************/
void TestMakeBox(void)
{
	printf("Box 1 with '*':\n");
	MakeBox("*", 5, 3); // A box using '*' character with width 5 and height 3

	printf("\nBox 2 with 'XO':\n");
	MakeBox("XO", 10, 4); // A box alternating between 'X' and 'O' with width 10 and height 4

	printf("\nBox 3 with 'ABC':\n");
	MakeBox("ABC", 7, 5);
}

void TestPrintRoom(void)
{
	int room[ROWS][COLS] =
		 {{0, 0, 1, 1, 1, 1, 1},
		  {1, 1, 1, 0, 0, 0, 1},
		  {1, 2, 5, 3, 0, 0, 1},
		  {1, 1, 1, 0, 3, 2, 1},
		  {1, 2, 1, 1, 3, 0, 1}};

	PrintRoom(room);
}

void TestInitialiseRoom(void)
{
	char layout[] = "111111105000000000000000000001111111";
	int room[ROWS][COLS];

	InitialiseRoom(room, layout);
	PrintRoom(room);
}

void TestLocatePerson(void)
{
	char layout[] = "0011111000111005100010032011001002320100111043010001000110000111110000011000000000110000000011110000";
	int room[ROWS][COLS];
	int row, col;

	// Initialize the room
	InitialiseRoom(room, layout);

	// Print the room to see the configuration
	PrintRoom(room);

	// Locate the person in the room
	LocatePerson(room, &row, &col);

	// Print the location of the person
	printf("The person is located at (%d, %d)\n", row, col);
}

void TestMove(void)
{
	char layout[] = "111111105001103001102301100201111111";
	char moves[100] = {0}; // Initialize moves array to empty
	int room[ROWS][COLS];

	InitialiseRoom(room, layout); // Initialize the room layout
	PrintRoom(room);					// Print the initial room state

	// Make a move down ('s')
	MakeMove(room, 's', moves);
	PrintRoom(room); // Print the room after the move
	printf("Person moved: %s\n", moves);

	// Make a move right ('d')
	MakeMove(room, 'w', moves);
	PrintRoom(room); // Print the room after the move
	printf("Person moved: %s\n", moves);

	// Make another move down ('s')
	MakeMove(room, 'd', moves);
	PrintRoom(room); // Print the room after the move
	printf("Person moved: %s\n", moves);
}

void TestGameOver(void)
{
	char layoutA[] = "111111105001103001100301100001111111"; // No targets covered
	char layoutB[] = "111111100001100001104501100401111111"; // All targets covered
	char layoutC[] = "111111100001100001136001100401111111"; // One person on target

	int roomA[ROWS][COLS];
	int roomB[ROWS][COLS];
	int roomC[ROWS][COLS];
	int gameOver;

	InitialiseRoom(roomA, layoutA); // Initialize the room with layout A
	InitialiseRoom(roomB, layoutB); // Initialize the room with layout B
	InitialiseRoom(roomC, layoutC); // Initialize the room with layout C

	PrintRoom(roomA); // Print room A
	PrintRoom(roomB); // Print room B
	PrintRoom(roomC); // Print room C

	gameOver = CheckGameOver(roomA);				// Check if game A is over
	printf("Game A is over: %d\n", gameOver); // Output should be 0 (not over)

	gameOver = CheckGameOver(roomB);				// Check if game B is over
	printf("Game B is over: %d\n", gameOver); // Output should be 1 (game is over)

	gameOver = CheckGameOver(roomC);				// Check if game C is over
	printf("Game C is over: %d\n", gameOver); // Output should be 0 (not over)
}

// Test for undo functionality
void TestUndo(void)
{
	char layout[] = "111111100501100301100001100001111111";
	char moves[100] = {0};
	int room[ROWS][COLS];
	InitialiseRoom(room, layout);

	printf("Initial Room:\n");
	PrintRoom(room);

	// moves
	MakeMove(room, 's', moves); // down
	MakeMove(room, 'd', moves); // right
	MakeMove(room, 's', moves); // down
	MakeMove(room, 'a', moves); // left

	printf("\nRoom after moves:\n");
	PrintRoom(room);
	printf("Moves recorded: %s\n", moves);

	// undo moves
	printf("Undo last move (a):\n");
	MakeMove(room, 'z', moves);
	PrintRoom(room);

	printf("Undo last move (s):\n");
	MakeMove(room, 'z', moves);
	PrintRoom(room);

	printf("Undo last move (d):\n");
	MakeMove(room, 'z', moves);
	PrintRoom(room);

	printf("Undo last move (s):\n");
	MakeMove(room, 'z', moves);
	PrintRoom(room);

	printf("Final moves recorded: %s\n", moves);
}
/***************************************************************************************
Test program
***************************************************************************************/
int main(void)
{
	printf("Testing Task One\n================\n\n");
	TestMakeBox();

	printf("\n\nTesting Task Two\n================\n\n");
	TestPrintRoom();

	printf("\n\nTesting Task Three\n==================\n\n");
	TestInitialiseRoom();

	printf("\n\nTesting Task Four\n==================\n\n");
	TestLocatePerson();

	printf("\n\nTesting Task Five-Eight\n==================\n\n");
	TestMove();

	printf("\n\nTesting Task Nine\n==================\n\n");
	TestGameOver();

	printf("\n\nTesting Task Ten\n==================\n\n");
	TestUndo();

	return 0;
}
