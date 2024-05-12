// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponHitAnimNotify.h"

#include "ARPGCharacter.h"

void UWeaponHitAnimNotify::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    
    if (MeshComp == nullptr || MeshComp->GetOwner() == nullptr) return;

    AARPGCharacter *Character = Cast<AARPGCharacter>(MeshComp->GetOwner());
    if (Character == nullptr) return;

    Character->InflictWeaponDamageOnTarget();
}
