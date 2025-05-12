// This C++ code simulates a two-player board game where players alternate turns to place colored pieces on a 3x3 grid, 
// aiming to align three of their pieces in a row, column, or diagonal to win. The `Piece` class defines the pieces, the 
// `Board` class manages the game grid and updates it with player moves, and the `Game` class controls the gameplay, tracks 
// player turns, and checks for a win or tie. The game ends when a player wins or all pieces are used, resulting in a tie.
// This game now allows users to undo the previous players turn and let it go back to their turn. 

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

// forward declaration of game class
class Game;

// class to represent a game piece
class Piece {
public:
    // constructor to initialize color and size of the piece
    Piece(char color, char size) : color(color), size(size) {}

    // method to get color of the piece
    char getColor() const { return color; }
    // method to get size of the piece
    char getSize() const { return size; }

private:
    char color; // color of the piece
    char size;  // size of the piece
};

// class to represent the game board
class Board {
    friend class Game; // allow game class to access private members
public:
    // constructor to initialize the grid and numValue
    Board() {
        // initialize grid with empty pieces
        grid = {
            {Piece(' ', ' '), Piece(' ', ' '), Piece(' ', ' ')},
            {Piece(' ', ' '), Piece(' ', ' '), Piece(' ', ' ')},
            {Piece(' ', ' '), Piece(' ', ' '), Piece(' ', ' ')}
        };

        // initialize numValue to 0 for each grid point
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                numValue[i][j] = 0;
            }
        }
    }

    // method to display the board
    void display() {
        int num = 1; // number to display on the grid
        cout << "" << endl << endl;
        for (int i = 0; i < 3; ++i) {
            cout << "             ";
            for (int j = 0; j < 3; ++j) {
                // display the number or the piece on the grid
                if (grid[i][j].getColor() == ' ') {
                    cout << " " << num;
                } else {
                    cout << grid[i][j].getColor() << grid[i][j].getSize();
                }
                // increment the number for the next position
                ++num;
                if (j < 2) cout << "|";
            }
            cout << endl;
            if (i < 2) cout << "             " << "--------" << endl;
        }
    }

    // method to update the board with the player's move
    void updateBoard(char letter, char number, char pieceColor) {
        // calculate row and column indices from the letter and number
        int numIndex, rowIndex;
        switch (number) {
            case '1': case '4': case '7':
                numIndex = 0;
                break;
            case '2': case '5': case '8':
                numIndex = 1;
                break;
            case '3': case '6': case '9':
                numIndex = 2;
                break;
        }

        switch (number) {
            case '1': case '2': case '3':
                rowIndex = 0;
                break;
            case '4': case '5': case '6':
                rowIndex = 1;
                break;
            case '7': case '8': case '9':
                rowIndex = 2;
                break;
        }

        // update numValue based on the letter
        switch (letter) {
            case 'a':
                numValue[rowIndex][numIndex] = 3;
                break;
            case 'b':
                numValue[rowIndex][numIndex] = 2;
                break;
            case 'c':
                numValue[rowIndex][numIndex] = 1;
                break;
        }

        // update the grid with the player's move based on the column and piece color
        if (letter == 'a') {
            if (pieceColor == 'y') {
                grid[rowIndex][numIndex] = Piece('Y', 'Y');
            } else {
                grid[rowIndex][numIndex] = Piece('R', 'R');
            }
        } else if (letter == 'b') {
            if (pieceColor == 'y') {
                grid[rowIndex][numIndex] = Piece('Y', number);
            } else {
                grid[rowIndex][numIndex] = Piece('R', number);
            }
        } else if (letter == 'c') {
            if (pieceColor == 'y') {
                grid[rowIndex][numIndex] = Piece('y', number);
            } else {
                grid[rowIndex][numIndex] = Piece('r', number);
            }
        }
    }

private:
    vector<vector<Piece>> grid; // 2d vector to store pieces
    int numValue[3][3];         // 2d array to store numValue at each grid point
};

// Structure to represent the state of the game
struct GameState {
    Board board;
    char currentPlayer;
    int yellowLargeCount, yellowMediumCount, yellowSmallCount;
    int redLargeCount, redMediumCount, redSmallCount;
    GameState* prev;

