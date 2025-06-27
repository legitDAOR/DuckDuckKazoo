#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UScrollBox;
class UButton;
class UEditableText;
class UWidgetSwitcher;
class UServerDisplayRow;
class UMyGameInstance;

UCLASS()
class DUCKDUCKKAZOO_API UMainMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    void SetGameInstanceReference(UMyGameInstance* Instance);
    void RefreshServerList();
    void SelectServer(int32 Index);

protected:
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:

    // Buttons
    UPROPERTY(meta = (BindWidget)) UButton* ButtonHostMulti;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonJoinMulti;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonHostLocal;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonJoinLocal;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonSinglePlayer;
    UPROPERTY(meta = (BindWidget)) UButton* PlayButton;
    UPROPERTY(meta = (BindWidget)) UButton* QuitButton;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonConfirmLocal;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonCancelLocal;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonYes;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonNo;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonLocalMultiplayer;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonMultiplayer;

    // UI Elements
    UPROPERTY(meta = (BindWidget)) UWidgetSwitcher* Switch;
    UPROPERTY(meta = (BindWidget)) UEditableText* IPAddress;
    UPROPERTY(meta = (BindWidget)) UEditableText* ServerNameInput;
    UPROPERTY(meta = (BindWidget)) UScrollBox* ServerScrollBox;

    UPROPERTY(EditAnywhere) TSubclassOf<UServerDisplayRow> ServerRowClass;
    UPROPERTY() UMyGameInstance* GameInstanceRef;

    TOptional<int32> SelectedIndex;

    // Button Callbacks
    UFUNCTION() void OnHostOnlineClicked();
    UFUNCTION() void OnJoinMultiClicked();
    UFUNCTION() void OnHostLocalClicked();
    UFUNCTION() void OnJoinLocalClicked();
    UFUNCTION() void OnSinglePlayerClicked();
    UFUNCTION() void OnConfirmLocalClicked();
    UFUNCTION() void OnCancelLocalClicked();
    UFUNCTION() void OnQuitClicked();
    UFUNCTION() void OnPlayButtonClicked();
    UFUNCTION() void OnYesClicked();
    UFUNCTION() void OnNoClicked();
    UFUNCTION() void OnLocalMultiplayerClicked();
    UFUNCTION() void OnMultiplayerClicked();
};
