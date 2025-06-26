#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/EditableText.h"
#include "Components/WidgetSwitcher.h"
#include "ServerDisplayRow.h"
#include "MyGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (ButtonHostMulti) ButtonHostMulti->OnClicked.AddDynamic(this, &UMainMenu::OnHostOnlineClicked);
    if (ButtonJoinMulti) ButtonJoinMulti->OnClicked.AddDynamic(this, &UMainMenu::OnJoinMultiClicked);
    if (ButtonHostLocal) ButtonHostLocal->OnClicked.AddDynamic(this, &UMainMenu::OnHostLocalClicked);
    if (ButtonJoinLocal) ButtonJoinLocal->OnClicked.AddDynamic(this, &UMainMenu::OnJoinLocalClicked);
    if (ButtonSinglePlayer) ButtonSinglePlayer->OnClicked.AddDynamic(this, &UMainMenu::OnSinglePlayerClicked);
    if (ButtonConfirmLocal) ButtonConfirmLocal->OnClicked.AddDynamic(this, &UMainMenu::OnConfirmLocalClicked);
    if (ButtonCancelLocal) ButtonCancelLocal->OnClicked.AddDynamic(this, &UMainMenu::OnCancelLocalClicked);
    if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClicked);
    if (PlayButton) PlayButton->OnClicked.AddDynamic(this, &UMainMenu::OnPlayButtonClicked);
    if (ButtonYes) ButtonYes->OnClicked.AddDynamic(this, &UMainMenu::OnYesClicked);
    if (ButtonNo) ButtonNo->OnClicked.AddDynamic(this, &UMainMenu::OnNoClicked);
    if (ButtonLocalMultiplayer) ButtonLocalMultiplayer->OnClicked.AddDynamic(this, &UMainMenu::OnLocalMultiplayerClicked);
    if (ButtonMultiplayer) ButtonMultiplayer->OnClicked.AddDynamic(this, &UMainMenu::OnMultiplayerClicked);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void UMainMenu::NativeDestruct()
{
    Super::NativeDestruct();

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}

void UMainMenu::SetGameInstanceReference(UMyGameInstance* Instance)
{
    GameInstanceRef = Instance;
}

void UMainMenu::RefreshServerList()
{
    if (!GameInstanceRef || !ServerScrollBox || !ServerRowClass) return;

    ServerScrollBox->ClearChildren();
    SelectedIndex.Reset();

    const TArray<FOnlineSessionSearchResult>& Results = GameInstanceRef->GetSearchResults();

    for (int32 i = 0; i < Results.Num(); ++i)
    {
        UServerDisplayRow* Row = CreateWidget<UServerDisplayRow>(this, ServerRowClass);
        if (Row)
        {
            FString ServerName, HostName, PlayerCount;
            Results[i].Session.SessionSettings.Get(FName("ServerName"), ServerName);
            HostName = Results[i].Session.OwningUserName;
            int32 MaxPlayers = Results[i].Session.SessionSettings.NumPublicConnections;
            int32 OpenSlots = Results[i].Session.NumOpenPublicConnections;
            PlayerCount = FString::Printf(TEXT("%d/%d"), MaxPlayers - OpenSlots, MaxPlayers);

            Row->Setup(this, i, ServerName, HostName, PlayerCount);
            ServerScrollBox->AddChild(Row);
        }
    }
}

void UMainMenu::SelectServer(int32 Index)
{
    SelectedIndex = Index;
    UE_LOG(LogTemp, Log, TEXT("Selected Server Index: %d"), Index);
}

FReply UMainMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (!Switch) return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        int32 Index = Switch->GetActiveWidgetIndex();
        switch (Index)
        {
        case 6: Switch->SetActiveWidgetIndex(3); break; // Join Multi to Multiplayer Menu
        case 5: Switch->SetActiveWidgetIndex(4); break; // Join Local to Local Menu
        case 4: Switch->SetActiveWidgetIndex(2); break; // Local Menu to Play Menu
        case 3: Switch->SetActiveWidgetIndex(2); break; // Multiplayer Menu to Play Menu
        case 2: Switch->SetActiveWidgetIndex(1); break; // Play Menu to Main Menu
        case 1: Switch->SetActiveWidgetIndex(0); break; // Main Menu to Quit Confirm
        case 0: Switch->SetActiveWidgetIndex(1); break; // Quit Confirm to Main Menu
        }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

// Hosting Logic

void UMainMenu::OnHostOnlineClicked()
{
    if (GameInstanceRef) GameInstanceRef->HostOnline();
}

void UMainMenu::OnHostLocalClicked()
{
    if (GameInstanceRef) GameInstanceRef->HostLocal();
}

void UMainMenu::OnSinglePlayerClicked()
{
    if (GameInstanceRef) GameInstanceRef->StartSinglePlayer();
}

// Join Buttons

void UMainMenu::OnJoinMultiClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(6); // Show Server List
    if (GameInstanceRef) GameInstanceRef->SearchAvailableSessions();
}

void UMainMenu::OnJoinLocalClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(5); // Show IP Entry
}

void UMainMenu::OnConfirmLocalClicked()
{
    if (!GameInstanceRef || !IPAddress) return;

    FString EnteredIP = IPAddress->GetText().ToString();

    // Ensure proper format, optional
    if (!EnteredIP.StartsWith(TEXT("127.")) && !EnteredIP.StartsWith(TEXT("192.")) && !EnteredIP.StartsWith(TEXT("localhost")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Entered IP seems invalid: %s"), *EnteredIP);
    }

    GameInstanceRef->JoinLocal(EnteredIP);
}

// Menu Navigation

void UMainMenu::OnCancelLocalClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(1);
}

void UMainMenu::OnPlayButtonClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(2);
}


void UMainMenu::OnQuitClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(0);
}

void UMainMenu::OnYesClicked()
{
    UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void UMainMenu::OnNoClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(1);
}

void UMainMenu::OnLocalMultiplayerClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(4);
}

void UMainMenu::OnMultiplayerClicked()
{
    if (Switch) Switch->SetActiveWidgetIndex(3);
}