    // constructor to initialize game state
    GameState(Board b, char cp, int ylc, int ymc, int ysc, int rlc, int rmc, int rsc, GameState* p)
    : board(b), currentPlayer(cp), yellowLargeCount(ylc), yellowMediumCount(ymc), yellowSmallCount(ysc),
      redLargeCount(rlc), redMediumCount(rmc), redSmallCount(rsc), prev(p) {}
};

// class to manage the game
class Game {
public:
    // constructor to initialize game
    Game() : currentPlayer('y'), history(nullptr) {
        pushState(); // push initial state
    }

    // destructor to clean up memory
    ~Game() {
        while (history != nullptr) {
            GameState* temp = history;
            history = history->prev;
            delete temp;
        }
    }

    // method to start the game
    void start() {
        currentPlayer = 'y'; // start with yellow player
        board.display();     // display initial board
        // game loop
        while (!isGameOver()) {
            printRemainingPieces(); // print remaining pieces for the current player
            promptForMove();        // prompt the current player for their move
            board.display();        // display updated board after each turn

            if (isGameOver()) {
                return; // end the game if it's over
            }
            currentPlayer = (currentPlayer == 'y') ? 'r' : 'y'; // switch to the other player for the next turn
        }
    }

private:
    Board board; // game board
    char currentPlayer; // current player indicator
    int yellowLargeCount = 2; // remaining large pieces for yellow
    int yellowMediumCount = 2; // remaining medium pieces for yellow
    int yellowSmallCount = 2; // remaining small pieces for yellow
    int redLargeCount = 2; // remaining large pieces for red
    int redMediumCount = 2; // remaining medium pieces for red
    int redSmallCount = 2; // remaining small pieces for red
    GameState* history; // pointer to track game states

    // method to push the current game state to history
    void pushState() {
        history = new GameState(board, currentPlayer, yellowLargeCount, yellowMediumCount, yellowSmallCount,
                                redLargeCount, redMediumCount, redSmallCount, history);
    }

    // method to handle undoing the last move
    void undo() {
        if (history && history->prev) {
            GameState* oldState = history;
            history = history->prev;
            board = history->board;
            yellowLargeCount = history->yellowLargeCount;
            yellowMediumCount = history->yellowMediumCount;
            yellowSmallCount = history->yellowSmallCount;
            redLargeCount = history->redLargeCount;
            redMediumCount = history->redMediumCount;
            redSmallCount = history->redSmallCount;

            delete oldState;
        } else {
            cout << "Cannot undo." << endl;
            promptForMove(); // ask for move again if undo not possible
        }
    }

    // method to print remaining pieces for the current player
    void printRemainingPieces() {
        cout << "" << endl;
        if (currentPlayer == 'y') {
            cout << "a. YY  " << yellowLargeCount << " remain." << endl;
            cout << "b. Y   " << yellowMediumCount << " remain." << endl;
            cout << "c. y   " << yellowSmallCount << " remain." << endl;
        } else if (currentPlayer == 'r') {
            cout << "a. RR  " << redLargeCount << " remain." << endl;
            cout << "b. R   " << redMediumCount << " remain." << endl;
            cout << "c. r   " << redSmallCount << " remain." << endl;
        }
        cout << "q to exit." << endl;
    }

    // method to prompt the current player for their move
    void promptForMove() {
        string option;
        cout << "\nIt is " << (currentPlayer == 'y' ? "yellow" : "red") << "'s turn." << endl;
        cout << "Choose action and location, for example a2: " << endl;
        cin >> option;

        char letter = option[0];
        char number = option[1];

        if (option == "u") {
            undo(); // handle undo if requested
            return;
        }

        while (!isValidMove(option)) {
            cout << "Invalid move. Try again." << endl;
            cout << "\nIt is " << (currentPlayer == 'y' ? "yellow" : "red") << "'s turn." << endl;
            cout << "Choose action and location, for example a2: " << endl;
            cin >> option;
            letter = option[0];
            number = option[1];
        }

        board.updateBoard(letter, number, currentPlayer); // update board with player's move
        updateRemainingPieces(letter); // update remaining pieces count
        pushState(); // push current state to history
    }

