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

void UServerDisplayRow::Setup(UMainMenu* InParent, int32 InIndex, const FServerData& ServerData)
{
	Parent = InParent;
	Index = InIndex;

	if (ServerNameText) ServerNameText->SetText(FText::FromString(ServerData.ServerName));
	if (HostNameText) HostNameText->SetText(FText::FromString(ServerData.HostUsername));
	if (PlayerCountText) PlayerCountText->SetText(FText::FromString(ServerData.PlayerCount));
}

void UServerDisplayRow::OnClicked()
{
	if (Parent)
	{
		Parent->SelectServer(Index);
	}
}
