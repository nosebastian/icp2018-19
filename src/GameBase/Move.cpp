#include "Move.h"
#include <iostream>

typedef enum {
    MoveState_FirstPositon1, 
    MoveState_Figure, 
    MoveState_Take,
    MoveState_Change, 
    MoveState_FirstPositon2, 
    MoveState_SecondPosition1,
    MoveState_SecondPosition2,
    MoveState_SecondPosition3,
    MoveState_End,
    MoveState_Error
} MoveFsmStates_t;

void Move::SaveMove(std::ostream & outputFile)
{
    if(FigureType != PAWN && FigureType != NONE)
        outputFile << FigureTypeToChar(FigureType);
    
    if (Take)
        outputFile << TAKE_CHAR;
    if(UseFrom)
        outputFile << ((char)(From.Coulumn - 1 + 'a')) << ((char)(From.Row - 1 + '1')); 
    
    outputFile << ((char)(To.Coulumn - 1 + 'a')) << ((char)(To.Row - 1 + '1'));

    if(ChangeTo != NONE)
        outputFile << FigureTypeToChar(FigureType);
    
    if(Check)
        outputFile << CHECK_CHAR;
    if(Checkmate)
        outputFile << CHECKMATE_CHAR;
}

Move::Move(FigureType_t figureType, const Position & to)
{
    FigureType = figureType;
    To = Position(to);
    ValidMove = true;
    UseFrom = false;
    Checkmate = false;
    Check = false;
    Take = false;
}

Move::Move(std::string & inputMove) 
{
    /**
     * Definitions of final states of FSM
    */
    const MoveFsmStates_t FinalStates[] = {
        MoveState_SecondPosition1,
        MoveState_SecondPosition3,
        MoveState_Change,
        MoveState_End
    };
    /**
     * Limiting variable
    */
    size_t len = inputMove.length();
    /**
     * Initial values definitions
    */
    this->FigureType = PAWN;
    this->ChangeTo = NONE;
    this->ValidMove = true;
    this->UseFrom = false;
    this->Take = false;
    this->Check = false;
    this->Checkmate = false;
    /**
     * Helper variables
    */
    char positionRow;
    char positionCoulumn;
    /**
     * Main FSM cycle
    */
    MoveFsmStates_t fsmState= MoveState_FirstPositon1;
    for (size_t i = 0; i < len; i++)
    {
        char currentChar = inputMove[i];
        switch (fsmState)
        {
        case MoveState_FirstPositon1:
            if (currentChar == QUEEN_CHAR || 
                currentChar == ROOK_CHAR || 
                currentChar == KING_CHAR || 
                currentChar == BISHOP_CHAR ||
                currentChar == KNIGHT_CHAR)
            {
                fsmState = MoveState_Figure;
                FigureType = FigureTypeCharToType(currentChar);
            }
            else if (currentChar == TAKE_CHAR)
            {
                fsmState = MoveState_Take;
                Take = true;
                FigureType = PAWN;
            }
            else if (currentChar >= 'a' && currentChar <= 'h')
            {
                fsmState = MoveState_FirstPositon2;
                positionCoulumn = currentChar;
                FigureType = PAWN;
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_Figure:
            if (currentChar == TAKE_CHAR)
            {
                fsmState = MoveState_Take;
                Take = true;
            }
            else if (currentChar >= 'a' && currentChar <= 'h')
            {
                fsmState = MoveState_FirstPositon2;
                positionCoulumn = currentChar;
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_Take:
            if (currentChar >= 'a' && currentChar <= 'h')
            {
                fsmState = MoveState_FirstPositon2;
                positionCoulumn = currentChar;
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_FirstPositon2:
            if (currentChar >= '1' && currentChar <= '8')
            {
                fsmState = MoveState_SecondPosition1;
                positionRow = currentChar;
                this->To = Position(positionRow, positionCoulumn);
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_SecondPosition1:
            if (currentChar >= 'a' && currentChar <= 'h')
            {
                fsmState = MoveState_SecondPosition2;
                positionCoulumn = currentChar;
            }
            else if(currentChar == CHECKMATE_CHAR)
            {
                fsmState = MoveState_End;
                Checkmate = true;
            }
            else if(currentChar == CHECK_CHAR)
            {
                fsmState = MoveState_End;
                Check = true;
            }
            else if (currentChar == QUEEN_CHAR || 
                currentChar == ROOK_CHAR || 
                currentChar == BISHOP_CHAR ||
                currentChar == KNIGHT_CHAR)
            {
                fsmState = MoveState_Change;
                ChangeTo = FigureTypeCharToType(currentChar);;
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_SecondPosition2:
            if (currentChar >= '1' && currentChar <= '8')
            {
                fsmState = MoveState_SecondPosition3;
                positionRow = currentChar;
                this->UseFrom = true;
                this->From = this->To;
                this->To = Position(positionRow, positionCoulumn);
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_SecondPosition3:
            if (currentChar == CHECK_CHAR)
            {
                fsmState = MoveState_End;
                this->Check = true;
            }
            else if (currentChar == CHECKMATE_CHAR)
            {
                fsmState = MoveState_End;
                this->Checkmate = true;
            }
            else if (currentChar == QUEEN_CHAR || 
                currentChar == ROOK_CHAR || 
                currentChar == BISHOP_CHAR ||
                currentChar == KNIGHT_CHAR)
            {
                fsmState = MoveState_Change;
                this->ChangeTo = FigureTypeCharToType(currentChar);
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_Change:
            if (currentChar == CHECK_CHAR)
            {
                fsmState = MoveState_End;
                this->Check = true;
            }
            else if (currentChar == CHECKMATE_CHAR)
            {
                fsmState = MoveState_End;
                this->Checkmate = true;
            }
            else
            {
                fsmState = MoveState_Error;
            }
            break;
        case MoveState_End:
            fsmState = MoveState_Error;
            break;
        default:
            break;
        }
    }
    /**
     * Check if state is in final states of FSM
    */
    bool isFinal = false;
    for (size_t i = 0; i < sizeof(FinalStates)/sizeof(MoveFsmStates_t); i++)
    {
        if(fsmState == FinalStates[i])
        {
            isFinal = true;
            break;
        }
    }
    /**
     * Set error if FSM is in error state
    */
    if(fsmState == MoveState_Error || !isFinal)
    {
        this->ValidMove = false;
    }
}