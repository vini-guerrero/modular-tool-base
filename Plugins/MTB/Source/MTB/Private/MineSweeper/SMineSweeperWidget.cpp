
#include "MineSweeper/SMineSweeperWidget.h"

#include "InputCoreTypes.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/AppStyle.h"

void SMineSweeperWidget::Construct(const FArguments& InArgs)
{
    RNG.Initialize(FDateTime::Now().GetTicks());
    MineSweeperTexts = FMineSweeperTexts();
    ChildSlot
    [
        BuildUI()
    ];
    StartNewGame();
}

TSharedRef<SWidget> SMineSweeperWidget::BuildUI()
{
    return
    SNew(SBorder)
    .Padding(8)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().Padding(0,0,0,8)
        [
            SNew(SBorder)
            .Padding(FMargin(8))
            .BorderBackgroundColor(FLinearColor(0.1f,0.1f,0.1f,0.3f))
            [
                SNew(SScrollBox)
                .Orientation(Orient_Horizontal)
                + SScrollBox::Slot()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,12,0)
                    [
                        SNew(STextBlock).Text(MineSweeperTexts.Width)
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(4,0,12,0)
                    [
                        SNew(SSpinBox<int32>)
                        .MinDesiredWidth(80)
                        .MinValue(2).MaxValue(MaxGridSize)
                        .Value_Lambda([this]{ return GridWidth; })
                        .OnValueChanged_Lambda([this](int32 V){ GridWidth = FMath::Clamp(V, 2, MaxGridSize); })
                    ]
                    + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,8,0)
                    [
                        SNew(STextBlock).Text(MineSweeperTexts.Height)
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(4,0,12,0)
                    [
                        SNew(SSpinBox<int32>)
                        .MinDesiredWidth(80)
                        .MinValue(2).MaxValue(MaxGridSize)
                        .Value_Lambda([this]{ return GridHeight; })
                        .OnValueChanged_Lambda([this](int32 V){ GridHeight = FMath::Clamp(V, 2, MaxGridSize); })
                    ]
                    + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,8,0)
                    [
                        SNew(STextBlock).Text(MineSweeperTexts.Bombs)
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(4,0,0,0)
                    [
                        SNew(SSpinBox<int32>)
                        .MinDesiredWidth(80)
                        .MinValue(1)
                        .MaxValue_Lambda([this]{
                            const int64 Tiles = static_cast<int64>(FMath::Max(2, GridWidth)) * FMath::Max(2, GridHeight);
                            return static_cast<int32>(FMath::Clamp<int64>(Tiles - 1, 1, 4096));
                        })
                        .Value_Lambda([this]{ return NumBombs; })
                        .OnValueChanged_Lambda([this](int32 V){
                            const int32 Tiles = FMath::Max(1, GridWidth * GridHeight);
                            NumBombs = FMath::Clamp(V, 1, FMath::Max(1, Tiles - 1));
                        })
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(16,0,0,0)
                    [
                        SNew(SButton)
                        .ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"))
                        .ContentPadding(FMargin(12,6))
                        .OnClicked(this, &SMineSweeperWidget::OnNewGameClicked)
                        [
                            SNew(STextBlock).Text(MineSweeperTexts.NewGame)
                        ]
                    ]
                ]
            ]
        ]
        + SVerticalBox::Slot().FillHeight(1.f)
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            [
                SAssignNew(GridPanel, SUniformGridPanel)
                .SlotPadding(FMargin(1))
            ]
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(0,8,0,0)
        [
            SNew(SBorder)
            .Padding(FMargin(6))
            .BorderBackgroundColor(FLinearColor(0.05f,0.05f,0.05f,0.3f))
            [
                SAssignNew(StatusText, STextBlock)
                .Text(MineSweeperTexts.Ready)
                .Justification(ETextJustify::Center)
            ]
        ]
    ];
}

void SMineSweeperWidget::StartNewGame()
{
    ResetGameFlags();

    GridWidth  = FMath::Clamp(GridWidth,  2, MaxGridSize);
    GridHeight = FMath::Clamp(GridHeight, 2, MaxGridSize);

    const int64 Tiles64 = static_cast<int64>(GridWidth) * GridHeight;
    const int32 MaxTiles = static_cast<int32>(FMath::Clamp<int64>(Tiles64, 4, static_cast<int64>(MaxGridSize) * MaxGridSize));

    NumBombs = FMath::Clamp(NumBombs, 1, FMath::Max(1, MaxTiles - 1));

    Bombs.Init(0, MaxTiles);
    Adjacent.Init(0, MaxTiles);
    Revealed.Init(0, MaxTiles);

    if (GridPanel.IsValid()) GridPanel->ClearChildren();

    PlaceBombs();
    ComputeAdjacencies();
    RebuildGrid();

    if (StatusText.IsValid()) StatusText->SetText(MineSweeperTexts.GoodLuck);
}

void SMineSweeperWidget::ResetGameFlags()
{
    bFirstClick = true;
    bGameOver = false;
    bWon = false;
}

void SMineSweeperWidget::PlaceBombs()
{
    const int32 Tiles = GridWidth * GridHeight;
    if (Tiles <= 1 || NumBombs <= 0) return;

    TArray<int32> Indices;
    Indices.Reserve(Tiles);
    for (int32 i = 0; i < Tiles; ++i) Indices.Add(i);
    for (int32 b = 0; b < NumBombs && Indices.Num() > 0; ++b)
    {
        const int32 Pick = RNG.RandRange(0, Indices.Num() - 1);
        const int32 Idx = Indices[Pick];
        Indices.RemoveAtSwap(Pick, 1, EAllowShrinking::No);
        Bombs[Idx] = 1;
    }
}

