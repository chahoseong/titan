#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/TitanWeaponComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/TitanTestDamageableActor.h"

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
		template <typename ActorType>
		ActorType* SpawnBlockingActor(float Distance) const
		{
			const float HalfDepth = 50.0f;

			ActorType* Actor = World->SpawnActor<ActorType>();
			UBoxComponent* Box = NewObject<UBoxComponent>(Actor);
			Box->SetBoxExtent(FVector(HalfDepth, 200.0f, 200.0f));
			Box->SetCollisionProfileName(TEXT("BlockAll"));
			Actor->SetRootComponent(Box);
			Box->RegisterComponent();
			Actor->SetActorLocation(PointAhead(Distance + HalfDepth));

			return Actor;
		}

		/** Spawns an actor that blocks shots and cannot be damaged */
		AActor* SpawnObstacle(float Distance) const
		{
			return SpawnBlockingActor<AActor>(Distance);
		}

		/** Spawns an actor that blocks shots and records the damage it receives */
		ATitanTestDamageableActor* SpawnTarget(float Distance) const
		{
			return SpawnBlockingActor<ATitanTestDamageableActor>(Distance);
		}

		FTestWorldWrapper WorldWrapper;
		UWorld* World = nullptr;
		UTitanWeaponComponent* Weapon = nullptr;
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponTargetWithinRangeReceivesDamageTest, "Titan.Weapon.TargetWithinRangeReceivesDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponTargetWithinRangeReceivesDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	const float Distance = Weapon->GetRange() * 0.5f;
	const ATitanTestDamageableActor* Target = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Target receives the weapon's damage"), Target->ReceivedDamage, Weapon->GetDamage());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponTargetBeyondRangeReceivesNoDamageTest, "Titan.Weapon.TargetBeyondRangeReceivesNoDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponTargetBeyondRangeReceivesNoDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	// The range is measured from the muzzle, which is at most a few steps from the shooter
	const float Distance = Weapon->GetRange() + 200.0f;
	const ATitanTestDamageableActor* Target = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Target beyond the range receives no damage"), Target->ReceivedDamage, 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponTargetBehindObstacleReceivesNoDamageTest, "Titan.Weapon.TargetBehindObstacleReceivesNoDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponTargetBehindObstacleReceivesNoDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	const float Distance = Weapon->GetRange() * 0.5f;
	Fixture.SpawnObstacle(Distance * 0.5f);
	const ATitanTestDamageableActor* Target = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Target behind an obstacle receives no damage"), Target->ReceivedDamage, 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanWeaponOnlyFirstTargetHitReceivesDamageTest, "Titan.Weapon.OnlyFirstTargetHitReceivesDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanWeaponOnlyFirstTargetHitReceivesDamageTest::RunTest(const FString& Parameters)
{
	TitanWeaponComponentTests::FFixture Fixture(*this);
	UTitanWeaponComponent* Weapon = Fixture.Weapon;
	if (!TestNotNull(TEXT("Weapon component"), Weapon))
	{
		return false;
	}

	const float Distance = Weapon->GetRange() * 0.5f;
	const ATitanTestDamageableActor* FrontTarget = Fixture.SpawnTarget(Distance * 0.5f);
	const ATitanTestDamageableActor* BackTarget = Fixture.SpawnTarget(Distance);

	Weapon->Fire(Fixture.PointAhead(Distance));
	TestEqual(TEXT("Front target receives the weapon's damage"), FrontTarget->ReceivedDamage, Weapon->GetDamage());
	TestEqual(TEXT("Back target receives no damage"), BackTarget->ReceivedDamage, 0.0f);

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
	const ATitanTestDamageableActor* Target = Fixture.SpawnTarget(Distance);
	const float Damage = Weapon->GetDamage();

	Weapon->Fire(TargetPoint);
	Weapon->Fire(TargetPoint);
	TestEqual(TEXT("A second shot at the same moment does not fire"), Target->ReceivedDamage, Damage);

	Fixture.WorldWrapper.TickTestWorld(Weapon->GetFireInterval() * 0.5f);
	Weapon->Fire(TargetPoint);
	TestEqual(TEXT("A shot before the fire interval has passed does not fire"), Target->ReceivedDamage, Damage);

	Fixture.WorldWrapper.TickTestWorld(Weapon->GetFireInterval());
	Weapon->Fire(TargetPoint);
	TestEqual(TEXT("A shot after the fire interval has passed fires"), Target->ReceivedDamage, Damage * 2.0f);

	return true;
}

#endif
