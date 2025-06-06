#include "MyGameInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
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

void UMyGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("Game Instance Init."));
	
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

	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/DuckDuckKazoo/Levels/Lobby?Listen");
		UE_LOG(LogTemp, Log, TEXT("Server travel initiated to Lobby with listen option."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not get the World to initiate server travel."));
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error: Could not host game."));
		}
	}
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
