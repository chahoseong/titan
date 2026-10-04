#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/TitanHealthComponent.h"
#include "Combat/TitanWeaponComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

namespace TitanWeaponComponentTests
{
	/**
	 * An empty game world with a shooter at the origin that owns a weapon component.
	 * Targets and obstacles are placed in front of the shooter. Uses no project asset.
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

			World = WorldWrapper.GetTestWorld();

			// Actors spawned from here on are initialized as in a running game, which calls InitializeComponent
			World->InitializeActorsForPlay(FURL());

			AActor* Shooter = World->SpawnActor<AActor>();
			Weapon = NewObject<UTitanWeaponComponent>(Shooter);
			Weapon->RegisterComponent();
		}

		/** A point straight ahead of the shooter */
		static FVector PointAhead(float Distance)
		{
			return FVector(Distance, 0.0f, 0.0f);
		}

		/** Spawns an actor that blocks shots, with its near side at Distance ahead of the shooter */
		AActor* SpawnObstacle(float Distance) const
		{
			const float HalfDepth = 50.0f;

			AActor* Actor = World->SpawnActor<AActor>();
			UBoxComponent* Box = NewObject<UBoxComponent>(Actor);
			Box->SetBoxExtent(FVector(HalfDepth, 200.0f, 200.0f));
			Box->SetCollisionProfileName(TEXT("BlockAll"));
			Actor->SetRootComponent(Box);
			Box->RegisterComponent();
			Actor->SetActorLocation(PointAhead(Distance + HalfDepth));

			return Actor;
		}

		/** Spawns an obstacle that also has health */
		UTitanHealthComponent* SpawnTarget(float Distance) const
		{
			UTitanHealthComponent* Health = NewObject<UTitanHealthComponent>(SpawnObstacle(Distance));
			Health->RegisterComponent();

			return Health;
		}

		FTestWorldWrapper WorldWrapper;
		UWorld* World = nullptr;
		UTitanWeaponComponent* Weapon = nullptr;
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponTargetWithinRangeTakesDamageTest, "Titan.Weapon.TargetWithinRangeTakesDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponTargetWithinRangeTakesDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	const float Distance = Weapon->GetRange() * 0.5f;
	const UTitanHealthComponent* Target = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Target loses the weapon's damage"), Target->GetHealth(), Target->GetMaxHealth() - Weapon->GetDamage());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponTargetBeyondRangeTakesNoDamageTest, "Titan.Weapon.TargetBeyondRangeTakesNoDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponTargetBeyondRangeTakesNoDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	// The range is measured from the muzzle, which is at most a few steps from the shooter
	const float Distance = Weapon->GetRange() + 200.0f;
	const UTitanHealthComponent* Target = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Target beyond the range keeps its health"), Target->GetHealth(), Target->GetMaxHealth());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponTargetBehindObstacleTakesNoDamageTest, "Titan.Weapon.TargetBehindObstacleTakesNoDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponTargetBehindObstacleTakesNoDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	const float Distance = Weapon->GetRange() * 0.5f;
	Fixture.SpawnObstacle(Distance * 0.5f);
	const UTitanHealthComponent* Target = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Target behind an obstacle keeps its health"), Target->GetHealth(), Target->GetMaxHealth());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponOnlyFirstTargetHitTakesDamageTest, "Titan.Weapon.OnlyFirstTargetHitTakesDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponOnlyFirstTargetHitTakesDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	const float Distance = Weapon->GetRange() * 0.5f;
	const UTitanHealthComponent* FrontTarget = Fixture.SpawnTarget(Distance * 0.5f);
	const UTitanHealthComponent* BackTarget = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Front target loses the weapon's damage"), FrontTarget->GetHealth(), FrontTarget->GetMaxHealth() - Weapon->GetDamage());
	TestEqual(TEXT("Back target keeps its health"), BackTarget->GetHealth(), BackTarget->GetMaxHealth());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponFiresAgainOnlyAfterFireIntervalTest, "Titan.Weapon.FiresAgainOnlyAfterFireInterval",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponFiresAgainOnlyAfterFireIntervalTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	const float Distance = Weapon->GetRange() * 0.5f;
	const FVector TargetPoint = Fixture.PointAhead(Distance);
	const UTitanHealthComponent* Target = Fixture.SpawnTarget(Distance);
	const float MaxHealth = Target->GetMaxHealth();
	const float Damage = Weapon->GetDamage();

	Weapon->Fire(TargetPoint);
	Weapon->Fire(TargetPoint);
	TestEqual(TEXT("A second shot at the same moment does not fire"), Target->GetHealth(), MaxHealth - Damage);

	Fixture.WorldWrapper.TickTestWorld(Weapon->GetFireInterval() * 0.5f);
	Weapon->Fire(TargetPoint);
	TestEqual(TEXT("A shot before the fire interval has passed does not fire"), Target->GetHealth(), MaxHealth - Damage);

	Fixture.WorldWrapper.TickTestWorld(Weapon->GetFireInterval());
	Weapon->Fire(TargetPoint);
	TestEqual(TEXT("A shot after the fire interval has passed fires"), Target->GetHealth(), MaxHealth - Damage * 2.0f);

	return true;
}

#endif
