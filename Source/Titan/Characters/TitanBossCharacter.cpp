#include "Characters/TitanBossCharacter.h"

#include "Combat/TitanBodyPartComponent.h"
#include "Combat/TitanHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TitanCollisionChannels.h"

ATitanBossCharacter::ATitanBossCharacter()
{
	// Weapon traces must pass the capsule to reach the mesh's physics bodies, which carry the bone names.
	// The mesh already blocks the Weapon channel through its CharacterMesh profile
	GetCapsuleComponent()->SetCollisionResponseToChannel(TitanTraceChannel_Weapon, ECR_Ignore);

	// Keep the physics bodies on the bones even while the mesh is not rendered
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	HealthComponent = CreateDefaultSubobject<UTitanHealthComponent>(TEXT("HealthComponent"));
	BodyPartComponent = CreateDefaultSubobject<UTitanBodyPartComponent>(TEXT("BodyPartComponent"));
}

void ATitanBossCharacter::ReceiveDamage(float Damage, const FHitResult& Hit)
{
	BodyPartComponent->ApplyDamage(*HealthComponent, Damage, Hit);
}
