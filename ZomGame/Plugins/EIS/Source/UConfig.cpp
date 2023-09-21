// Copyright 2022 ZADROT LLC

UConfig::UConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Scale = 1.0f;
}

void UConfig::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UConfig, Scale))
	{
		FSlateApplication::Get().SetApplicationScale(Scale);
	}
}

void UConfig::UpdateScale()
{ 
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().SetApplicationScale(Scale); 
	}
}