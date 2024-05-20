// Fill out your copyright notice in the Description page of Project Settings.


#include "MobMeleeAttackHitAnimNotify.h"

#include "BaseMobType.h"

void UMobMeleeAttackHitAnimNotify::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    
    if (MeshComp == nullptr || MeshComp->GetOwner() == nullptr) return;

    ABaseMobType *Mob = Cast<ABaseMobType>(MeshComp->GetOwner());
    if (Mob == nullptr) return;

    Mob->InflictWeaponDamageOnTarget();
}
