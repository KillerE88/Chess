#include "includes.h"
#include "user_interface.h"
#include "Chess.h"
#include "debug.h"

Game* current_game = NULL; //fix from globab

bool isMoveValid(Chess::Position present, Chess::Position future, Chess::EnPassant* S_enPassant, Chess::Castling* S_castling, Chess::Promotion* S_promotion)
{
    bool bValid = false;

    char chPiece = current_game->getPieceAtPosition(present.iRow, present.iColumn);

    switch (toupper(chPiece))
    {
    case 'P':
    {
        // Wants to move forward
        if (future.iColumn == present.iColumn)
        {
            // Simple move forward
            if ((Chess::isWhitePiece(chPiece) && future.iRow == present.iRow + 1) ||
                (Chess::isBlackPiece(chPiece) && future.iRow == present.iRow - 1))
            {
                if (EMPTY_SQUARE == current_game->getPieceAtPosition(future.iRow, future.iColumn))
                {
                    bValid = true;
                }
            }

            // Double move forward
            else if ((Chess::isWhitePiece(chPiece) && future.iRow == present.iRow + 2) ||
                (Chess::isBlackPiece(chPiece) && future.iRow == present.iRow - 2))
            {
                // This is only allowed if the pawn is in its original place
                if (Chess::isWhitePiece(chPiece))
                {
                    if (EMPTY_SQUARE == current_game->getPieceAtPosition(future.iRow - 1, future.iColumn) &&
                        EMPTY_SQUARE == current_game->getPieceAtPosition(future.iRow, future.iColumn) &&
                        1 == present.iRow)
                    {
                        bValid = true;
                    }
                }
                else // if ( isBlackPiece(chPiece) )
                {
                    if (EMPTY_SQUARE == current_game->getPieceAtPosition(future.iRow + 1, future.iColumn) &&
                        EMPTY_SQUARE == current_game->getPieceAtPosition(future.iRow, future.iColumn) &&
                        6 == present.iRow)
                    {
                        bValid = true;
                    }
                }
            }
            else
            {
                // This is invalid
                return false;
            }
        }

        // The "en passant" move
        else if ((Chess::isWhitePiece(chPiece) && 4 == present.iRow && 5 == future.iRow && 1 == abs(future.iColumn - present.iColumn)) ||
            (Chess::isBlackPiece(chPiece) && 3 == present.iRow && 2 == future.iRow && 1 == abs(future.iColumn - present.iColumn)))
        {
            // It is only valid if last move of the opponent was a double move forward by a pawn on a adjacent column
            string last_move = current_game->getLastMove();

            // Parse the line
            Chess::Position LastMoveFrom;
            Chess::Position LastMoveTo;
            current_game->parseMove(last_move, &LastMoveFrom, &LastMoveTo);

            // checking if its a pawn
            char chLstMvPiece = current_game->getPieceAtPosition(LastMoveTo.iRow, LastMoveTo.iColumn);

            if (toupper(chLstMvPiece) != 'P')
            {
                return false;
            }

            // Did the pawn use a double move
            if (2 == abs(LastMoveTo.iRow - LastMoveFrom.iRow) && 1 == abs(LastMoveFrom.iColumn - present.iColumn))
            {
                cout << "En passant move!\n";
                bValid = true;

                S_enPassant->bApplied = true;
                S_enPassant->PawnCaptured.iRow = LastMoveTo.iRow;
                S_enPassant->PawnCaptured.iColumn = LastMoveTo.iColumn;
            }
        }

        // Wants to capture a piece
        else if (1 == abs(future.iColumn - present.iColumn))
        {
            if ((Chess::isWhitePiece(chPiece) && future.iRow == present.iRow + 1) || (Chess::isBlackPiece(chPiece) && future.iRow == present.iRow - 1))
            {
                // Only allowed if there is something to be captured in the square
                if (EMPTY_SQUARE != current_game->getPieceAtPosition(future.iRow, future.iColumn))
                {
                    bValid = true;
                    cout << "Pawn captured a piece!\n";
                }
            }
        }
        else
        {
            // This is invalid
            return false;
        }

        // If a pawn reaches its eight rank, it must be promoted to another piece
        if ((Chess::isWhitePiece(chPiece) && 7 == future.iRow) ||
            (Chess::isBlackPiece(chPiece) && 0 == future.iRow))
        {
            cout << "Pawn must be promoted!\n";
            S_promotion->bApplied = true;
        }
    }
    break;

    case 'R':
    {
        // Horizontal move
        if ((future.iRow == present.iRow) && (future.iColumn != present.iColumn))
        {
            // Check if there are no pieces on the way
            if (current_game->isPathFree(present, future, Chess::HORIZONTAL))
            {
                bValid = true;
            }
        }
        // Vertical move
        else if ((future.iRow != present.iRow) && (future.iColumn == present.iColumn))
        {
            // Check if there are no pieces on the way
            if (current_game->isPathFree(present, future, Chess::VERTICAL))
            {
                bValid = true;
            }
        }
    }
    break;

    case 'N':
    {
        if ((2 == abs(future.iRow - present.iRow)) && (1 == abs(future.iColumn - present.iColumn)))
        {
            bValid = true;
        }

        else if ((1 == abs(future.iRow - present.iRow)) && (2 == abs(future.iColumn - present.iColumn)))
        {
            bValid = true;
        }
    }
    break;

    case 'B':
    {
        // Diagonal move
        if (abs(future.iRow - present.iRow) == abs(future.iColumn - present.iColumn))
        {
            // Check if there are no pieces on the way
            if (current_game->isPathFree(present, future, Chess::DIAGONAL))
            {
                bValid = true;
            }
        }
    }
    break;

    case 'Q':
    {
        // Horizontal move
        if ((future.iRow == present.iRow) && (future.iColumn != present.iColumn))
        {
            // Check if there are no pieces on the way
            if (current_game->isPathFree(present, future, Chess::HORIZONTAL))
            {
                bValid = true;
            }
        }
        // Vertical move
        else if ((future.iRow != present.iRow) && (future.iColumn == present.iColumn))
        {
            // Check if there are no pieces on the way
            if (current_game->isPathFree(present, future, Chess::VERTICAL))
            {
                bValid = true;
            }
        }

        // Diagonal move
        else if (abs(future.iRow - present.iRow) == abs(future.iColumn - present.iColumn))
        {
            // Check if there are no pieces on the way
            if (current_game->isPathFree(present, future, Chess::DIAGONAL))
            {
                bValid = true;
            }
        }
    }
    break;

    case 'K':
    {
        // Horizontal move by 1
        if ((future.iRow == present.iRow) && (1 == abs(future.iColumn - present.iColumn)))
        {
            bValid = true;
        }

        // Vertical move by 1
        else if ((future.iColumn == present.iColumn) && (1 == abs(future.iRow - present.iRow)))
        {
            bValid = true;
        }

        // Diagonal move by 1
        else if ((1 == abs(future.iRow - present.iRow)) && (1 == abs(future.iColumn - present.iColumn)))
        {
            bValid = true;
        }

        // Castling
        else if ((future.iRow == present.iRow) && (2 == abs(future.iColumn - present.iColumn)))
        {
            // Castling is only allowed in these circunstances:

            // King is not in check
            if (true == current_game->playerKingInCheck())
            {
                return false;
            }

            // No pieces in between the king and the rook
            if (false == current_game->isPathFree(present, future, Chess::HORIZONTAL))
            {
                return false;
            }

            // King and rook must not have moved yet;
            // King must not pass through a square that is attacked by an enemy piece
            if (future.iColumn > present.iColumn)
            {
                // if future.iColumn is greather, it means king side
                if (false == current_game->castlingAllowed(Chess::Side::KING_SIDE, Chess::getPieceColor(chPiece)))
                {
                    createNextMessage("Castling to the king side is not allowed.\n");
                    return false;
                }
                else
                {
                    // Check if the square that the king skips is not under attack
                    Chess::UnderAttack square_skipped = current_game->isUnderAttack(present.iRow, present.iColumn + 1, current_game->getCurrentTurn());
                    if (false == square_skipped.bUnderAttack)
                    {
                        // Fill the S_castling structure
                        S_castling->bApplied = true;

                        // Present position of the rook
                        S_castling->rook_before.iRow = present.iRow;
                        S_castling->rook_before.iColumn = present.iColumn + 3;

                        // Future position of the rook
                        S_castling->rook_after.iRow = future.iRow;
                        S_castling->rook_after.iColumn = present.iColumn + 1; // future.iColumn -1

                        bValid = true;
                    }
                }
            }
            else //if (future.iColumn < present.iColumn)
            {
                // if present.iColumn is greather, it means queen side
                if (false == current_game->castlingAllowed(Chess::Side::QUEEN_SIDE, Chess::getPieceColor(chPiece)))
                {
                    createNextMessage("Castling to the queen side is not allowed.\n");
                    return false;
                }
                else
                {
                    // Check if the square that the king skips is not attacked
                    Chess::UnderAttack square_skipped = current_game->isUnderAttack(present.iRow, present.iColumn - 1, current_game->getCurrentTurn());
                    if (false == square_skipped.bUnderAttack)
                    {
                        // Fill the S_castling structure
                        S_castling->bApplied = true;

                        // Present position of the rook
                        S_castling->rook_before.iRow = present.iRow;
                        S_castling->rook_before.iColumn = present.iColumn - 4;

                        // Future position of the rook
                        S_castling->rook_after.iRow = future.iRow;
                        S_castling->rook_after.iColumn = present.iColumn - 1; // future.iColumn +1

                        bValid = true;
                    }
                }
            }
        }
    }
    break;

    default:
    {
        cout << "!!!!Should not reach here. Invalid piece: " << char(chPiece) << "\n\n\n";
    }
    break;
    }

    // If it is a move in an invalid direction, do not even bother to check the rest
    if (false == bValid)
    {
        cout << "Piece is not allowed to move to that square\n";
        return false;
    }


    // checking to see if square was occupied/taken
    if (current_game->isSquareOccupied(future.iRow, future.iColumn))
    {
        char chAuxPiece = current_game->getPieceAtPosition(future.iRow, future.iColumn);
        if (Chess::getPieceColor(chPiece) == Chess::getPieceColor(chAuxPiece))
        {
            cout << "Position is already taken by a piece of the same color\n";
            return false;
        }
    }

    // checking if the king will be in check with next moves
    if (true == current_game->wouldKingBeInCheck(chPiece, present, future, S_enPassant))
    {
        cout << "Move would put player's king in check\n";
        return false;
    }

    return bValid;
}

