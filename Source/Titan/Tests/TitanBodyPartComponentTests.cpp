#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/TitanBodyPartComponent.h"
#include "Combat/TitanBodyPartSet.h"
#include "Combat/TitanHealthComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "TitanGameplayTags.h"

namespace TitanBodyPartComponentTests
{
	const FName HeadBone = TEXT("head");
	const FName LegBone = TEXT("calf_l");
	const float HeadMultiplier = 2.0f;
	const float LegMultiplier = 0.5f;

	/**
	 * An empty game world with one actor that owns a health component and a body part component.
	 * The body parts are built in code: a head and a leg with different damage multipliers. Uses no project asset.
	 */
	struct FFixture
	{
		explicit FFixture(FAutomationTestBase& Test)
		{
			if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
			{
				WorldWrapper.ForwardErrorMessages(&Test);
				return;
			}

			UWorld* World = WorldWrapper.GetTestWorld();

			// Actors spawned from here on are initialized as in a running game, which calls InitializeComponent
			World->InitializeActorsForPlay(FURL());

			AActor* Owner = World->SpawnActor<AActor>();

			Health = NewObject<UTitanHealthComponent>(Owner);
			Health->RegisterComponent();

			UTitanBodyPartSet* BodyPartSet = NewObject<UTitanBodyPartSet>(Owner);

			FTitanBodyPartEntry& Head = BodyPartSet->BodyParts.AddDefaulted_GetRef();
			Head.BodyPart = TitanGameplayTags::BodyPart_Head;
			Head.Bones = { HeadBone };
			Head.DamageMultiplier = HeadMultiplier;

			FTitanBodyPartEntry& Leg = BodyPartSet->BodyParts.AddDefaulted_GetRef();
			Leg.BodyPart = TitanGameplayTags::BodyPart_Leg_Left;
			Leg.Bones = { LegBone };
			Leg.DamageMultiplier = LegMultiplier;

			BodyParts = NewObject<UTitanBodyPartComponent>(Owner);

			// The body part set is assigned in Blueprint defaults in the game, so there is no setter
			FObjectProperty* BodyPartSetProperty = FindFProperty<FObjectProperty>(UTitanBodyPartComponent::StaticClass(), TEXT("BodyPartSet"));
			BodyPartSetProperty->SetObjectPropertyValue_InContainer(BodyParts, BodyPartSet);

			BodyParts->RegisterComponent();
		}

		/** A hit on the physics body of the bone */
		static FHitResult HitOn(FName BoneName)
		{
			FHitResult Hit;
			Hit.BoneName = BoneName;

			return Hit;
		}

		FTestWorldWrapper WorldWrapper;
		UTitanHealthComponent* Health = nullptr;
		UTitanBodyPartComponent* BodyParts = nullptr;
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanBodyPartDamageIsMultipliedByHitLocationTest, "Titan.BodyPart.DamageIsMultipliedByHitLocation",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanBodyPartDamageIsMultipliedByHitLocationTest::RunTest(const FString& Parameters)
{
	using namespace TitanBodyPartComponentTests;

	const FFixture Fixture(*this);
	if (!TestNotNull(TEXT("Body part component"), Fixture.BodyParts))
	{
		return false;
	}

	UTitanHealthComponent& Health = *Fixture.Health;
	const float MaxHealth = Health.GetMaxHealth();
	const float Damage = MaxHealth * 0.1f;

	Fixture.BodyParts->ApplyDamage(Health, Damage, FFixture::HitOn(HeadBone));
	TestEqual(TEXT("A hit on the head applies the damage times the head's multiplier"),
		Health.GetHealth(), MaxHealth - Damage * HeadMultiplier);

	const float HealthBeforeLegHit = Health.GetHealth();
	Fixture.BodyParts->ApplyDamage(Health, Damage, FFixture::HitOn(LegBone));
	TestEqual(TEXT("A hit on the leg applies the damage times the leg's multiplier"),
		Health.GetHealth(), HealthBeforeLegHit - Damage * LegMultiplier);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanBodyPartHitOutsideBodyPartsTakesNoDamageTest, "Titan.BodyPart.HitOutsideBodyPartsTakesNoDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanBodyPartHitOutsideBodyPartsTakesNoDamageTest::RunTest(const FString& Parameters)
{
	using namespace TitanBodyPartComponentTests;

	const FFixture Fixture(*this);
	if (!TestNotNull(TEXT("Body part component"), Fixture.BodyParts))
	{
		return false;
	}

	UTitanHealthComponent& Health = *Fixture.Health;
	const float MaxHealth = Health.GetMaxHealth();
	const float Damage = MaxHealth * 0.1f;

	Fixture.BodyParts->ApplyDamage(Health, Damage, FFixture::HitOn(TEXT("spine_01")));
	TestEqual(TEXT("A hit on a bone that no body part lists applies no damage"), Health.GetHealth(), MaxHealth);

	Fixture.BodyParts->ApplyDamage(Health, Damage, FFixture::HitOn(NAME_None));
	TestEqual(TEXT("A hit on no bone applies no damage"), Health.GetHealth(), MaxHealth);

	return true;
}

#endif
