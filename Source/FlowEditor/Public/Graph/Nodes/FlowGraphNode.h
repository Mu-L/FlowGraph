// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Templates/SubclassOf.h"

#include "FlowTypes.h"
#include "Nodes/FlowPin.h"
#include "FlowGraphNode.generated.h"

class UEdGraphSchema;

class UFlowNode;

USTRUCT()
struct FLOWEDITOR_API FFlowBreakpoint
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool bHasBreakpoint;

	bool bBreakpointEnabled;
	bool bBreakpointHit;

	FFlowBreakpoint()
	{
		bHasBreakpoint = false;
		bBreakpointEnabled = false;
		bBreakpointHit = false;
	};

	void AddBreakpoint();
	void RemoveBreakpoint();
	bool HasBreakpoint() const;

	void EnableBreakpoint();
	bool CanEnableBreakpoint() const;

	void DisableBreakpoint();
	bool IsBreakpointEnabled() const;

	void ToggleBreakpoint();
};

/**
 * Graph representation of the Flow Node
 */
UCLASS()
class FLOWEDITOR_API UFlowGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

//////////////////////////////////////////////////////////////////////////
// Flow node

private:
	UPROPERTY(Instanced)
	UFlowNode* FlowNode;

	bool bBlueprintCompilationPending;
	bool bNeedsFullReconstruction;
	static bool bFlowAssetsLoaded;

public:
	// It would be intuitive to assign a custom Graph Node class in Flow Node class
	// However, we shouldn't assign class from editor module to runtime module class
	UPROPERTY()
	TArray<TSubclassOf<UFlowNode>> AssignedNodeClasses;
	
	void SetFlowNode(UFlowNode* InFlowNode);
	UFlowNode* GetFlowNode() const;

	// UObject
	virtual void PostLoad() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostEditImport() override;
	// --

	// UEdGraphNode
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
    // --
	
	void PostCopyNode();

private:
	void SubscribeToExternalChanges();

	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	void OnBlueprintCompiled();

	void OnExternalChange();

//////////////////////////////////////////////////////////////////////////
// Graph node

public:
	UPROPERTY()
	FFlowBreakpoint NodeBreakpoint;

	// UEdGraphNode
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	// --

	/**
	 * Handles inserting the node between the FromPin and what the FromPin was original connected to
	 *
	 * @param FromPin			The pin this node is being spawned from
	 * @param NewLinkPin		The new pin the FromPin will connect to
	 * @param OutNodeList		Any nodes that are modified will get added to this list for notification purposes
	 */
	void InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList);

	// UEdGraphNode
	virtual void ReconstructNode() override;
	virtual void AllocateDefaultPins() override;
	// --

	// variants of K2Node methods
	void RewireOldPinsToNewPins(TArray<UEdGraphPin*>& InOldPins);
	void ReconstructSinglePin(UEdGraphPin* NewPin, UEdGraphPin* OldPin);
	// --

	// UEdGraphNode
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual bool CanDuplicateNode() const override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool ShowPaletteIconOnNode() const override { return true; }
	virtual FText GetTooltipText() const override;
	// --

//////////////////////////////////////////////////////////////////////////
// Utils

public:
	// Short summary of node's content
	FString GetNodeDescription() const;

	// Get flow node for the inspected asset instance
	UFlowNode* GetInspectedNodeInstance() const;

	// Used for highlighting active nodes of the inspected asset instance
	EFlowNodeState GetActivationState() const;

	// Information displayed while node is active
	FString GetStatusString() const;
	FLinearColor GetStatusBackgroundColor() const;

	// Check this to display information while node is preloaded
	bool IsContentPreloaded() const;

	bool CanFocusViewport() const;

	// UEdGraphNode
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	// --

//////////////////////////////////////////////////////////////////////////
// Pins

public:
	TArray<UEdGraphPin*> InputPins;
	TArray<UEdGraphPin*> OutputPins;

	UPROPERTY()
	TMap<FEdGraphPinReference, FFlowBreakpoint> PinBreakpoints;

	void CreateInputPin(const FFlowPin& FlowPin, const int32 Index = INDEX_NONE);
	void CreateOutputPin(const FFlowPin& FlowPin, const int32 Index = INDEX_NONE);

	void RemoveOrphanedPin(UEdGraphPin* Pin);

	bool SupportsContextPins() const;

	bool CanUserAddInput() const;
	bool CanUserAddOutput() const;

	bool CanUserRemoveInput(const UEdGraphPin* Pin) const;
	bool CanUserRemoveOutput(const UEdGraphPin* Pin) const;

	void AddUserInput();
	void AddUserOutput();

	// Add pin only on this instance of node, under default pins
	void AddInstancePin(const EEdGraphPinDirection Direction, const FName& PinName);

	// Call node and graph updates manually, if using bBatchRemoval
	void RemoveInstancePin(UEdGraphPin* Pin);

	// Create pins from the context asset, i.e. Sequencer events
	void RefreshContextPins(const bool bReconstructNode);

	// UEdGraphNode
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	// --

//////////////////////////////////////////////////////////////////////////
// Breakpoints

public:
	void OnInputTriggered(const int32 Index);
	void OnOutputTriggered(const int32 Index);

private:
	void TryPausingSession(bool bPauseSession);

	void OnResumePIE(const bool bIsSimulating);
	void OnEndPIE(const bool bIsSimulating);
	void ResetBreakpoints();

//////////////////////////////////////////////////////////////////////////
// Execution Override

public:
	// Pin activation forced by user during PIE
	void ForcePinActivation(const FEdGraphPinReference PinReference) const;
};
