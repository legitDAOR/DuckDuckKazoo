#include "ServerDisplayRow.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MainMenu.h"

bool UServerDisplayRow::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (SelectServerButton)
	{
		SelectServerButton->OnClicked.AddDynamic(this, &UServerDisplayRow::OnClicked);
	}

	return true;
}

void UServerDisplayRow::Setup(UMainMenu* InParent, int32 InIndex, const FString& InServerName, const FString& InHostName, const FString& InPlayerCount)
{
	Parent = InParent;
	Index = InIndex;

	if (ServerNameText) ServerNameText->SetText(FText::FromString(InServerName));
	if (HostNameText) HostNameText->SetText(FText::FromString(InHostName));
	if (PlayerCountText) PlayerCountText->SetText(FText::FromString(InPlayerCount));
}

void UServerDisplayRow::OnClicked()
{
	if (Parent)
	{
		Parent->SelectServer(Index);
	}
}
