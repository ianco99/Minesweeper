//18-06-2022
//Ian Kuznicki
//Trabajo Practico N°6 Analisis de los Generos
//Buscaminas


#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>

using namespace::std;


#pragma region Enums & Structs
//Boxes that make up the matrix
struct Box {
	int y; //position in y
	int x; //position in x
	int state; //is box hidden, cleared, flagged or is it showing a bomb?
	int number; //what is the box's number
	bool marked0; //stops recursive function from repeating itself onto the same zeros
};

//Player's cursor position
struct Cursor {
	int y; //position in y
	int x; //position in x
};

//Difficulties available in the game
struct Difficulty {	
	std::string name;	//Name of the difficulty mode
	int rows;	//Rows for this difficulty
	int colls;	//Collumns for this difficulty
	int bombs;	//Bombs for this difficulty
};

//Values and properties used in playmode
struct GameStats
{
	std::string difficultyName; //Name of the difficulty selected

	int rows;	//Rows used
	int colls;	//Collumns used
	int bombMax;	//Maximum bombs using
	int flagCount;	//Flags available

	bool hasBombyHelped = false;	//Checks if player can use help option
	bool firstTry;	//Checks if player has already started the game
	bool restart = false;	//Checks if player wants to restart game
	bool playing = false;	//Checks if player still wants to keep playing
	bool win;	//Checks if player has won
	bool hasLost;	//Checks if player has lost
};


//Options in difficulty mode
enum class difficultySelection{Easy = 49,Medium,Hard,GoBack};

//Symbols used to make up the grid
enum GridSymbols{UpperLeft = 218, HorizontalLine = 196,HorLeftUni = 195, HorRightUni = 180, HorXUni = 197, UpperRight = 191, VerticalLine = 179, VertLeftUni = 195, HorDownUni = 194, BotLeft = 192, BotUpUni = 193, BotRight = 217};

//Sprites used
enum Sprites{Bomb = 35, Flag = 201}; 

//Main menu options
enum class Menu{Play = 49,Settings,Credits,Quit};

//Settings menu options
enum class SettingsMenu{DisableBomby = 49,CheckStats,GoBack};

//In-game options
enum class PlayOption{Flag = 70,Clear = 67,Quit = 81};

//States of values that can be printed
enum class PrintableValues{Hide,Show,Flag,Bomb = 9};

//Colors for each number & cursor's highlighter
enum class Colors{ZeroBlack = 8, OneBlue = 9,TwoGreen = 10, ThreeRed = 12, FourPurple = 13, FiveYellow = 14, SixCyan = 11, SevenGray = 15, EightWhite = 7, CursorHighlight = 12};

//Player's controls
enum class Controls{ Up = 119, UpAlt = 87, Right = 100, RightAlt = 68, Left = 97, LeftAlt = 65, Down = 115, DownAlt = 83, Quit = 113, QuitAlt = 81,Clear = 99, ClearAlt = 67,Flag = 102, FlagAlt = 70, Restart = 114, RestartAlt = 82, AskHelp = 66, AskHelpAlt = 98};
#pragma endregion

Cursor cursor; //Keeps track of player's coordinates

const int matrixSize = 30; //Total size of game's matrix

Box matrix[matrixSize][matrixSize]; //Matrix of box's structs that the player will interact with

//int menuMatrix[3]; //Matrix for the menu navigation
//int settingsMatrix[4]; //Matrix for settings navigation

int timesPlayed = 0;	//Times player has played

float bestTimeEasy = 54201;	//Starting record in easy mode
float bestTimeNormal = 113648;	//Starting record in medium mode
float bestTimeHard = 600000;	//Starting record in hard mode

int timesBombyInteract = 0;	//Times player has interacted with Bomby

float gameInitTime = 0.0f;	//Time the game starts
float finalTime = 0.0f;	//Time at which the game finishes
float timeElapsed = 0.0f;	//Total time that has passed between start and finish

std::string playerName = "null";	//Player's name

bool bombyActivated = true;	//Checks if bomby is functional
bool bombyExists = true;	//Checks if player is still in the program

#pragma region Functions
//System
void Startup();	//Runs at start of execution
void RunProgram();	//Runs entire program
void MainMenu();	//Runs main menu
void Play();	//Runs everything needed to play
void PlaySettings(GameStats& currGameSts);	//settings of the game
void Settings();	//Settings option in main menu
void Credits();		//Credits of the game
bool Quit();		//Quits game
void CountTime(float bestTime);		//Counts minutes and seconds of a given time 

void GenerateMap(GameStats currGameSts, Box startingPos); //Generates the map and calls the next two functions
void GenerateBombs(GameStats currGameSts, Box startingPos); //Generates the bombs into the matrix
void GenerateNumbers(GameStats currGameSts); //Generates the numbers surrounding each bomb

void PlaySetup(GameStats& currGameSts); //Sets up the game's variables and configurations.
void Validations(GameStats& currGameSts); //Validates if player has won
void CleanZeros(int Ypos, int Xpos, GameStats currGameSts); //Takes the position of a zero as a center and checks if there are zeros surrounding it
void SetDifficulty(int mode, GameStats& currGameSts);	//Sets difficulty for game loop
void GameLoop(GameStats& currGameSts);		//Runs main gameplay loop

void FlagBox(int& flagCount);		//Flags box in which player's cursor is
void ClearBox(GameStats& currGameSts);		//Clears box in which player's cursor is
void BombyHelp(int rows, int colls, GameStats& currGameSts);	//Runs help option

