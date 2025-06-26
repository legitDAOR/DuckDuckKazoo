#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerDisplayRow.generated.h"

UCLASS()
class DUCKDUCKKAZOO_API UServerDisplayRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

public:
	UPROPERTY(meta = (BindWidget)) class UTextBlock* ServerNameText;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* HostName;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* ServerSize;
	UPROPERTY(meta = (BindWidget)) class UButton* SelectServerButton;

	UPROPERTY() class UMainMenu* Parent;
	UPROPERTY() int32 Index;

	void SetServerDetails(const FString& ServerName, const FString& Host, const FString& Size);
	void SetParentAndIndex(class UMainMenu* InParent, int32 InIndex);

private:
	UFUNCTION() void OnClicked();
};
