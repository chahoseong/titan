#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/TitanBossCharacter.h"
#include "Combat/TitanBodyPartSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "Tests/AutomationCommon.h"
#include "TitanCollisionChannels.h"
#include "TitanGameplayTags.h"

namespace TitanHitLocationTests
{
	// Lives in a git-ignored folder, so it is missing on a fresh clone
	const TCHAR* const BossMeshPackage = TEXT("/Game/Synty/PolygonMech/Models/Mech/SK_PolygonMec_Main");
	const TCHAR* const BossClassPath = TEXT("/Game/Titan/Characters/Boss/BP_BossCharacter.BP_BossCharacter_C");

	/** Returns null after reporting why: a warning if the boss mesh is missing, an error for anything else */
	UClass* LoadBossClass(FAutomationTestBase& Test)
	{
		if (!FPackageName::DoesPackageExist(BossMeshPackage))
		{
			Test.AddWarning(FString::Printf(TEXT("Skipped: boss mesh %s is not in this checkout."), BossMeshPackage));
			return nullptr;
		}

		UClass* BossClass = LoadClass<ATitanBossCharacter>(nullptr, BossClassPath);
		if (!BossClass)
		{
			Test.AddError(FString::Printf(TEXT("Could not load %s."), BossClassPath));
		}

		return BossClass;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHitLocationCoverageTest, "Titan.HitLocation.Coverage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHitLocationCoverageTest::RunTest(const FString& Parameters)
{
	const UClass* BossClass = TitanHitLocationTests::LoadBossClass(*this);
	if (!BossClass)
	{
		return !HasAnyErrors();
	}

	const ATitanBossCharacter* Boss = BossClass->GetDefaultObject<ATitanBossCharacter>();
	const UTitanBodyPartSet* BodyPartSet = Boss->GetBodyPartSet();
	const UPhysicsAsset* PhysicsAsset = Boss->GetMesh()->GetPhysicsAsset();
	if (!TestNotNull(TEXT("Boss has a body part set"), BodyPartSet) || !TestNotNull(TEXT("Boss mesh has a physics asset"), PhysicsAsset))
	{
		return false;
	}

	// A hit on any body must resolve to exactly one body part
	TSet<FName> BodyBones;
	for (const USkeletalBodySetup* BodySetup : PhysicsAsset->SkeletalBodySetups)
	{
		BodyBones.Add(BodySetup->BoneName);

		int32 OwnerCount = 0;
		for (const FTitanBodyPartEntry& Entry : BodyPartSet->BodyParts)
		{
			OwnerCount += Entry.Bones.Contains(BodySetup->BoneName) ? 1 : 0;
		}

		if (OwnerCount != 1)
		{
			AddError(FString::Printf(TEXT("Body on bone %s belongs to %d body parts, expected 1."), *BodySetup->BoneName.ToString(), OwnerCount));
		}
	}

	// A listed bone without a body can never be hit, so it is a stale or misspelled name
	for (const FTitanBodyPartEntry& Entry : BodyPartSet->BodyParts)
	{
		for (const FName& Bone : Entry.Bones)
		{
			if (!BodyBones.Contains(Bone))
			{
				AddError(FString::Printf(TEXT("Bone %s in body part %s has no physics body."), *Bone.ToString(), *Entry.BodyPart.ToString()));
			}
		}
	}

	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHitLocationTraceTest, "Titan.HitLocation.Trace",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHitLocationTraceTest::RunTest(const FString& Parameters)
{
	UClass* BossClass = TitanHitLocationTests::LoadBossClass(*this);
	if (!BossClass)
	{
		return !HasAnyErrors();
	}

	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		WorldWrapper.ForwardErrorMessages(this);
		return false;
	}

	UWorld* World = WorldWrapper.GetTestWorld();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const ATitanBossCharacter* Boss = World->SpawnActor<ATitanBossCharacter>(BossClass, FTransform::Identity, SpawnParameters);
	if (!TestNotNull(TEXT("Boss spawned"), Boss))
	{
		return false;
	}

	struct FDirection
	{
		const TCHAR* Name;
		FVector Vector;
	};

	const FDirection Front = { TEXT("front"), Boss->GetActorForwardVector() };
	const FDirection Back = { TEXT("back"), -Boss->GetActorForwardVector() };
	const FDirection Above = { TEXT("above"), FVector::UpVector };

	struct FTarget
	{
		FName Bone;
		FGameplayTag BodyPart;
		FDirection Directions[2];
	};

	// One bone with a physics body per body part, shot from two sides the body part is exposed on.
	// The torso armor covers the head from behind, so the head is shot from above instead
	const FTarget Targets[] =
	{
		{ TEXT("head"), TitanGameplayTags::BodyPart_Head, { Front, Above } },
		{ TEXT("spine_01"), TitanGameplayTags::BodyPart_Torso, { Front, Back } },
		{ TEXT("lowerarm_l"), TitanGameplayTags::BodyPart_Arm_Left, { Front, Back } },
		{ TEXT("lowerarm_r"), TitanGameplayTags::BodyPart_Arm_Right, { Front, Back } },
		{ TEXT("calf_l"), TitanGameplayTags::BodyPart_Leg_Left, { Front, Back } },
		{ TEXT("calf_r"), TitanGameplayTags::BodyPart_Leg_Right, { Front, Back } },
	};

	const float TraceDistance = 2000.0f;

	for (const FTarget& Target : Targets)
	{
		const FBodyInstance* Body = Boss->GetMesh()->GetBodyInstance(Target.Bone);
		if (!Body || !Body->IsValidBodyInstance())
		{
			AddError(FString::Printf(TEXT("No physics body on bone %s."), *Target.Bone.ToString()));
			continue;
		}

		// Shoot at the middle of the body
		const FVector BodyCenter = Body->GetBodyBounds().GetCenter();
		for (const FDirection& Direction : Target.Directions)
		{
			FHitResult Hit;
			const bool bHit = World->LineTraceSingleByChannel(Hit, BodyCenter + Direction.Vector * TraceDistance, BodyCenter, TitanTraceChannel_Weapon);
			const FGameplayTag HitLocation = bHit ? Boss->GetHitLocation(Hit) : FGameplayTag();

			if (HitLocation != Target.BodyPart)
			{
				AddError(FString::Printf(TEXT("Trace at bone %s from %s: expected %s, got %s (hit component %s, bone %s)."),
					*Target.Bone.ToString(),
					Direction.Name,
					*Target.BodyPart.ToString(),
					*HitLocation.ToString(),
					*GetNameSafe(Hit.GetComponent()),
					*Hit.BoneName.ToString()));
			}
		}
	}

	WorldWrapper.DestroyTestWorld(true);
	WorldWrapper.ForwardErrorMessages(this);

	return !HasAnyErrors();
}

#endif