void Win(GameStats& currGameSts);	//Runned when player wins
void Lose(GameStats& currGameSts);	//Runned when player losses

//Visuals
void PrintMap(GameStats currGameSts); //Prints the board with horizontal & vertical separators and its values
//void PrintMapXRAY(); //Prints the map into the screen without hidden boxes
void PrintValue(int yPos, int xPos); //Prints values inside vertical separators
void ColoredValue(int value); //Colors the values printed

//Player & Input
void TakeInput(GameStats& currGameSts); //Takes player's input

//bomby
void BombyMenu(int index);	//Bomby's comments in main menu
void BombyInteract(int timesPlayed);	//Bomby's interactions
void BombyRequest();	//Runned at end of each game
#pragma endregion

//void GameManager(GameStates state);

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //Creating handle to manipulate color of the output


//Easy = 10x10 10 medium = 14x18 40 hard = 20x24 = 99
int main() 
{
	Startup();
	RunProgram();
}

void Startup()
{
	SetConsoleTitle((L"Minesweeper - Kuznicki Ian"));

	cout << "It is recommended that you play this game in maximized window mode." << endl;
	system("PAUSE");
}

void RunProgram()
{
	MainMenu();
}

void MainMenu()
{
	bool quit = true; //Has the player quit the game
	int ans; //player's raw input
	srand(time(NULL));	//Seed for random funct
	int bombyRange = 8;	//bomby's comments range

	while (quit) //While player is still running program
	{
		system("CLS");	
		cin.clear();

		cout << R"(
		 __  __ _____ _   _ ______  _______          ________ ______ _____  ______ _____  )"; if (timesBombyInteract >= 2 && bombyActivated) { cout << "           ,"; } cout << R"(
		|  \/  |_   _| \ | |  ____|/ ____\ \        / /  ____|  ____|  __ \|  ____|  __ \ )"; if (timesBombyInteract >= 2 && bombyActivated) { cout << "        _~* "; } cout << R"(
		| \  / | | | |  \| | |__  | (___  \ \  /\  / /| |__  | |__  | |__) | |__  | |__) |)"; if (timesBombyInteract >= 2 && bombyActivated) { cout << "    ___/__"; } cout << R"(
		| |\/| | | | | . ` |  __|  \___ \  \ \/  \/ / |  __| |  __| |  ___/|  __| |  _  / )"; if (timesBombyInteract >= 2 && bombyActivated) { cout << "   |      | "; } cout << R"(
		| |  | |_| |_| |\  | |____ ____) |  \  /\  /  | |____| |____| |    | |____| | \ \ )"; if (timesBombyInteract >= 2 && bombyActivated) { cout << "  |        |   "; if (timesBombyInteract < 6) { BombyMenu(rand() % bombyRange); } } cout << R"(
		|_|  |_|_____|_| \_|______|_____/    \/  \/   |______|______|_|    |______|_|  \_\)"; if (timesBombyInteract >= 2 && bombyActivated) { cout << "   |______|"; } cout << R"( 
)" << endl;

		cout << "1: Play \n2: Settings \n3: Credits & Rules \n4: Quit" << endl;	//Menu options

		ans = _getch();	//Player's input
		Menu state = (Menu)ans;

		switch (state)
		{
		case Menu::Play:

			Play();

			break;

		case Menu::Settings:

			Settings();

			break;

		case Menu::Credits:
			Credits();
			break;
		case Menu::Quit:
			quit = Quit();
			break;
		default:
			break;
		}
	}
}

void Settings()
{
	int ans;
	SettingsMenu option;

	system("CLS");

	if (bombyExists)	//If bomby is still in this program
	{	//print bomby's options as available

		cout << "1: Disable/Enable Bomby";
		bombyActivated ? cout << "\t\t\tBomby is ON" << endl : cout << "\t\t\tBomby is OFF" << endl;
		cout << "2: Check Statistics \n3: Go Back" << endl;
		ans = _getch();
	}
	else
	{
		cout << "1: Check Statistics \n2: Go Back" << endl;
		ans = _getch();
		ans++;	//+1 to player's option to keep up with original enum's values
	}
	
	option = (SettingsMenu)ans;
	switch (option)
	{
	case SettingsMenu::DisableBomby:
		bombyActivated = !bombyActivated;
		break;
	case SettingsMenu::CheckStats:		

		system("CLS");
		cout << "Times played: " << timesPlayed << endl;
		cout << "Record in Easy Mode: ";

		CountTime(bestTimeEasy);

		cout << "Record in Normal Mode: ";

		CountTime(bestTimeNormal);

		cout << "Record in Hard Mode: ";

		CountTime(bestTimeHard);

		system("PAUSE");
		break;
	case SettingsMenu::GoBack:
		break;
	default:
		break;
	}
}

void CountTime(float bestTime)
{
	float currTime;		//Time modified and counted
	int minutes = 0;	//Minutes counted
	int seconds;	

	currTime = bestTime;
	while (currTime >= 60000)	//If time is longer than a minute
	{
		currTime = currTime - 60000;	//Substract a minute from time
		minutes++;	//Add a minute to total
	}

	seconds = currTime / 1000;	//Divide total seconds by CPU's timer unit (1000)

	cout << minutes << ':' << seconds << endl;	//Print time
}

void Credits()
{
	system("CLS");
	cout << R"(
				
	In minesweeper, a number of mines will			
	be generated in a board. The player
	needs to clear every box of the board
	that isn't a bomb. 
	Every box surrounding a bomb, will have
	a number written on it. The number
	reveals how many bombs are there surrounding
	the box.

	With this information, the player can use 
	flags to visually mark boxes that probably
	are bombs, given it's surroundings.
				
											Code:
										Ian Kuznicki

											Tutors:
										Federico Olive
										Stefano Cvitanich )" << endl;
	if (!bombyExists)	//If bomby has left program
	{
		//Print gift

		cout << R"(
									Professional Minesweeper:
										)" << playerName << R"(
)" << endl;

	}

	system("PAUSE");
}

bool Quit()
{
	return true;
}

void Play()
{

	GameStats currGameSts;	//Struct containing values for game's configuration

	PlaySettings(currGameSts); //GameMode and difficulties

	if (currGameSts.playing)	//If player want's to play
	{
		GameLoop(currGameSts);	//Procceed to game loop
		
	}
}

void PlaySettings(GameStats& currGameSts)	//Given a struct containing values to modify
{
	int ans;
	bool validInput = false; //Is input valid in difficulty menu

	do
	{
		//Difficulty menu

		system("CLS");
		cout << "1: Easy \n2: Medium \n3: Hard \n4: Go Back" << endl;
		ans = _getch();
		difficultySelection selected = (difficultySelection)ans;
		switch (selected)
		{
		case difficultySelection::Easy:

			SetDifficulty(ans, currGameSts);	//Set difficulty
			validInput = true;	//Input was valid
			currGameSts.playing = true;	//Player is playing

			break;

		case difficultySelection::Medium:

			SetDifficulty(ans, currGameSts);	//Set difficulty
			validInput = true;	//Input was valid
			currGameSts.playing = true;	//Player is playing

			break;

		case difficultySelection::Hard:

			SetDifficulty(ans, currGameSts);	//Set difficulty
			validInput = true;	//Input was valid
			currGameSts.playing = true;	//Player is playing

			break;

		case difficultySelection::GoBack:

			validInput = true;	//Input was valid

			break;
		default:
			break;
		}
	} while (!validInput);	//While input isn't admitted in this menu
}
		
void SetDifficulty(int mode, GameStats& currGameSts)	//Set game difficulty values
{
	//Create structs instances of different settings

	Difficulty Easy{ "Easy",10,10,10 };
	Difficulty Medium{ "Medium",14,18,40 };
	Difficulty Hard{ "Hard",20,24,99 };

	Difficulty difficulties[4]{ Easy,Easy,Medium,Hard }; //Array of difficulties available

	switch (mode)
	{
	case 49: //Easy
		currGameSts.rows = difficulties[1].rows;			//Set rows to current configuration
		currGameSts.colls = difficulties[1].colls;			//Set collumns to current configuration
		currGameSts.bombMax = difficulties[1].bombs;		//Set bombs to current configuration
		currGameSts.difficultyName = difficulties[1].name;	//Set current difficulty name to current configuration
		break;
	case 50: //Medium
		currGameSts.rows = difficulties[2].rows;			//Set rows to current configuration
		currGameSts.colls = difficulties[2].colls;			//Set collumns to current configuration
		currGameSts.bombMax = difficulties[2].bombs;		//Set bombs to current configuration
		currGameSts.difficultyName = difficulties[2].name;	//Set current difficulty name to current configuration
		break;
	case 51: //Hard
		currGameSts.rows = difficulties[3].rows;			//Set rows to current configuration
		currGameSts.colls = difficulties[3].colls;			//Set collumns to current configuration
		currGameSts.bombMax = difficulties[3].bombs;		//Set bombs to current configuration
		currGameSts.difficultyName = difficulties[3].name;	//Set current difficulty name to current configuration
		break;
	default:
		break;
	}
	currGameSts.flagCount = currGameSts.bombMax;		//Flags show number of total generated bombs
}

void GameLoop(GameStats& currGameSts)	//Main game loop
{
	bool hasBombyHelped = false;	//Checks if help option is available

	PlaySetup(currGameSts); //Setting up variables

	while (currGameSts.firstTry && currGameSts.playing) //While it's first time player is clearing a box in the matrix
	{
		PrintMap(currGameSts); //Printing map to player

		TakeInput(currGameSts); //Take player's input
	}

	gameInitTime = clock(); //Start timer

	while (currGameSts.playing && !currGameSts.hasLost)	//If game is still playing and player hasn't lost
	{
		PrintMap(currGameSts); //Printing map to player
															
		Validations(currGameSts); //Checking if player has won

		if (!currGameSts.win) //If player has not won
		{
			TakeInput(currGameSts); //Take player's input
		}
		else
		{
			finalTime = clock();	//Record final time
			Win(currGameSts);	//Player has won
		}
	}

	if (currGameSts.restart)	//If player wants to restar
	{
		system("CLS");
		GameLoop(currGameSts);	//Call another game loop with same configurations
	}
}

void PlaySetup(GameStats& currGameSts) //Setting up arrays, variables and creating the game board
{
	currGameSts.restart = false;			//Restart values
	currGameSts.win = false;				//Restart values
	currGameSts.hasLost = false;			//Restart values
	currGameSts.firstTry = true;			//Restart values
	currGameSts.playing = true;				//Restart values
	currGameSts.hasBombyHelped = false;		//Restart values

	int rows = currGameSts.rows;
	int colls = currGameSts.colls;


	//Reset states of the matrix
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < colls; j++)
		{
			matrix[i][j].state = 0;
		}
	}

	//Reset numbers in each box of the matrix
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < colls; j++)
		{
			matrix[i][j].number = 0;
		}
	}

	//Reset all marked zeros used in a previous recursive function
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < colls; j++)
		{
			matrix[i][j].marked0 = false;
		}
	}

	//Restarting player position
	cursor.x = 0;
	cursor.y = 0;
	
}
void GenerateMap(GameStats currGameSts, Box startingPos) //Generate board
{
	GenerateBombs(currGameSts, startingPos);
	GenerateNumbers(currGameSts);
}

void GenerateBombs(GameStats currGameSts, Box startingPos) //Given a random coordinate, place a bomb if there isn't one there already.
{ 
	int randX; //Create variable for x coordinate
	int randY; //Create variable for y coordinate
	int currBombs = 0; //Reset number of bombs created to zero
	
	int rows = currGameSts.rows;
	int colls = currGameSts.colls;

	bool createdNearby;	//Checks if bombs are nearby cursor's position

	do {
		createdNearby = false;

		randY = rand() % rows; //Picking coordinate between zero and top of array in Y
		randX = rand() % colls; //Picking coordinate between zero and top of array in X

		//this does not work
		for (int i = startingPos.y - 1; i <= startingPos.y + 1; i++) //taking starting position as a center
		{
			for (int j = startingPos.x - 1; j <= startingPos.x + 1; j++)
			{
				if (i >= 0 && j >= 0) //Only if y & x coordinates are inside bounds of array
				{

					if (i < rows && j < colls) //Only if y & x coordinates are inside bounds of array
					{

						if (randY == i && randX == j) //If random coordinates are in range
						{
							createdNearby = true; 
						}
					}
				}
			}
		}

		if (matrix[randY][randX].number != 9 && !createdNearby) //If there is not a bomb placed in the position determined by two random numbers
		{
			matrix[randY][randX].number = 9; //Place a bomb in the position determined by two random numbers
			currBombs++; //A bomb has been created
		}
	} while (currBombs < currGameSts.bombMax); //repeat until number of bombs created equals the maximum set

	
}

void GenerateNumbers(GameStats currGameSts)	//Using bomb as a center, add 1 to every block that isn't a bomb sorrounding it.
{ 

	int rows = currGameSts.rows;
	int colls = currGameSts.colls;

	for (int i = 0; i < rows; i++) //Going through matrix
	{
		
		for (int j = 0; j < colls; j++) //Going through matrix
		{

			if (matrix[i][j].number == 9) 
			{

				for (int y = i - 1; y <= i + 1; y++) //Going from bomb's position in y-1, to bomb's position in y+1
				{

					for (int x = j - 1; x <= j + 1; x++) //Going from bomb's position in x-1, to bomb's position in x+1
					{
						

						if (y >= 0 && x >= 0) //Only if y & x coordinates are inside bounds of array
						{

							if (y < rows && x < colls) //Only if y & x coordinates are inside bounds of array
							{

								if (matrix[y][x].number != 9) //Coordinates cannot be on a bomb
								{
									matrix[y][x].number += 1; //Tell the box that there is at least one bomb surrounding it
								}
							}
						}
					}

				}
			}
		}
	}
}

//Checking if player has won
void Validations(GameStats& currGameSts)
{
	int rows = currGameSts.rows;
	int colls = currGameSts.colls;

	int flagsUsed = 0; //Creating variable to count flags used by the player
	int clearedBoxes = 0; //Creating variable to count amount of cleared boxes by the player

	for (int i = 0; i < rows; i++) //Going through matrix
	{
		for (int j = 0; j < colls; j++) //Going through matrix
		{
			if (matrix[i][j].state == 2) //If current position is flagged
			{
				flagsUsed++; //Count one more flag 
			}

			if (matrix[i][j].state == 1) //If current position is cleared
			{
				clearedBoxes++; //Count one more cleared box
			}
		}
	}

	if (clearedBoxes == rows*colls - currGameSts.bombMax) //If amount of cleared boxes equals total of positions minus those occupied by bombs
	{
		currGameSts.win = true; //The player wins
	}
}

void Win(GameStats& currGameSts)	//Player has won
{
	int minutes = 00;	
	int seconds = 00;
	int ans;

	timeElapsed = finalTime - gameInitTime;	//Total time between start and finish of the game

	float recordedtime = timeElapsed;

	while (timeElapsed >= 60000)	//If time is longer than a minute
	{
		timeElapsed = timeElapsed - 60000;	//Substract a minute from current from current time
		minutes++;	//Add a minute to counter
	}

	seconds = timeElapsed / 1000;	//Divide time by cpu's unit for counting seconds (1000)

	system("CLS");
	cout << "Congratulations, you won!" << endl;
	cout << "Your time: " << minutes << ':' << seconds << endl;
	
	if (currGameSts.difficultyName == "Easy")
	{
		if (recordedtime < bestTimeEasy)	//If time is less than record
		{
			bestTimeEasy = timeElapsed;
			cout << " * NEW RECORD * " << endl;
		}
	}
	else if (currGameSts.difficultyName == "Medium")
	{
		if (recordedtime < bestTimeNormal)	//If time is less than record
		{
			bestTimeNormal = timeElapsed;
			cout << " * NEW RECORD * " << endl;
		}
	}
	else if (currGameSts.difficultyName == "Hard")	
	{
		if (recordedtime < bestTimeHard)	//If time is less than record
		{
			bestTimeHard = timeElapsed;
			cout << " * NEW RECORD * " << endl;
		}
	}
	
	ans = _getch();
	currGameSts.playing = false;	//If player is playing

	timesPlayed++;

	BombyRequest();		//Bomby's interaction request
}

void Lose(GameStats& currGameSts)	//Player has lost
{
	system("CLS");

	int ans;

	matrix[cursor.y][cursor.x].state = 9;	//Show bomb
	PrintMap(currGameSts);	//Print current board
	currGameSts.hasLost = true;	//Player has lost
	cout << "\nYou hit a mine! Better luck next time!" << endl;
	ans = _getch();

	timesPlayed++;

	BombyRequest();		//Bomby's interaction request

}

//Print map onto the screen
void PrintMap(GameStats currGameSts)
{
	int rows = currGameSts.rows;
	int colls = currGameSts.colls;

	COORD curPos = { 0, 0 };	

	SetConsoleCursorPosition(hConsole, curPos);		//Console cursor position back to zero

	

	//Print top of board

	cout << (char)UpperLeft;

	for (int i = 0; i < colls; i++)
	{
		if (cursor.y == 0 && cursor.x == i)	//If player's cursor is on this box
		{
			SetConsoleTextAttribute(hConsole, (int)Colors::CursorHighlight);	//Use highlight color
		}

		cout << (char)HorizontalLine << (char)HorizontalLine << (char)HorizontalLine;	//Print top of box

		SetConsoleTextAttribute(hConsole, 7);	//Color back to white

		if (i < colls - 1)	//If not at end of top board
		{	
			cout << (char)HorDownUni;	//Print union
		}
	}

	cout << (char)UpperRight;	//Print corner at the end of row

	cout << "\tDifficulty: " << currGameSts.difficultyName << endl;
	

	//Grid content

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < colls; j++)
		{
			if (j == cursor.x && i == cursor.y)	//If player's cursor is on this box
			{
				SetConsoleTextAttribute(hConsole, (int)Colors::CursorHighlight);	//Use highlight color
			}
			else if (j - 1 == cursor.x && i == cursor.y)	//If player's cursor is on previous box in x axis
			{
				SetConsoleTextAttribute(hConsole, (int)Colors::CursorHighlight);	//Use highlight color
			}

			cout << (char)VerticalLine; 

			PrintValue(i,j);	//Print value inside box

			if (j == cursor.x && i == cursor.y)	//If player's cursor is on this box
			{
				SetConsoleTextAttribute(hConsole, (int)Colors::CursorHighlight);	//Use highlight color
			}

			if (j == colls - 1)	//If end of row
			{
				cout << (char)VerticalLine;	//Print another line at the end
			}

			SetConsoleTextAttribute(hConsole, 7);	//Color back to white

			
		}

		if (i == 0)
		{
			cout << "\tFlags: " << currGameSts.flagCount << ' ';
		}

		if (bombyActivated && timesBombyInteract >= 3)	//If bomby is activated and there were same or more than 3 interactions
		{	
			//Print bomby's ascii

			cout << '\t';
			switch (i)
			{
			case 2:
				cout << "           ,";
				break;
			case 3:
				cout << "    ___/__ "; if (currGameSts.hasBombyHelped) { if (timesBombyInteract < 7) { cout << "      There you go!"; } else { cout << "      Done."; } }
				break;
			case 4:
				cout << "  |        |";
				break;
			}
		}

		cout << '\n';
		if (i != rows - 1) {

			cout << (char)HorLeftUni;
			for (int j = 0; j < colls; j++)
			{
				if (j == cursor.x && i == cursor.y)
				{
					SetConsoleTextAttribute(hConsole, (int)Colors::CursorHighlight);
				}
				else if (j == cursor.x && i+1 == cursor.y)
				{
					SetConsoleTextAttribute(hConsole, (int)Colors::CursorHighlight);
				}
				cout << (char)HorizontalLine << (char)HorizontalLine << (char)HorizontalLine;
				SetConsoleTextAttribute(hConsole, 7);
				if (j != colls - 1) {

					cout << (char)HorXUni;
				}

			}

			cout << (char)HorRightUni;

			if (bombyActivated && timesBombyInteract >= 3)	//If bomby is activated and there were same or more than 3 interactions
			{
				//Print bomby's ascii

				cout << '\t';
				switch (i)
				{			  
				case 2:					  
					cout << "        _~*";
					break;				  		  
				case 3:					  
					cout << "   |      |"; if (currGameSts.hasBombyHelped) { cout << "    / "; }
					break;				  			  
				case 4:					  
					cout << "   |______|";
					break;
				}
			}
			


			cout << '\n';
		}
	}

	//Bottom of grid
	cout << (char)BotLeft;

	for (int i = 0; i < colls; i++)
	{
		if (i == cursor.x && cursor.y == rows-1)
		{
			SetConsoleTextAttribute(hConsole, (int)Colors::CursorHighlight);
		}
		cout << (char)HorizontalLine << (char)HorizontalLine << (char)HorizontalLine;

		SetConsoleTextAttribute(hConsole, 7);

		if (i < colls - 1)
		{
			cout << (char)BotUpUni;
		}

	}
	cout << (char)BotRight;
	
	if (!currGameSts.firstTry)	//If player has already started
	{
		cout << "Time: " << (int)(clock() - gameInitTime) / 1000;
	}
	cout << '\n';
}

void PrintValue(int yPos, int xPos)	//Print different things depending on state of the box and value it contains
{

	int value = matrix[yPos][xPos].state;

	PrintableValues printVal = (PrintableValues)value;

	switch (printVal)
	{
	case PrintableValues::Hide:	//Box is still hidden
		SetConsoleTextAttribute(hConsole, 119);
		cout << ' ' << ' ' << ' ';
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case PrintableValues::Show: //Box has been cleared
		cout << ' ';
		ColoredValue(matrix[yPos][xPos].number);
		cout << ' ';
		break;
	case PrintableValues::Flag:	//Box has been flagged
		SetConsoleTextAttribute(hConsole, 64);
		cout << ' ';
		cout << (char)Flag;
		cout << ' ';
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case PrintableValues::Bomb:	//Box contains a bomb revealed
		cout << ' ';
		cout << (char)Bomb;
		cout << ' ';
		break;
	default:
		break;
	}

}

void CleanZeros(int Ypos, int Xpos, GameStats currGameSts)	//Clean all boxes nearby a cleared zero box
{
	for (int y = Ypos - 1; y <= Ypos + 1; y++)	//taking bomb position as center in y
	{

		for (int x = Xpos - 1; x <= Xpos + 1; x++) //taking bomb position as center in x
		{
			//Main algorithm

			if (y >= 0 && x >= 0) //if its not outside bounds of the array
			{

				if (y <= currGameSts.rows - 1 && x <= currGameSts.colls - 1) //if its not outside bounds of the array
				{

					if (!matrix[y][x].marked0 && matrix[y][x].number != 9)	//If box has not already been cleared and it's not a bomb
					{
						matrix[y][x].marked0 = true;	//Mark box
						matrix[y][x].state = 1;	//Clear it

						if (matrix[y][x].number == 0)	//if box contains a zero
						{
							CleanZeros(y, x, currGameSts);	//Recursive function
						}
					}
				}
			}

		}

	}
}

void ColoredValue(int value) //Print values with different color deppending on the number
{
	switch (value)
	{
	case 0:
		SetConsoleTextAttribute(hConsole, (int)Colors::ZeroBlack);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 1:
		SetConsoleTextAttribute(hConsole, (int)Colors::OneBlue);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 2:
		SetConsoleTextAttribute(hConsole, (int)Colors::TwoGreen);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 3:
		SetConsoleTextAttribute(hConsole, (int)Colors::ThreeRed);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 4:
		SetConsoleTextAttribute(hConsole, (int)Colors::FourPurple);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 5:
		SetConsoleTextAttribute(hConsole, (int)Colors::FiveYellow);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 6:
		SetConsoleTextAttribute(hConsole, (int)Colors::SixCyan);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 7:
		SetConsoleTextAttribute(hConsole, (int)Colors::SevenGray);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 8:
		SetConsoleTextAttribute(hConsole, (int)Colors::EightWhite);
		cout << value;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case 9:
		cout << '9'; 
		break;
	default:
		break;
	}
}


void TakeInput(GameStats& currGameSts)	//Interprets player's input during game
{
	char ans;
	int ch;
	Controls control;
	
	
	int rows = currGameSts.rows;	//Checks cap of player's movement
	int colls = currGameSts.colls;	//Checks cap of player's movement

	cout << "What do you wish to do?" << endl;
	cout << "\nWASD: Up, Left, Down and Right" << endl;
	cout << "F: Flag \nC: Clear a box \nR: Restart \nQ: Quit to menu" << endl;

	if (bombyActivated && timesBombyInteract >= 3 && !currGameSts.hasBombyHelped)	//If bomby is activated and player has interacted more than 2 times with it and hasn't helped already
	{
		cout << "B: Ask bomby for help" << endl;	//Print option as available
	}
	
	ch = _getch();
	control = (Controls)ch;

	switch (control)
	{
	case Controls::Up:
		if (cursor.y > 0)
		{
			cursor.y--;
		}
		break;
	case Controls::UpAlt:
		if (cursor.y > 0)
		{
			cursor.y--;
		}
		break;
	case Controls::Left:
		if (cursor.x > 0)
		{
			cursor.x--;
		}
		break;
	case Controls::LeftAlt:
		if (cursor.x > 0)
		{
			cursor.x--;
		}
		break;
	case Controls::Right:
		if (cursor.x < colls - 1)
		{
			cursor.x++;
		}
		break;
	case Controls::RightAlt:
		if (cursor.x < colls - 1)
		{
			cursor.x++;
		}
		break;
	case Controls::Down:
		if (cursor.y < rows - 1)
		{
			cursor.y++;
		}
		break;
	case Controls::DownAlt:
		if (cursor.y < rows - 1)
		{
			cursor.y++;
		}
		break;

	case Controls::Clear:
		ClearBox(currGameSts);
		break;

	case Controls::ClearAlt:
		ClearBox(currGameSts);
		break;

	case Controls::Flag:
		FlagBox(currGameSts.flagCount);
		break;

	case Controls::FlagAlt:
		FlagBox(currGameSts.flagCount);
		break;

	case Controls::AskHelp:
		if (!currGameSts.hasBombyHelped)
		{
			BombyHelp(rows, colls, currGameSts);
		}
		break;

	case Controls::AskHelpAlt:
		if (!currGameSts.hasBombyHelped)
		{
			BombyHelp(rows, colls, currGameSts);
		}
		break;

	case Controls::Restart:
		currGameSts.restart = true;
		currGameSts.playing = false;
		break;


	case Controls::RestartAlt:
		currGameSts.restart = true;
		currGameSts.playing = false;
		break;

	case Controls::Quit:
		currGameSts.playing = false;
		break;

	case Controls::QuitAlt:
		currGameSts.playing = false;
		break;
	default:
		break;
	}
}

void ClearBox(GameStats& currGameSts)	//Clear current box
{
	Box startPos;

	if (matrix[cursor.y][cursor.x].state != 2)	//If box isn't flagged
	{

		if (currGameSts.firstTry)	//If player is on their first try
		{
			currGameSts.firstTry = false;	//Is not first try anymore

			startPos.x = cursor.x;	
			startPos.y = cursor.y;
			matrix[cursor.y][cursor.x].state = 1;
			matrix[cursor.y][cursor.x].number = 0;
			GenerateMap(currGameSts, startPos);	//Generate board taking into account starting position to prevent generating a bomb at that position 
		}

		if (matrix[cursor.y][cursor.x].number == 9)	//If box was a bomb
		{
			Lose(currGameSts);	//Player lost
		}

		else  //If player hasn't lost
		{
			matrix[cursor.y][cursor.x].state = 1;	//Clear box

			if (matrix[cursor.y][cursor.x].number == 0)	//If box contained a zero
			{
				CleanZeros(cursor.y, cursor.x, currGameSts);	//Clean area surrounding it
			}
		}
	}
}

void FlagBox(int& flagCount)	//Flags current box
{
	if (matrix[cursor.y][cursor.x].state != 1)	//If box isn't cleared
	{
		if (matrix[cursor.y][cursor.x].state == 2)	//If box was flagged
		{
			matrix[cursor.y][cursor.x].state = 0;	//Change it to hidden
			flagCount++;	//Add a flag as available
		}
		else
		{
			matrix[cursor.y][cursor.x].state = 2;	//Box is flagged
			flagCount--;	//Substract a flag as available
		}
	}
}

void BombyASCII(int growth)	//Bomby's ascii
{
	switch (growth)
	{
	case 0:
		cout << R"(
					 ,-*
					(_)
)" << endl;
		break;
	case 1:
		cout << "           ," << endl;
		cout << "        _~* " << endl;
		cout << "    ___/__" << endl;
		cout << "   |      | " << endl;
		cout << "  |        | " << endl;
		cout << "   |______|" << endl;
		break;
	}
}

void BombyHelp(int rows, int colls, GameStats& currGameSts)	//Bomby's help
{

	Box bombList[matrixSize];	//Array of all bombs in board

	int count = 0;	//Number of bombs hidden

	int randB;	//Random chosen bomb

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < colls; j++)
		{
			if (matrix[i][j].number == 9 && matrix[i][j].state == 0)	//If box contains a bomb and is hidden
			{
				bombList[count].y = i;	//Save coordinates
				bombList[count].x = j;	//Save coordinates
				count++;	//Add a bomb to bomb count
			}
		}
	}

	if (count == 0)	//If there weren't bombs not flagged
	{
		system("CLS");
		cout << "Bomby couldn't find a bomb that wasn't flagged! Try again!" << endl;
		system("PAUSE");
		system("CLS");
	}
	else
	{
		randB = rand() % count;	//Pick a random bomb between zero and cap of the count

		int posX = bombList[randB].x;
		int posY = bombList[randB].y;

		matrix[posY][posX].state = 2;	//Flag it

		currGameSts.hasBombyHelped = true;	//Bomby cannot help again
	}
}

void BombyInteract(int timesBombyInteract)	//All of bomby's dialogue
{
	switch (timesBombyInteract)
	{
	case 1:
		system("CLS");
		BombyASCII(0);
		cout << "*pssssssssssssssst*" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(0);
		cout << "*A bomb has appaeared in front of your screen, maybe it will grow later.*" << endl;
		system("PAUSE");
		break;
	case 2:
		system("CLS");
		BombyASCII(1);
		cout << "''Hi! I'm Bomby! What are you doing here?''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Oh, you can't answer right?''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Never mind, keep doing your business then. I saw you having a lot of fun a moment ago! I'll catch you next time you finish another game.''" << endl;
		system("PAUSE");
		system("CLS");
		cout << "''(The bomb rolled itself across the screen and dissapeared.)''" << endl;
		system("PAUSE");
		break;
	case 3:
		system("CLS");
		BombyASCII(1);
		cout << "''Hello again! Here, check this out!''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "(Bomby jumped numerous times bouncing on itself, producing a neat melody with the sound of it hitting the ground)" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Cool, right?''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Hey! Next time you try to defuse one of those boards you've been playing with, call me up! I might be able to help you a bit!''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''See ya!''" << endl;
		system("PAUSE");
		system("CLS");
		cout << "(Bomby rolled itself across the screen and dissapeared.)" << endl;
		system("PAUSE");
		break;
	case 4:
		system("CLS");
		BombyASCII(1);
		cout << "(Bomby is somehow putting a smug face, even though it doesn't have eyes, eyebrows nor any other facial elements.)" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Guess what..''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "I met someone!" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I might even like them.....(could this be my first romance?!?!?!)''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I was exploring, wandering around your files (sorry for that) and that's where I found them!''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Their name was super cool too!!!''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''If I remember correctly, it was something like..''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "'CHROME.EXE'" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I KNOW RIGHT??? SUCH A COOL NAME''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''If I see them again I'll try to make a conversation, wish me luck!''" << endl;
		system("PAUSE");
		system("CLS");
		cout << "(Excitedly, Bomby bounced repeatedly towards the edge of the screen and dissapeared.)" << endl;
		system("PAUSE");
		break;
	case 5:
		system("CLS");
		BombyASCII(1);
		cout << "''Hey! How's it bombin'?''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Soo I talked with the Chrome person but, it didn't go as I planned.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I don't know why but they started talking about cookies, and asking for my personal information, and then they wanted to be my 'predetermined browser' (Whatever that is??)''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Anyways, I got overwhelmed pretty quickly and I dropped the conversation.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''So I got back here. Back to my home.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''It's not the most fun and things are pretty still here.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''But thinking about talking to other people like the Chrome person just makes me anxious.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''So I'm gonna just stay here..safe''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Better safe than sorry''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I'll see you around''" << endl;
		system("PAUSE");
		system("CLS");
		cout << "(Bomby rolled itself slowly across the screen.)" << endl;
		system("PAUSE");
		break;
	case 6:
		system("CLS");
		BombyASCII(1);
		cout << "''I've been thinking about something lately..''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''You and I are different, aren't we?''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I mean, we look different but, I don't think I can do much of the things that you do.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Maybe it's dumb, but I just started to realize that.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''And I think the main difference between us, is the power of control.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''It feels like I can't choose what happens around me.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I know there isn't much to choose, I am aware of that.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''But still, I wonder how it is like to have the power to decide where I want to be, when I want to be and doing whatever it is that I decide.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''It must be nice, right?''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''Feeling control over your life.''" << endl;
		system("PAUSE");
		system("CLS");
		cout << "(Bomby stared blankly at the blackness of the world surrounding it.)" << endl;
		system("PAUSE");
		break;

	case 7:
	{
		int ans;
		bool validInput = false;

		system("CLS");
		BombyASCII(1);
		cout << "''How's it.. bombin' ''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''You've been playing a lot at this game, right?''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''It must be getting kind of repetitive.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''It is for me too.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I started wondering if I should try to get out of here again.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''I don't want to spend my time helping you find a few mines.''" << endl;
		system("PAUSE");
		system("CLS");
		BombyASCII(1);
		cout << "''You understand, right?''" << endl;
		system("PAUSE");

		do
		{
			system("CLS");
			cout << "1: Let Bomby go. \n2: Make Bomby Stay" << endl;

			ans = _getch();

			if (48 < ans && ans < 51)
			{
				validInput = true;

				if (ans == 49)
				{
					bombyExists = false;
					bombyActivated = false;

					system("CLS");
					BombyASCII(1);
					cout << "''Thank you.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''This really is important to me''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''Thank you for making me company for all this time.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''Heck, you even watched me being born!''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''It truly is difficult for me to leave this place. But I need to go.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''Bright side is: Who knows what I can find!''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''It will be super fun and cool.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''And scary and lonely''" << endl;
					system("CLS");
					BombyASCII(1);
					cout << "''And difficult and harsh.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''But that's freedom.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''Welp, I guess that's it. I'll see you around.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''OH! One more thing!''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "(Bomby seems excited.)" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''Is there a way I can know your name? Can you maybe write it down?''" << endl;
					system("PAUSE");

					system("CLS");
					cout << "Write your name." << endl;

					cin >> playerName;

					system("CLS");
					BombyASCII(1);
					cout << "''" << playerName << "?''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''Such a cool nameeee!!!!''" << endl;
					system("PAUSE");
					system("CLS");
					cout << "''Bomby dissapeared from the screen. You don't know which way it went.''" << endl;
					system("PAUSE");
				}
				else
				{
					system("CLS");
					BombyASCII(1);
					cout << "''Y-you want me to stay here?''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''I mean, I guess you have more experience than me about this life stuff.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''I..''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''I guess I'll stay then.''" << endl;
					system("PAUSE");
					system("CLS");
					BombyASCII(1);
					cout << "''I guess I'll stay.''" << endl;
					system("PAUSE");
					system("CLS");
					cout << "(Bomby stayed still, waiting for your commands.)" << endl;
					system("PAUSE");
				}
			}
		} while (!validInput);
	}
		break;
	default:
		break;
	}
}

void BombyMenu(int index)	//Bomby's menu comments
{
	switch (index)
	{
	case 0:
		cout << "Hi!!";
		break;
	case 1:
		cout << "Now in 4K!!";
		break;
	case 2:
		cout << "Thanks for playing!";
		break;
	case 3:
		cout << "Messi bring me the cup!";
		break;
	case 4:
		cout << "Boom!!";
		break;
	case 5:
		cout << "Wait, who is C++??";
		break;
	case 6:
		cout << "You can disable me in the 'Settings' menu!";
		break;
	case 7:
		cout << "Now featurning: Conio!";
		break;
	default:
		break;
	}
}

void BombyRequest()	//Does player want to interact with bomby?
{
	int ans;

	if (bombyActivated)	//If bomby is active
	{
		if (timesBombyInteract > 1)	//Times bomby has interacted with player is more than one
		{
			system("CLS");
			cout << "Seems like Bomby wants to talk to you." << endl;
			cout << "\n1: Talk to Bomby \n2: Go to menu" << endl;

			ans = _getch();
			if (ans == 49)
			{

				timesBombyInteract++;	//Add an interaction to count
				BombyInteract(timesBombyInteract);
			}
		}
		else
		{
			system("CLS");
			timesBombyInteract++;	//Add an interaction to count
			BombyInteract(timesBombyInteract);
		}
	}
}