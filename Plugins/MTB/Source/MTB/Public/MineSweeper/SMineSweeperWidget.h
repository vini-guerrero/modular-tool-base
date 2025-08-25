#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "MTBUtils.h" // Text & Notify Helpers

class SUniformGridPanel;
class STextBlock;

struct FMineSweeperTexts
{
    FText Width = MTBUtils::GetMineSweeperText("WidthKey");
    FText Height = MTBUtils::GetMineSweeperText("HeightKey");
    FText Bombs = MTBUtils::GetMineSweeperText("BombsKey");
    FText NewGame = MTBUtils::GetMineSweeperText("NewGameKey");
    FText Ready = MTBUtils::GetMineSweeperText("ReadyKey");
    FText GoodLuck = MTBUtils::GetMineSweeperText("GoodLuckKey");
    FText GameOver = MTBUtils::GetMineSweeperText("GameOverKey");
	FText GameOverNotify = MTBUtils::GetMineSweeperText("GameOverNotifyKey");
    FText YouWin = MTBUtils::GetMineSweeperText("YouWinKey");
	FText YouWinNotify = MTBUtils::GetMineSweeperText("YouWinKeyNotifyKey");
};

class SMineSweeperWidget : public SCompoundWidget
{

public:

	SLATE_BEGIN_ARGS(SMineSweeperWidget) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);

private:

	TSharedRef<SWidget> BuildUI();
	void RebuildGrid();
	FReply OnNewGameClicked();

	void StartNewGame();
	void ResetGameFlags();
	void PlaceBombs();
	void ComputeAdjacencies();

	FReply OnTileClicked(int32 Index);
	void RevealTile(int32 Index);
	void RevealZeroAreaFrom(int32 StartIndex);
	void ForEachNeighbor(int32 Index, TFunctionRef<void(int32)> Visit);

	FORCEINLINE int32 ToIndex(int32 X, int32 Y) const { return Y * GridWidth + X; }
	FORCEINLINE bool InBounds(int32 X, int32 Y) const { return X >= 0 && Y >= 0 && X < GridWidth && Y < GridHeight; }
	FORCEINLINE bool IsGameOverOrWon() const { return bGameOver || bWon; }

private:

	static constexpr int32 MaxGridSize = 64;
	int32 GridWidth = 10;
	int32 GridHeight = 10;
	int32 NumBombs = 15;

	bool bFirstClick = true;
	bool bGameOver = false;
	bool bWon = false;

	TArray<uint8> Bombs;
	TArray<uint8> Adjacent;
	TArray<uint8> Revealed;

	TSharedPtr<SUniformGridPanel> GridPanel;
	TSharedPtr<STextBlock> StatusText;

	FRandomStream RNG;
	FMineSweeperTexts MineSweeperTexts;

};
