#include "Characters/TitanBossCharacter.h"

#include "Combat/TitanBodyPartSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/HitResult.h"
#include "TitanCollisionChannels.h"

ATitanBossCharacter::ATitanBossCharacter()
{
	// Weapon traces must pass the capsule to reach the mesh's physics bodies, which carry the bone names.
	// The mesh already blocks the Weapon channel through its CharacterMesh profile
	GetCapsuleComponent()->SetCollisionResponseToChannel(TitanTraceChannel_Weapon, ECR_Ignore);

	// Keep the physics bodies on the bones even while the mesh is not rendered
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

FGameplayTag ATitanBossCharacter::GetHitLocation(const FHitResult& Hit) const
{
	if (!BodyPartSet || Hit.GetComponent() != GetMesh())
	{
		return FGameplayTag();
	}

	return BodyPartSet->FindBodyPart(Hit.BoneName);
}
