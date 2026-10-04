#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/TitanBodyPartSet.h"
#include "Misc/AutomationTest.h"
#include "TitanGameplayTags.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanBodyPartSetFindTest, "Titan.BodyPartSet.Find",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanBodyPartSetFindTest::RunTest(const FString& Parameters)
{
	// Built in code so the test runs without any project asset
	UTitanBodyPartSet* BodyPartSet = NewObject<UTitanBodyPartSet>();

	FTitanBodyPartEntry& Head = BodyPartSet->BodyParts.AddDefaulted_GetRef();
	Head.BodyPart = TitanGameplayTags::BodyPart_Head;
	Head.Bones = { TEXT("head"), TEXT("neck_01") };

	FTitanBodyPartEntry& LeftArm = BodyPartSet->BodyParts.AddDefaulted_GetRef();
	LeftArm.BodyPart = TitanGameplayTags::BodyPart_Arm_Left;
	LeftArm.Bones = { TEXT("lowerarm_l") };

	TestTrue(TEXT("A listed bone resolves to its body part"),
		BodyPartSet->FindBodyPart(TEXT("head")) == TitanGameplayTags::BodyPart_Head);
	TestTrue(TEXT("Every bone of an entry resolves to the same body part"),
		BodyPartSet->FindBodyPart(TEXT("neck_01")) == TitanGameplayTags::BodyPart_Head);
	TestTrue(TEXT("Bones of different entries resolve to different body parts"),
		BodyPartSet->FindBodyPart(TEXT("lowerarm_l")) == TitanGameplayTags::BodyPart_Arm_Left);
	TestFalse(TEXT("A bone no entry lists resolves to an empty tag"),
		BodyPartSet->FindBodyPart(TEXT("calf_l")).IsValid());
	TestFalse(TEXT("No bone resolves to an empty tag"),
		BodyPartSet->FindBodyPart(NAME_None).IsValid());

	return true;
}

#endif
