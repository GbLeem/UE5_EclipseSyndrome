#include "System/DefaultGameMode.h"

#include "Character/PlayerCharacterController.h"
#include "System/DefaultGameState.h"

ADefaultGameMode::ADefaultGameMode()
{
	PlayerControllerClass = APlayerCharacterController::StaticClass();
	GameStateClass = ADefaultGameState::StaticClass();
}
