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

void UServerDisplayRow::SetServerDetails(const FString& ServerName, const FString& Host, const FString& Size)
{
	if (ServerNameText)
		ServerNameText->SetText(FText::FromString(ServerName));

	if (HostName)
		HostName->SetText(FText::FromString(Host));

	if (ServerSize)
		ServerSize->SetText(FText::FromString(Size));
}

void UServerDisplayRow::SetParentAndIndex(UMainMenu* InParent, int32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
}

void UServerDisplayRow::OnClicked()
{
	if (Parent)
	{
		Parent->SelectServer(Index);
	}
}
