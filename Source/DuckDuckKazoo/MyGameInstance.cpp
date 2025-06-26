#include "MyGameInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "MainMenu.h"


UMyGameInstance::UMyGameInstance()
	: MainMenu(nullptr)
{
	UE_LOG(LogTemp, Warning, TEXT("Game instance constructed."));

	// Load the widget blueprint class
	static ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/DuckDuckKazoo/MainMenu/WBP_MainMenu"));
	if (MenuBPClass.Class != nullptr)
	{
		MainMenuClass = MenuBPClass.Class;
		UE_LOG(LogTemp, Log, TEXT("Main menu class successfully loaded."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load main menu class!"));
	}
}

const FName UMyGameInstance::SESSION_NAME = TEXT("DuckSession");

void UMyGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Session Interface initialized."));

			// Set up delegates only once
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get Session Interface."));
		}
	}
}



void UMyGameInstance::ShowMainMenu()
{
	if (MainMenuClass)
	{
		if (!MainMenu)
		{
			MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);
			if (MainMenu)
			{
				MainMenu->AddToViewport();

				UMainMenu* Menu = Cast<UMainMenu>(MainMenu);
				if (Menu)
				{
					Menu->SetGameInstanceReference(this);
				}

				UE_LOG(LogTemp, Log, TEXT("Main menu widget added to viewport."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create main menu widget."));
			}
		}
	}
}



void UMyGameInstance::Host()
{
	UEngine* Engine = GetEngine();
	if (ensure(Engine))
	{
		Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Hosting game..."));
	}

	CreateGameSession();
}

void UMyGameInstance::Join(const FString& Address)
{
	UEngine* Engine = GetEngine();
	if (ensure(Engine))
	{
		Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Joining game at: %s"), *Address));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		UE_LOG(LogTemp, Log, TEXT("Client travel initiated to: %s"), *Address);
	} 
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not get the PlayerController to initiate client travel."));
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error: Could not join %s"), *Address));
		}
	}
}

void UMyGameInstance::CreateGameSession()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session Interface is invalid."));
		return;
	}

	// Check for existing session
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session already exists, destroying first..."));
		DestroyExistingSession();
		return;
	}

	// Prepare session settings
	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = true;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->Set(FName("ServerName"), FString("My Cool Server"), EOnlineDataAdvertisementType::ViaOnlineService);


	// Create session
	if (SessionInterface->CreateSession(0, SESSION_NAME, *SessionSettings))
	{
		UE_LOG(LogTemp, Log, TEXT("Creating session..."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initiate session creation."));
	}
}

void UMyGameInstance::DestroyExistingSession()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session Interface is invalid."));
		return;
	}

	if (SessionInterface->DestroySession(SESSION_NAME))
	{
		UE_LOG(LogTemp, Log, TEXT("Destroying existing session..."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy session."));
	}
}

void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionName != SESSION_NAME)
		return;

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session created successfully. Starting Server Travel."));
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/DuckDuckKazoo/Levels/Lobby?Listen");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Session creation failed."));
	}
}

void UMyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionName != SESSION_NAME)
		return;

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session destroyed, creating new one..."));
		CreateGameSession();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy session."));
	}
}

void UMyGameInstance::SearchAvailableSessions()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session Interface invalid"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 20;
	const static FName PRESENCE_KEY = FName(TEXT("Presence"));
	SessionSearch->QuerySettings.Set(PRESENCE_KEY, true, EOnlineComparisonOp::Equals);


	UE_LOG(LogTemp, Log, TEXT("Searching for LAN sessions..."));

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session search failed or invalid search object."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Found %d session(s)."), SessionSearch->SearchResults.Num());

	// Notify Main Menu to refresh list
	if (MainMenu)
	{
		UMainMenu* Menu = Cast<UMainMenu>(MainMenu);
		if (Menu)
		{
			Menu->RefreshServerList();
		}
	}
}

const TArray<FOnlineSessionSearchResult>& UMyGameInstance::GetSearchResults() const
{
	static const TArray<FOnlineSessionSearchResult> EmptyResults;
	return SessionSearch.IsValid() ? SessionSearch->SearchResults : EmptyResults;
}








