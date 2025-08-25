#pragma once

#include "Internationalization/StringTableCore.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h" 

namespace MTBUtils
{
    inline const FName& MineSweeperTableName()
    {
        static const FName TableName(TEXT("/MTB/StringTables/ST_MineSweeper.ST_MineSweeper"));
        return TableName;
    }

    inline FText FromStringTable(const FString& Key, const FName& Table = MineSweeperTableName())
    {
        return FText::FromStringTable(Table, *Key);
    }

    inline FText GetMineSweeperText(const FString& Key)
    {
        return FromStringTable(Key);
    }

    static void ShowNotifyInfo(const FText& Title, const FText& InfoText = FText(), float Duration = 4.f)
    {
        if (!FSlateApplication::IsInitialized()) return;
        FSlateNotificationManager& Manager = FSlateNotificationManager::Get();
        FText FullMessage = InfoText.IsEmpty() ? Title : FText::Format(FText::FromString("{0}\n{1}"), Title, InfoText);
        FNotificationInfo Info(FullMessage);
        Info.bFireAndForget = true;
        Info.bUseLargeFont = true;
        Info.FadeOutDuration = Duration;
        Info.ExpireDuration = Duration;
        Info.bUseThrobber = false;
        Info.bUseSuccessFailIcons = true;
        Info.bAllowThrottleWhenFrameRateIsLow = true;
        Info.ButtonDetails.Empty();
        TSharedPtr<SNotificationItem> Notification = Manager.AddNotification(Info);
        if (Notification.IsValid()) Notification->SetCompletionState(SNotificationItem::CS_Success);
    }
}