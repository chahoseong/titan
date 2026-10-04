#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/TitanHealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

namespace TitanHealthComponentTests
{
	/** An empty game world with one actor that owns a health component. Uses no project asset */
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
		}

		FTestWorldWrapper WorldWrapper;
		UTitanHealthComponent* Health = nullptr;
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHealthStartsAtMaxHealthTest, "Titan.Health.StartsAtMaxHealth",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHealthStartsAtMaxHealthTest::RunTest(const FString& Parameters)
{
	const TitanHealthComponentTests::FFixture Fixture(*this);
	const UTitanHealthComponent* Health = Fixture.Health;
	if (!TestNotNull(TEXT("Health component"), Health))
	{
		return false;
	}

	TestEqual(TEXT("Health starts at max health"), Health->GetHealth(), Health->GetMaxHealth());
	TestFalse(TEXT("Owner starts alive"), Health->IsDead());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHealthDamageReducesHealthTest, "Titan.Health.DamageReducesHealth",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHealthDamageReducesHealthTest::RunTest(const FString& Parameters)
{
	const TitanHealthComponentTests::FFixture Fixture(*this);
	UTitanHealthComponent* Health = Fixture.Health;
	if (!TestNotNull(TEXT("Health component"), Health))
	{
		return false;
	}

	const float MaxHealth = Health->GetMaxHealth();
	const float Damage = MaxHealth * 0.25f;

	Health->ApplyDamage(Damage);
	TestEqual(TEXT("Health drops by the damage"), Health->GetHealth(), MaxHealth - Damage);

	Health->ApplyDamage(Damage);
	TestEqual(TEXT("Damage adds up over hits"), Health->GetHealth(), MaxHealth - Damage * 2.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHealthStaysWithinZeroAndMaxTest, "Titan.Health.HealthStaysWithinZeroAndMax",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHealthStaysWithinZeroAndMaxTest::RunTest(const FString& Parameters)
{
	const TitanHealthComponentTests::FFixture Fixture(*this);
	UTitanHealthComponent* Health = Fixture.Health;
	if (!TestNotNull(TEXT("Health component"), Health))
	{
		return false;
	}

	const float MaxHealth = Health->GetMaxHealth();

	// Negative damage would raise health if it were applied
	Health->ApplyDamage(-MaxHealth);
	TestEqual(TEXT("Health does not rise above max health"), Health->GetHealth(), MaxHealth);

	// More damage than the health that is left
	Health->ApplyDamage(MaxHealth * 2.0f);
	TestEqual(TEXT("Health does not drop below zero"), Health->GetHealth(), 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHealthDiesAtZeroHealthTest, "Titan.Health.DiesAtZeroHealth",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHealthDiesAtZeroHealthTest::RunTest(const FString& Parameters)
{
	const TitanHealthComponentTests::FFixture Fixture(*this);
	UTitanHealthComponent* Health = Fixture.Health;
	if (!TestNotNull(TEXT("Health component"), Health))
	{
		return false;
	}

	const float HalfHealth = Health->GetMaxHealth() * 0.5f;

	Health->ApplyDamage(HalfHealth);
	TestFalse(TEXT("Owner is alive while health is above zero"), Health->IsDead());

	Health->ApplyDamage(HalfHealth);
	TestEqual(TEXT("Health is zero"), Health->GetHealth(), 0.0f);
	TestTrue(TEXT("Owner is dead once health is zero"), Health->IsDead());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHealthNoDamageAfterDeathTest, "Titan.Health.NoDamageAfterDeath",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHealthNoDamageAfterDeathTest::RunTest(const FString& Parameters)
{
	const TitanHealthComponentTests::FFixture Fixture(*this);
	UTitanHealthComponent* Health = Fixture.Health;
	if (!TestNotNull(TEXT("Health component"), Health))
	{
		return false;
	}

	const float MaxHealth = Health->GetMaxHealth();

	Health->ApplyDamage(MaxHealth);
	TestTrue(TEXT("Owner is dead"), Health->IsDead());

	Health->ApplyDamage(MaxHealth * 0.25f);
	TestEqual(TEXT("Health stays at zero"), Health->GetHealth(), 0.0f);
	TestTrue(TEXT("Owner stays dead"), Health->IsDead());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanHealthIgnoresNonPositiveDamageTest, "Titan.Health.IgnoresNonPositiveDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanHealthIgnoresNonPositiveDamageTest::RunTest(const FString& Parameters)
{
	const TitanHealthComponentTests::FFixture Fixture(*this);
	UTitanHealthComponent* Health = Fixture.Health;
	if (!TestNotNull(TEXT("Health component"), Health))
	{
		return false;
	}

	const float MaxHealth = Health->GetMaxHealth();
	const float Damage = MaxHealth * 0.25f;

	// Start below max health so that healing by negative damage would show
	Health->ApplyDamage(Damage);
	const float HealthBefore = Health->GetHealth();

	Health->ApplyDamage(0.0f);
	TestEqual(TEXT("Zero damage leaves health unchanged"), Health->GetHealth(), HealthBefore);

	Health->ApplyDamage(-Damage);
	TestEqual(TEXT("Negative damage leaves health unchanged"), Health->GetHealth(), HealthBefore);

	return true;
}

#endif
