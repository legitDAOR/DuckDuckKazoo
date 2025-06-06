#include "MainMenu.h"
#include "Components/Button.h"
#include "MyGameInstance.h"
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
	if (ButtonConfirm) ButtonConfirm->OnClicked.AddDynamic(this, &UMainMenu::OnConfirmClicked);
	if (ButtonCancel) ButtonCancel->OnClicked.AddDynamic(this, &UMainMenu::OnCancelClicked);
	if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClicked);
	if (ButtonYes) ButtonYes->OnClicked.AddDynamic(this, &UMainMenu::OnYesClicked);
	if (ButtonNo) ButtonNo->OnClicked.AddDynamic(this, &UMainMenu::OnNoClicked);

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

// Escape key handling
FReply UMainMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!Switch) return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		int32 CurrentIndex = Switch->GetActiveWidgetIndex();

		switch (CurrentIndex)
		{
		case 3: // Input IP Menu
			Switch->SetActiveWidgetIndex(2);
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from Input IP Menu (3) to Host/Join Menu (2)."));
			return FReply::Handled();

		case 2: // Host or Join Menu
			Switch->SetActiveWidgetIndex(1);
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from Host/Join Menu (2) to Main Menu (1)."));
			return FReply::Handled();

		case 1: // Main Menu
			Switch->SetActiveWidgetIndex(0); // Open quit confirmation
			UE_LOG(LogTemp, Log, TEXT("Escape: Going from Main Menu (1) to Confirmation Menu (0)."));
			return FReply::Handled();

		case 0: // Quit Confirmation Menu
			Switch->SetActiveWidgetIndex(1); // Return to Main Menu
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
		Switch->SetActiveWidgetIndex(3); // IP input screen
		UE_LOG(LogTemp, Log, TEXT("Switcher changed to index 2 (Join Menu)."));
	}
}

void UMainMenu::OnPlayClicked()
{
	if (Switch)
	{
		Switch->SetActiveWidgetIndex(2); // Host or Join
		UE_LOG(LogTemp, Log, TEXT("Switcher changed to index 1 (Main Menu)."));
	}
}

void UMainMenu::OnConfirmClicked()
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