void makeTheMove(Chess::Position present, Chess::Position future, Chess::EnPassant* S_enPassant, Chess::Castling* S_castling, Chess::Promotion* S_promotion)
{
    char chPiece = current_game->getPieceAtPosition(present.iRow, present.iColumn);

    // Capturing a piece
    if (current_game->isSquareOccupied(future.iRow, future.iColumn))
    {
        char chAuxPiece = current_game->getPieceAtPosition(future.iRow, future.iColumn);

        if (Chess::getPieceColor(chPiece) != Chess::getPieceColor(chAuxPiece))
        {
            createNextMessage(Chess::describePiece(chAuxPiece) + " captured!\n");
        }
        else
        {
            cout << "Error. We should not be making this move\n";
            throw("Error. We should not be making this move");
        }
    }
    else if (true == S_enPassant->bApplied)
    {
        createNextMessage("Pawn captured by \"en passant\" move!\n");
    }

    if ((true == S_castling->bApplied))
    {
        createNextMessage("Castling applied!\n");
    }

    current_game->movePiece(present, future, S_enPassant, S_castling, S_promotion);
}

// Functions to handle the commands of the program
// New game, Move, Undo, Save game, Load game

void newGame(void)
{
    if (NULL != current_game)
    {
        delete current_game;
    }

    current_game = new Game();
}