void SMineSweeperWidget::ComputeAdjacencies()
{
    auto CountAround = [this](int32 Index) -> uint8
    {
        uint8 Count = 0;
        ForEachNeighbor(Index, [this, &Count](int32 NIdx){ Count += Bombs[NIdx] ? 1 : 0; });
        return Count;
    };
    const int32 Tiles = GridWidth * GridHeight;
    for (int32 i = 0; i < Tiles; ++i) Adjacent[i] = Bombs[i] ? 0 : CountAround(i);
}

void SMineSweeperWidget::RebuildGrid()
{
    if (!GridPanel.IsValid()) return;
    GridPanel->ClearChildren();
    const int32 Tiles = GridWidth * GridHeight;
    for (int32 index = 0; index < Tiles; ++index)
    {
        const int32 X = index % GridWidth;
        const int32 Y = index / GridWidth;
        const bool bIsRevealed = Revealed[index] != 0;
        const bool bIsBomb = Bombs[index] != 0;
        const uint8 Adj = Adjacent[index];
        FText Label;
        if (bIsRevealed)
        {
            if (bIsBomb) Label = FText::FromString(TEXT("💣"));
            else if (Adj > 0) Label = FText::AsNumber(Adj);
            else Label = FText::GetEmpty();
        }
        else
        {
            Label = FText::FromString(TEXT(" "));
        }
        GridPanel->AddSlot(X, Y)
        [
            SNew(SButton)
            .ContentPadding(FMargin(6))
            .IsEnabled(!IsGameOverOrWon() && !bIsRevealed)
            .OnClicked(FOnClicked::CreateSP(this, &SMineSweeperWidget::OnTileClicked, index))
            [
                SNew(STextBlock)
                .Justification(ETextJustify::Center)
                .Text(Label)
                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
            ]
        ];
    }
    if (!bGameOver)
    {
        int32 HiddenSafe = 0;
        for (int32 i = 0; i < Tiles; ++i) if (!Bombs[i] && !Revealed[i]) ++HiddenSafe;
        if (HiddenSafe == 0)
        {
            bWon = true;
            if (StatusText.IsValid()) {
                StatusText->SetText(MineSweeperTexts.YouWin);
                MTBUtils::ShowNotifyInfo(MineSweeperTexts.YouWin, MineSweeperTexts.YouWinNotify);
            }
        }
    }
}

FReply SMineSweeperWidget::OnNewGameClicked()
{
    StartNewGame();
    return FReply::Handled();
}

FReply SMineSweeperWidget::OnTileClicked(int32 Index)
{
    if (IsGameOverOrWon() || Revealed[Index]) return FReply::Handled();

    if (bFirstClick)
    {
        bFirstClick = false;

        if (Bombs[Index])
        {
            int32 SwapTo = INDEX_NONE;
            const int32 Tiles = GridWidth * GridHeight;

            auto IsNeighbor = [this, Index](int32 Other)
            {
                if (Other == Index) return true;
                const int32 X  = Index % GridWidth;
                const int32 Y  = Index / GridWidth;
                const int32 OX = Other % GridWidth;
                const int32 OY = Other / GridWidth;
                return FMath::Abs(OX - X) <= 1 && FMath::Abs(OY - Y) <= 1;
            };

            for (int32 i = 0; i < Tiles; ++i)
            {
                if (!Bombs[i] && !IsNeighbor(i))
                {
                    SwapTo = i;
                    break;
                }
            }

            if (SwapTo == INDEX_NONE)
            {
                for (int32 i = 0; i < Tiles; ++i)
                {
                    if (!Bombs[i] && i != Index)
                    {
                        SwapTo = i;
                        break;
                    }
                }
            }

            if (SwapTo != INDEX_NONE)
            {
                Bombs[Index] = 0;
                Bombs[SwapTo] = 1;
            }
        }

        ComputeAdjacencies();
    }

    if (Bombs[Index])
    {
        for (int32 i = 0; i < Bombs.Num(); ++i)
        {
            if (Bombs[i]) Revealed[i] = 1;
        }
        bGameOver = true;
        if (StatusText.IsValid()) {
            StatusText->SetText(MineSweeperTexts.GameOver);
            MTBUtils::ShowNotifyInfo(MineSweeperTexts.GameOver, MineSweeperTexts.GameOverNotify);
        }
        RebuildGrid();
        return FReply::Handled();
    }

    RevealTile(Index);
    RebuildGrid();
    return FReply::Handled();
}

void SMineSweeperWidget::RevealTile(int32 Index)
{
    if (Revealed[Index]) return;
    Revealed[Index] = 1;
    if (Adjacent[Index] == 0 && !Bombs[Index]) RevealZeroAreaFrom(Index);
}

void SMineSweeperWidget::RevealZeroAreaFrom(int32 StartIndex)
{
    TArray<int32> Queue;
    Queue.Reserve(GridWidth * GridHeight / 4);
    Queue.Add(StartIndex);
    while (Queue.Num() > 0)
    {
        const int32 Current = Queue.Pop(EAllowShrinking::No);
        ForEachNeighbor(Current, [this, &Queue](int32 NIdx)
        {
            if (!Revealed[NIdx] && !Bombs[NIdx])
            {
                Revealed[NIdx] = 1;
                if (Adjacent[NIdx] == 0) Queue.Add(NIdx);
            }
        });
    }
}

void SMineSweeperWidget::ForEachNeighbor(int32 Index, TFunctionRef<void(int32)> Visit)
{
    const int32 X = Index % GridWidth;
    const int32 Y = Index / GridWidth;
    for (int32 dy = -1; dy <= 1; ++dy)
    {
        for (int32 dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0) continue;
            const int32 NX = X + dx;
            const int32 NY = Y + dy;
            if (InBounds(NX, NY)) Visit(ToIndex(NX, NY));
        }
    }
}
