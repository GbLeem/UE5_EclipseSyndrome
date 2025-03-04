#include "Character/PlayerRollAnimNotify.h"
#include "Character/PlayerCharacter.h"

void UPlayerRollAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (PlayerCharacter)
		{
			PlayerCharacter->bIsRolling = false;
		}
	}
}