void undoMove(void)
{
    if (false == current_game->undoIsPossible())
    {
        createNextMessage("Undo is not possible now!\n");
        return;
    }

    current_game->undoLastMove();
    createNextMessage("Last move was undone\n");
}

void movePiece(void)
{
    std::string to_record;

    // Get user input for the piece they want to move
    cout << "Choose piece to be moved. (example: A1 or b2): ";

    std::string move_from;
    getline(cin, move_from);

    if (move_from.length() > 2)
    {
        createNextMessage("You should type only two characters (column and row)\n");
        return;
    }

    Chess::Position present;
    present.iColumn = move_from[0];
    present.iRow = move_from[1];

    // Checking to see if piece is valid
    // It's inside the board (A1-H8)
    // the piece being moved is with the correct player

    present.iColumn = toupper(present.iColumn);

    if (present.iColumn < 'A' || present.iColumn > 'H')
    {
        createNextMessage("Invalid column.\n");
        return;
    }

    if (present.iRow < '0' || present.iRow > '8')
    {
        createNextMessage("Invalid row.\n");
        return;
    }

    // Logging user input
    to_record += present.iColumn;
    to_record += present.iRow;
    to_record += "-";

    // Convert column from ['A'-'H'] to [0x00-0x07]
    present.iColumn = present.iColumn - 'A';

    // Convert row from ['1'-'8'] to [0x00-0x07]
    present.iRow = present.iRow - '1';

    char chPiece = current_game->getPieceAtPosition(present.iRow, present.iColumn);
    cout << "Piece is " << char(chPiece) << "\n";

    if (0x20 == chPiece)
    {
        createNextMessage("You picked an EMPTY square.\n");
        return;
    }

    if (Chess::WHITE_PIECE == current_game->getCurrentTurn())
    {
        if (false == Chess::isWhitePiece(chPiece))
        {
            createNextMessage("It is WHITE's turn and you picked a BLACK piece\n");
            return;
        }
    }
    else
    {
        if (false == Chess::isBlackPiece(chPiece))
        {
            createNextMessage("It is BLACK's turn and you picked a WHITE piece\n");
            return;
        }
    }

    // Get user input for the square to move to
    cout << "Move to: ";
    std::string move_to;
    getline(cin, move_to);

    if (move_to.length() > 2)
    {
        createNextMessage("You should type only two characters (column and row)\n");
        return;
    }

    // Must check for valid move if:
    // It's inside the board (A1-H8)
    // The move is valid

    Chess::Position future;
    future.iColumn = move_to[0];
    future.iRow = move_to[1];

    future.iColumn = toupper(future.iColumn);

    if (future.iColumn < 'A' || future.iColumn > 'H')
    {
        createNextMessage("Invalid column.\n");
        return;
    }

    if (future.iRow < '0' || future.iRow > '8')
    {
        createNextMessage("Invalid row.\n");
        return;
    }

    // logging user input
    to_record += future.iColumn;
    to_record += future.iRow;

    // Convert columns from ['A'-'H'] to [0x00-0x07]
    future.iColumn = future.iColumn - 'A';

    // Convert row from ['1'-'8'] to [0x00-0x07]
    future.iRow = future.iRow - '1';

    // Making sure the user doesn't type in the same info twice example A1 to A1
    if (future.iRow == present.iRow && future.iColumn == present.iColumn)
    {
        createNextMessage("[Invalid] You picked the same square!\n");
        return;
    }

    // checking for valid move
    Chess::EnPassant  S_enPassant = { 0 };
    Chess::Castling   S_castling = { 0 };
    Chess::Promotion  S_promotion = { 0 };

    if (false == isMoveValid(present, future, &S_enPassant, &S_castling, &S_promotion))
    {
        createNextMessage("[Invalid] Piece can not move to that square!\n");
        return;
    }

    // Promotion: user most choose a piece to
    // replace the pawn

    if (S_promotion.bApplied == true)
    {
        cout << "Promote to (Q, R, N, B): ";
        std::string piece;
        getline(cin, piece);

        if (piece.length() > 1)
        {
            createNextMessage("You should type only one character (Q, R, N or B)\n");
            return;
        }

        char chPromoted = toupper(piece[0]);

        if (chPromoted != 'Q' && chPromoted != 'R' && chPromoted != 'N' && chPromoted != 'B')
        {
            createNextMessage("Invalid character.\n");
            return;
        }

        S_promotion.chBefore = current_game->getPieceAtPosition(present.iRow, present.iColumn);

        if (Chess::WHITE_PLAYER == current_game->getCurrentTurn())
        {
            S_promotion.chAfter = toupper(chPromoted);
        }
        else
        {
            S_promotion.chAfter = tolower(chPromoted);
        }

        to_record += '=';
        to_record += toupper(chPromoted); // always log with a capital letter
    }

    // because we need the getCurrentTurn()
    current_game->logMove(to_record);

 
    // Make the move
    makeTheMove(present, future, &S_enPassant, &S_castling, &S_promotion);

    // Checking if the last move put the oponent's king in check
    if (true == current_game->playerKingInCheck())
    {
        if (true == current_game->isCheckMate())
        {
            if (Chess::WHITE_PLAYER == current_game->getCurrentTurn())
            {
                appendToNextMessage("Checkmate! Black wins the game!\n");
            }
            else
            {
                appendToNextMessage("Checkmate! White wins the game!\n");
            }
        }
        else
        {
            if (Chess::WHITE_PLAYER == current_game->getCurrentTurn())
            {
                appendToNextMessage("White king is in check!\n");
            }
            else
            {
                appendToNextMessage("Black king is in check!\n");
            }
        }
    }

    return;
}

