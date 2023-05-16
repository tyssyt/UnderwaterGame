// Fill out your copyright notice in the Description page of Project Settings.

#include "Conveyor.h"
#include "XD/Components/InventoryComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include <vector>
#include <utility>

AConveyor* AConveyor::Create(UWorld* world, ABuilding* source, ABuilding* target, int maxThroughput) {
    AConveyor* conveyor = world->SpawnActor<AConveyor>();
    conveyor->MaxThroughput = maxThroughput;
    conveyor->Source = source;
    conveyor->Target = target;
    conveyor->Connect();
    return conveyor;
}

AConveyor::AConveyor() {
    PrimaryActorTick.bCanEverTick = true;

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Cube")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
    Mesh->SetRelativeScale3D(FVector(1.f, .1f, 0.1f));
}

void AConveyor::BeginPlay() {
    Super::BeginPlay();
    Connect(); // for conveyors placed in the editor
}

void AConveyor::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (TargetInv) {
        // TODO only enable Tick if Source & Target Exists
        CurrentThroughput = TargetInv->PushInto(Buffer);
        Buffer -= CurrentThroughput;
    }

    if (SourceInv) {
        // TODO only enable Tick if Source & Target Exists
        Buffer += SourceInv->PullFrom(MaxThroughput - Buffer);
    }
}

void AConveyor::Connect() {
    // replace log with visual feedback
    SourceInv = nullptr;
    TargetInv = nullptr;

    if (!Source || !Target || Source == Target)
        return;

    // find all Resources that the Target accepts
    UInventoryComponent* targetInventory = Target->FindComponentByClass<UInventoryComponent>();
    if (!targetInventory)
        return;
    auto& inputs = targetInventory->GetInputs();

    // find all Resource that the Source sends
    UInventoryComponent* sourceInventory = Source->FindComponentByClass<UInventoryComponent>();
    if (!sourceInventory)
        return;
    auto& outputs = sourceInventory->GetOutputs();
    
    // find matching in/outputs
    std::vector<std::pair<FInventorySlot*, FInventorySlot*>> matches;
    bool hasUntypedInput = false;
    bool hasUntypedOutput = false;

    for (auto& output : outputs) {
        if (!output.Resource) {
            hasUntypedOutput = true;
            continue;
        }
        for (auto& input : inputs) {
            if (!input.Resource)
                hasUntypedInput = true;
            if (input.Resource == output.Resource)
                matches.push_back(std::make_pair(&output, &input));
        }
    }

    if (matches.size() == 0) { // no matches found, check for untyped in- or outputs
        if (hasUntypedInput && outputs.Num() == 1) {
            for (auto& input : inputs) {
                if (!input.Resource) {
                    FInventorySlot* output = &outputs[0];
                    input.Resource = output->Resource;
                    SourceInv = output;
                    TargetInv = &input;
                    break;
                }
            }
        } else if (hasUntypedOutput && inputs.Num() == 1) {
            for (auto& output : outputs) {
                if (!output.Resource) {
                    FInventorySlot* input = &inputs[0];
                    output.Resource = input->Resource;
                    SourceInv = &output;
                    TargetInv = input;
                    break;
                }
            }
        } else {
            UE_LOG(LogTemp, Warning, TEXT("No Matches found"));
            return;
        }
    } else if (matches.size() == 1) { // exactly one match found, connect them
        SourceInv = matches[0].first;
        TargetInv = matches[0].second;
    } else { // multiple matches found, show a dialog
        UE_LOG(LogTemp, Warning, TEXT("Multiple Matches found"));
        return; // TODO show a dialog to select the input
    }
    // TODO if the input/target is untyped, and there are multiple outputs/sources, also prompt
    // TODO think about if we want the vice versa of the above
    // TODO think about depot to depot connections... if neither of them is conneected to something else in the chain...

    // Visually Connect Source to Target
    const FVector sourceToTarget = Target->GetActorLocation() - Source->GetActorLocation();
    SetActorLocation((Source->GetActorLocation() + Target->GetActorLocation()) * .5f);
    SetActorRotation(FRotationMatrix::MakeFromX(sourceToTarget).Rotator());
    SetActorRelativeScale3D(FVector(sourceToTarget.Size() / 100.f, .1f, 0.1f));
}

void UConveyorUI::Tick() {
    if (Conveyor) {
        CurrentThroughput->SetText(FText::AsNumber(Conveyor->CurrentThroughput * 60)); // TODO use actual ticks/s
        Input->Set(Conveyor->SourceInv);
        Output->Set(Conveyor->TargetInv);
    }
}
