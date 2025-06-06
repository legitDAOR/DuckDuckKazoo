#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UMyGameInstance;

UCLASS()
class DUCKDUCKKAZOO_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetGameInstanceReference(UMyGameInstance* Instance);

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	// Buttons
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonJoin;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonHost;
	UPROPERTY(meta = (BindWidget)) class UButton* PlayButton;
	UPROPERTY(meta = (BindWidget)) class UButton* QuitButton;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonConfirm;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonYes;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonNo;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonCancel;

	// Switcher and Text Input
	UPROPERTY(meta = (BindWidget)) class UWidgetSwitcher* Switch;
	UPROPERTY(meta = (BindWidget)) class UEditableText* IPAddress;

	// Button functions
	UFUNCTION() void OnHostClicked();
	UFUNCTION() void OnJoinClicked();
	UFUNCTION() void OnPlayClicked();
	UFUNCTION() void OnConfirmClicked();
	UFUNCTION() void OnCancelClicked();
	UFUNCTION() void OnQuitClicked();
	UFUNCTION() void OnYesClicked();
	UFUNCTION() void OnNoClicked();

	UMyGameInstance* GameInstanceRef;
};
