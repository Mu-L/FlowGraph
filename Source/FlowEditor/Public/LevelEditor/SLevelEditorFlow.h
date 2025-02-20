// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

struct FAssetData;

class FLOWEDITOR_API SLevelEditorFlow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelEditorFlow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	void OnMapOpened(const FString& Filename, bool bAsTemplate);
	void CreateFlowWidget();

	void OnFlowChanged(const FAssetData& NewAsset);
	FString GetFlowPath() const;

	class UFlowComponent* FindFlowComponent() const;
	
	FName FlowPath;
};
