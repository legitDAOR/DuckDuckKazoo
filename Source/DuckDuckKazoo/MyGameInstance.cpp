#include "MyGameInstance.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "MainMenu.h"

#define SEARCH_PRESENCE FName(TEXT("Presence"))
const FName UMyGameInstance::SESSION_NAME = TEXT("DuckSession");

UMyGameInstance::UMyGameInstance() : MainMenu(nullptr)
{
    static ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/DuckDuckKazoo/MainMenu/WBP_MainMenu"));
    if (MenuBPClass.Class) MainMenuClass = MenuBPClass.Class;
}

void UMyGameInstance::Init()
{
    Super::Init();

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
        }
    }
}

void UMyGameInstance::ShowMainMenu()
{
    if (MainMenuClass && !MainMenu)
    {
        MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);
        if (MainMenu)
        {
            MainMenu->AddToViewport();
            if (UMainMenu* Menu = Cast<UMainMenu>(MainMenu))
            {
                Menu->SetGameInstanceReference(this);
            }
        }
    }
}

void UMyGameInstance::HostOnline(const FString& CustomName)
{
    if (SessionInterface.IsValid())
    {
        DestroyExistingSession();

        SessionSettings = MakeShareable(new FOnlineSessionSettings());
        SessionSettings->bIsLANMatch = false;
        SessionSettings->NumPublicConnections = 4;
        SessionSettings->bAllowJoinInProgress = true;
        SessionSettings->bShouldAdvertise = true;
        SessionSettings->bUsesPresence = true;
        SessionSettings->bUseLobbiesIfAvailable = true;
        SessionSettings->Set(FName("ServerName"), CustomName, EOnlineDataAdvertisementType::ViaOnlineService);

        SessionInterface->CreateSession(0, SESSION_NAME, *SessionSettings);
    }
}

void UMyGameInstance::HostLocal()
{
    if (UWorld* World = GetWorld())
    {
        World->ServerTravel(LOBBY_MAP + TEXT("?Listen"));
    }
}

void UMyGameInstance::StartSinglePlayer()
{
    if (UWorld* World = GetWorld())
    {
        World->ServerTravel(SINGLEPLAYER_MAP);
    }
}

void UMyGameInstance::DestroyExistingSession()
{
    if (SessionInterface.IsValid() && SessionInterface->GetNamedSession(SESSION_NAME))
    {
        SessionInterface->DestroySession(SESSION_NAME);
    }
}

void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful && SessionName == SESSION_NAME)
    {
        if (UWorld* World = GetWorld())
        {
            World->ServerTravel(LOBBY_MAP + TEXT("?Listen"));
        }
    }
}

void UMyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UMyGameInstance::SearchAvailableSessions()
{
    if (!SessionInterface.IsValid()) return;

    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = false;
    SessionSearch->MaxSearchResults = 50;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (SessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("FindSessionsComplete: %d sessions found"), SessionSearch->SearchResults.Num());
    }

    if (bWasSuccessful && MainMenu)
    {
        if (UMainMenu* Menu = Cast<UMainMenu>(MainMenu))
        {
            Menu->RefreshServerList();
        }
    }
}

const TArray<FOnlineSessionSearchResult>& UMyGameInstance::GetSearchResults() const
{
    static const TArray<FOnlineSessionSearchResult> Empty;
    return SessionSearch.IsValid() ? SessionSearch->SearchResults : Empty;
}

void UMyGameInstance::JoinLocal(const FString& Address)
{
    if (Address.IsEmpty()) return;

    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
    }
}

void UMyGameInstance::JoinByIndex(int32 Index)
{
    if (SessionInterface.IsValid() && SessionSearch.IsValid() && SessionSearch->SearchResults.IsValidIndex(Index))
    {
        SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
    }
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid()) return;

    FString ConnectString;
    if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
    {
        if (APlayerController* PC = GetFirstLocalPlayerController())
        {
            PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
        }
    }
}
