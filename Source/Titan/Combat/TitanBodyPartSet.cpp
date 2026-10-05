#include "Combat/TitanBodyPartSet.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

const FTitanBodyPartEntry* UTitanBodyPartSet::FindBodyPart(FName BoneName) const
{
	return BodyParts.FindByPredicate([BoneName](const FTitanBodyPartEntry& Entry)
	{
		return Entry.Bones.Contains(BoneName);
	});
}

#if WITH_EDITOR
EDataValidationResult UTitanBodyPartSet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	TSet<FGameplayTag> SeenBodyParts;
	TSet<FName> SeenBones;

	for (int32 Index = 0; Index < BodyParts.Num(); ++Index)
	{
		const FTitanBodyPartEntry& Entry = BodyParts[Index];

		if (!Entry.BodyPart.IsValid())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Entry %d has no body part tag."), Index)));
			Result = EDataValidationResult::Invalid;
		}
		else if (SeenBodyParts.Contains(Entry.BodyPart))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Body part %s is listed more than once."), *Entry.BodyPart.ToString())));
			Result = EDataValidationResult::Invalid;
		}
		SeenBodyParts.Add(Entry.BodyPart);

		if (Entry.Bones.IsEmpty())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Entry %d has no bones."), Index)));
			Result = EDataValidationResult::Invalid;
		}

		for (const FName& Bone : Entry.Bones)
		{
			// A bone in two entries would make the hit location depend on entry order
			if (SeenBones.Contains(Bone))
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Bone %s belongs to more than one body part."), *Bone.ToString())));
				Result = EDataValidationResult::Invalid;
			}
			SeenBones.Add(Bone);
		}
	}

	return Result;
}
#endif
