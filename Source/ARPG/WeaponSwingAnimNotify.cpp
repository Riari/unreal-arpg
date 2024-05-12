// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSwingAnimNotify.h"

#include "ARPGCharacter.h"

void UWeaponSwingAnimNotify::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    
    if (MeshComp == nullptr || MeshComp->GetOwner() == nullptr) return;

    AARPGCharacter *Character = Cast<AARPGCharacter>(MeshComp->GetOwner());
    if (Character == nullptr) return;

    Character->PlayWeaponSwingSound();
}
