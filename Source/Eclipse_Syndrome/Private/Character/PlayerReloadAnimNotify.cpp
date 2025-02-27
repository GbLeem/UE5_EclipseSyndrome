#include "Character/PlayerReloadAnimNotify.h"
#include "Character/PlayerCharacter.h"

void UPlayerReloadAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
        if (PlayerCharacter && PlayerCharacter->CurrentWeapon)
        {            
            PlayerCharacter->bIsReloading = false;          
        }
    }
}