    // method to update remaining pieces count after a move
    void updateRemainingPieces(char letter) {
        if (currentPlayer == 'y') {
            if (letter == 'a') {
                yellowLargeCount--;
            } else if (letter == 'b') {
                yellowMediumCount--;
            } else if (letter == 'c') {
                yellowSmallCount--;
            }
        } else if (currentPlayer == 'r') {
            if (letter == 'a') {
                redLargeCount--;
            } else if (letter == 'b') {
                redMediumCount--;
            } else if (letter == 'c') {
                redSmallCount--;
            }
        }
    }

    // method to check if the game is over (win or tie)
    bool isGameOver() {
        char currentPlayerColor = (currentPlayer == 'y') ? 'Y' : 'R';

        // check rows and columns for a win
        for (int i = 0; i < 3; ++i) {
            if ((board.grid[i][0].getColor() == currentPlayerColor &&
                board.grid[i][1].getColor() == currentPlayerColor &&
                board.grid[i][2].getColor() == currentPlayerColor) ||
                (board.grid[0][i].getColor() == currentPlayerColor &&
                board.grid[1][i].getColor() == currentPlayerColor &&
                board.grid[2][i].getColor() == currentPlayerColor)) {
                cout << (currentPlayer == 'y' ? "Yellow" : "Red") << " wins!" << endl;
                return true;
            }
        }

        // check diagonals for a win with case insensitivity for small pieces
        if ((toupper(board.grid[0][0].getColor()) == toupper(currentPlayerColor) &&
            toupper(board.grid[1][1].getColor()) == toupper(currentPlayerColor) &&
            toupper(board.grid[2][2].getColor()) == toupper(currentPlayerColor)) ||
            (toupper(board.grid[0][2].getColor()) == toupper(currentPlayerColor) &&
            toupper(board.grid[1][1].getColor()) == toupper(currentPlayerColor) &&
            toupper(board.grid[2][0].getColor()) == toupper(currentPlayerColor))) {
            cout << (currentPlayer == 'y' ? "Yellow" : "Red") << " wins!" << endl;
            return true;
        }

        // check for a tie
        bool allMovesUsed = true;
        if (yellowLargeCount > 0 || yellowMediumCount > 0 || yellowSmallCount > 0 ||
            redLargeCount > 0 || redMediumCount > 0 || redSmallCount > 0) {
            allMovesUsed = false;
        }

        if (allMovesUsed) {
            cout << "Tie game." << endl;
            return true;
        }

        return false;
    }

    // method to validate the move format
    bool isValidMove(const string& option) {
        char letter = option[0];
        char number = option[1];

        if (option == "u") {
            undo(); // handle undo
            return true;
        }

        if (letter == 'q') {
            exit(0);
        } else if ((letter != 'a' && letter != 'b' && letter != 'c') || (number < '1' || number > '9')) {
            return false;
        } else if (!(isdigit(number) && isalpha(letter))) {
            return false;
        } else if ((currentPlayer == 'y') && ((yellowLargeCount == 0 && letter == 'a') || (yellowMediumCount == 0 && letter == 'b') || (yellowSmallCount == 0 && letter == 'c'))) {
            return false;
        } else if ((currentPlayer == 'r') && ((redLargeCount == 0 && letter == 'a') || (redMediumCount == 0 && letter == 'b') || (redSmallCount == 0 && letter == 'c'))) {
            return false;
        } else if (option.length() != 2) {
            return false; 
        } else {
            // check if the move violates the size constraint
            int numIndex, rowIndex;
            switch (number) {
                case '1': case '4': case '7':
                    numIndex = 0;
                    break;
                case '2': case '5': case '8':
                    numIndex = 1;
                    break;
                case '3': case '6': case '9':
                    numIndex = 2;
                    break;
            }

            switch (number) {
                case '1': case '2': case '3':
                    rowIndex = 0;
                    break;
                case '4': case '5': case '6':
                    rowIndex = 1;
                    break;
                case '7': case '8': case '9':
                    rowIndex = 2;
                    break;
            }

            int size = 0;
            switch (letter) {
                case 'a':
                    size = 3;
                    break;
                case 'b':
                    size = 2;
                    break;
                case 'c':
                    size = 1;
                    break;
            }

            // check if the move violates the size constraint
            if (size <= board.numValue[rowIndex][numIndex]) {
                return false;
            }

            return true;
        }
    }
};

// main function
int main() {
    Game game;
    game.start(); // start the game

    return 0;
}