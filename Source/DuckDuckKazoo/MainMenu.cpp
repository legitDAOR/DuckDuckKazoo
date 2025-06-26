#include "MainMenu.h"
#include "Components/Button.h"
#include "MyGameInstance.h"
#include "Components/ScrollBox.h"
#include "ServerDisplayRow.h"
#include "Components/EditableText.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenu::SetGameInstanceReference(UMyGameInstance* Instance)
{
	GameInstanceRef = Instance;
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// Button bindings
	if (ButtonHost) ButtonHost->OnClicked.AddDynamic(this, &UMainMenu::OnHostClicked);
	if (ButtonJoin) ButtonJoin->OnClicked.AddDynamic(this, &UMainMenu::OnJoinClicked);
	if (PlayButton) PlayButton->OnClicked.AddDynamic(this, &UMainMenu::OnPlayClicked);
	//if (ButtonConfirm) ButtonConfirm->OnClicked.AddDynamic(this, &UMainMenu::OnConfirmClicked);
	if (ButtonCancel) ButtonCancel->OnClicked.AddDynamic(this, &UMainMenu::OnCancelClicked);
	if (ButtonConfirmLocal) ButtonConfirmLocal->OnClicked.AddDynamic(this, &UMainMenu::OnConfirmLocalClicked);
	if (ButtonCancelLocal) ButtonCancelLocal->OnClicked.AddDynamic(this, &UMainMenu::OnCancelLocalClicked);
	if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClicked);
	if (ButtonYes) ButtonYes->OnClicked.AddDynamic(this, &UMainMenu::OnYesClicked);
	if (ButtonNo) ButtonNo->OnClicked.AddDynamic(this, &UMainMenu::OnNoClicked);
	//if (ButtonSinglePlayer) ButtonSinglePlayer->OnClicked.AddDynamic(this, &UMainMenu::OnSinglePlayerClicked);
	if (ButtonLocalMultiplayer) ButtonLocalMultiplayer->OnClicked.AddDynamic(this, &UMainMenu::OnLocalMultiplayerClicked);
	if (ButtonMultiplayer) ButtonMultiplayer->OnClicked.AddDynamic(this, &UMainMenu::OnMultiplayerClicked);

	// Input mode
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;

		UE_LOG(LogTemp, Log, TEXT("MainMenu: Input mode set to UI only."));
	}
}

void UMainMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;

		UE_LOG(LogTemp, Log, TEXT("MainMenu: Input mode reset to Game Only."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenu: Could not reset input mode — PlayerController not found."));
	}
}

void UMainMenu::RefreshServerList()
{
	if (!GameInstanceRef || !ServerScrollBox || !ServerRowClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing references for server list update."));
		return;
	}

	ServerScrollBox->ClearChildren();

	const TArray<FOnlineSessionSearchResult>& Results = GameInstanceRef->GetSearchResults();

	if (Results.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No sessions found."));
		return;
	}

	for (int32 i = 0; i < Results.Num(); ++i)
	{
		UServerDisplayRow* Row = CreateWidget<UServerDisplayRow>(this, ServerRowClass);
		if (Row)
		{
			FString ServerDisplayName = "Unnamed Server";
			FString HostPlayerName = "Unknown Host";
			FString PlayerCount = "0/0";

			const FOnlineSessionSearchResult& SearchResult = Results[i];

			// Get the ServerName that was set during session creation
			SearchResult.Session.SessionSettings.Get(FName("ServerName"), ServerDisplayName);

			// Host player name comes from the session
			HostPlayerName = SearchResult.Session.OwningUserName;

			// Calculate player count
			int32 MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			int32 OpenSlots = SearchResult.Session.NumOpenPublicConnections;
			int32 CurrentPlayers = MaxPlayers - OpenSlots;

			PlayerCount = FString::Printf(TEXT("%d/%d"), CurrentPlayers, MaxPlayers);

			// Fill the row with data
			Row->SetServerDetails(ServerDisplayName, HostPlayerName, PlayerCount);
			Row->SetParentAndIndex(this, i);

			ServerScrollBox->AddChild(Row);
		}
	}
}



/*void UMainMenu::SelectServer(int32 Index)
{
	SelectedIndex = Index;
	UE_LOG(LogTemp, Log, TEXT("Selected server at index: %d"), Index);

	if (GameInstanceRef)
	{
		const TArray<FOnlineSessionSearchResult>& Results = GameInstanceRef->GetSearchResults();
		if (Results.IsValidIndex(Index))
		{
			GameInstanceRef->Join(Results[Index].GetSessionIdStr());
		}
	}
}*/

