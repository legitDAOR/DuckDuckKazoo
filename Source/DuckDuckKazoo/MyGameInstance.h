#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstance.generated.h"

class UMainMenu;

UCLASS()
class DUCKDUCKKAZOO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();
	virtual void Init() override;

	UFUNCTION(BlueprintCallable) void ShowMainMenu();

	// Multiplayer options
	UFUNCTION(BlueprintCallable) void HostOnline();
	UFUNCTION(BlueprintCallable) void HostLocal();
	UFUNCTION(BlueprintCallable) void StartSinglePlayer();
	UFUNCTION(BlueprintCallable) void JoinByIndex(int32 Index);
	UFUNCTION(BlueprintCallable) void JoinLocal(const FString& Address);

	void SearchAvailableSessions();
	const TArray<FOnlineSessionSearchResult>& GetSearchResults() const;

private:
	UPROPERTY() TSubclassOf<UUserWidget> MainMenuClass;
	UPROPERTY() UUserWidget* MainMenu;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	static const FName SESSION_NAME;
	const FString LOBBY_MAP = TEXT("/Game/DuckDuckKazoo/Levels/Lobby");
	const FString SINGLEPLAYER_MAP = TEXT("/Game/DuckDuckKazoo/Levels/SinglePlayerMap");

	void CreateOnlineSession();
	void DestroyExistingSession();

	// Callbacks
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
