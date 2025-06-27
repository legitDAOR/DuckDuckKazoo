#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstance.generated.h"

class UMainMenu;

USTRUCT(BlueprintType)
struct FServerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	FString HostUsername;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerCount;
};

UCLASS()
class DUCKDUCKKAZOO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();
	virtual void Init() override;

	UFUNCTION(BlueprintCallable) void ShowMainMenu();

	// Multiplayer options
	UFUNCTION(BlueprintCallable) void HostOnline(const FString& CustomName = "Online Game");
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

	void DestroyExistingSession();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
