#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerDisplayRow.generated.h"

class UTextBlock;
class UButton;
class UMainMenu;

UCLASS()
class DUCKDUCKKAZOO_API UServerDisplayRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(UMainMenu* InParent, int32 InIndex, const FString& InServerName, const FString& InHostName, const FString& InPlayerCount);

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget)) UTextBlock* ServerNameText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* HostNameText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* PlayerCountText;
	UPROPERTY(meta = (BindWidget)) UButton* SelectServerButton;

	UPROPERTY() UMainMenu* Parent;
	int32 Index;

	UFUNCTION() void OnClicked();
};
