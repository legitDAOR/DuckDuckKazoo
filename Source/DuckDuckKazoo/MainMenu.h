#pragma once

#include "CoreMinimal.h"
#include "Components/ScrollBox.h"
#include "ServerDisplayRow.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UMyGameInstance;
class UScrollBox;
class UServerDisplayRow;

UCLASS()
class DUCKDUCKKAZOO_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetGameInstanceReference(UMyGameInstance* Instance);

	void SelectServer(int32 Index);

	void RefreshServerList();

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	// Buttons
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonJoin;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonHost;
	UPROPERTY(meta = (BindWidget)) class UButton* PlayButton;
	UPROPERTY(meta = (BindWidget)) class UButton* QuitButton;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonConfirm;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonCancel;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonConfirmLocal;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonCancelLocal;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonYes;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonNo;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonSinglePlayer;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonLocalMultiplayer;
	UPROPERTY(meta = (BindWidget)) class UButton* ButtonMultiplayer;
	

	// Switcher and Text Input
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* Switch;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* IPAddress;

	// Searching and populating rows
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ServerScrollBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UServerDisplayRow> ServerRowClass;


	// Button functions
	UFUNCTION() void OnHostClicked();
	UFUNCTION() void OnJoinClicked();
	UFUNCTION() void OnPlayClicked();
	//UFUNCTION() void OnConfirmClicked();
	UFUNCTION() void OnCancelClicked();
	UFUNCTION() void OnConfirmLocalClicked();
	UFUNCTION() void OnCancelLocalClicked();
	UFUNCTION() void OnQuitClicked();
	UFUNCTION() void OnYesClicked();
	UFUNCTION() void OnNoClicked();
	//UFUNCTION() void OnSinglePlayerClicked();
	UFUNCTION() void OnLocalMultiplayerClicked();
	UFUNCTION() void OnMultiplayerClicked();

	int32 SelectedIndex = -1;

	UMyGameInstance* GameInstanceRef;
};