void UMainMenu::SelectServer(int32 Index)
{
	UE_LOG(LogTemp, Log, TEXT("Dummy server selected: %d"), Index);
}



// Escape key handling
FReply UMainMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!Switch) return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		int32 CurrentIndex = Switch->GetActiveWidgetIndex();

		switch (CurrentIndex)
		{
		case 5: // JoinMenu with Sessions
			Switch->SetActiveWidgetIndex(3);
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from JoinMenu (6) to LocalMultiplayerMenu (3)"));
			return FReply::Handled();
			
		case 4: // JoinMenuLocal
			Switch->SetActiveWidgetIndex(3);
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from JoinMenuLocal (5) to MultiplayerMenu (4)"));
			return FReply::Handled();
			
		case 3: // MultiplayerMenu
			Switch->SetActiveWidgetIndex(2);
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from MultiplayerMenu (4) to PlayMenu (2)"));
			return FReply::Handled();

		case 2: // Play Menu
			Switch->SetActiveWidgetIndex(1);
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from Host/Join Menu (2) to Main Menu (1)."));
			return FReply::Handled();

		case 1: // Main Menu
			Switch->SetActiveWidgetIndex(0); 
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from Main Menu (1) to Confirmation Menu (0)."));
			return FReply::Handled();

		case 0: // Quit Confirmation Menu
			Switch->SetActiveWidgetIndex(1); 
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from Confirmation Menu (0) to Main Menu (1)."));
			return FReply::Handled();

		default:
			UE_LOG(LogTemp, Warning, TEXT("Escape pressed — no behavior defined for index %d."), CurrentIndex);
			break;
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}


// Button logic

void UMainMenu::OnHostClicked()
{
	if (GameInstanceRef)
	{
		GameInstanceRef->Host();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null"));
	}
}

void UMainMenu::OnJoinClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(5); // JoinMenuMultiplayer
		UE_LOG(LogTemp, Log, TEXT("Switcher changed to index 5 JoinMenuMultiplayer"));

		if (ServerScrollBox) ServerScrollBox->ClearChildren();

		// Trigger real session search
		if (GameInstanceRef)
		{
			GameInstanceRef->SearchAvailableSessions();
		}
	}
}

void UMainMenu::OnPlayClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(2); // Game mode selection
		UE_LOG(LogTemp, Log, TEXT("Switcher changed to index 1 (Main Menu)."));
	}
}

/*void UMainMenu::OnConfirmClicked()
{
	
}*/

void UMainMenu::OnConfirmLocalClicked()
{
	if (GameInstanceRef && IPAddress)
	{
		FString EnteredIP = IPAddress->GetText().ToString();

		if (!EnteredIP.IsEmpty())
		{
			GameInstanceRef->Join(EnteredIP);
			UE_LOG(LogTemp, Log, TEXT("Join called with IP: %s"), *EnteredIP);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("IP address is empty."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef or IPAddress is null."));
	}
}


void UMainMenu::OnCancelClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(1); // Return to main menu
		UE_LOG(LogTemp, Log, TEXT("Cancelled join — returning to Menu (index 1)."));
	}
}

void UMainMenu::OnCancelLocalClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(1); // Return to main menu
		UE_LOG(LogTemp, Log, TEXT("Cancelled join — returning to Menu (index 1)."));
	}
}

void UMainMenu::OnQuitClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(0); // Show confirmation menu
		UE_LOG(LogTemp, Log, TEXT("Quit clicked — showing confirmation menu (index 0)."));
	}
}

void UMainMenu::OnYesClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Quit confirmed — exiting game."));
	UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void UMainMenu::OnNoClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(1); // Back to main menu
		UE_LOG(LogTemp, Log, TEXT("Quit canceled — returning to main menu (index 1)."));
	}
}

void UMainMenu::OnLocalMultiplayerClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(3);
		UE_LOG(LogTemp, Log, TEXT("LocalMultiplayer chosen - going to the menu (index 4)."));
	}
}

void UMainMenu::OnMultiplayerClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(3);
		UE_LOG(LogTemp, Log, TEXT("Multiplayer chosen - going to the menu (index 5)."));
	}
}

/*void UMainMenu::OnSinglePlayerClicked()
{
	if (Switch)
	{
		
	}
}*/