void saveGame(void) //the function I plan on adding
{
    
}

void loadGame(void) // the function I plan on adding
{
   
}

int main()
{
    bool bRun = true;

    // Clear screen an print the logo
    clearScreen();
    printLogo();

    string input = "";

    while (bRun)
    {
        printMessage();
        printMenu();

        // Get input from user
        cout << "Type here: ";
        getline(cin, input);

        if (input.length() != 1)
        {
            cout << "Invalid option. Type one letter only\n\n";
            continue;
        }

        try
        {
            switch (input[0])
            {
            case 'N':
            case 'n':
            {
                newGame();
                clearScreen();
                printLogo();
                printSituation(*current_game);
                printBoard(*current_game);
            }
            break;

            case 'M':
            case 'm':
            {
                if (NULL != current_game)
                {
                    if (current_game->isFinished())
                    {
                        cout << "This game has already finished!\n";
                    }
                    else
                    {
                        movePiece();
                        //clearScreen();
                        printLogo();
                        printSituation(*current_game);
                        printBoard(*current_game);
                    }
                }
                else
                {
                    cout << "No game running!\n";
                }

            }
            break;

            case 'Q':
            case 'q':
            {
                bRun = false;
            }
            break;

            case 'U':
            case 'u':
            {
                if (NULL != current_game)
                {
                    undoMove();
                    //clearScreen();
                    printLogo();
                    printSituation(*current_game);
                    printBoard(*current_game);
                }
                else
                {
                    cout << "No game running\n";
                }
            }
            break;

            case 'S':
            case 's':
            {
                if (NULL != current_game)
                {
                    //saveGame();
                    clearScreen();
                    printLogo();
                    printSituation(*current_game);
                    printBoard(*current_game);
                }
                else
                {
                    cout << "No game running\n";
                }
            }
            break;

            case 'L':
            case 'l':
            {
                //loadGame();
                clearScreen();
                printLogo();
                printSituation(*current_game);
                printBoard(*current_game);
            }
            break;

            default:
            {
                cout << "Option does not exist\n\n";
            }
            break;

            }

        }
        catch (const char* s)
        {
            s;
        }
    }


    return 0;
}
