#include "CaptureTheFlagGameState.h"

#include "Kismet/KismetMathLibrary.h"

ACaptureTheFlagGameState::ACaptureTheFlagGameState()
{
    // TODO: Remove magic number, add the possibility of infering the number 
    // of teams in a level by counting how many AColorCheckpoints exist in it.
    TeamPlayers.Init(0, 2);
}

int32 ACaptureTheFlagGameState::GetAvailableTeamIndex()
{
    // Find first smallest team
    int32 IndexOfMinValue, _MinValue;
    UKismetMathLibrary::MinOfIntArray(TeamPlayers, IndexOfMinValue, _MinValue);
    return IndexOfMinValue;
}