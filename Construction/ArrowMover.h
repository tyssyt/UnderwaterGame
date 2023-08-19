// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ArrowMover.generated.h"

DECLARE_DELEGATE(FOnArrowMovedEvent);

UCLASS(Abstract)
class XD_API UArrowMover : public UStaticMeshComponent {
    GENERATED_BODY()

public:
    UArrowMover();

    FOnArrowMovedEvent OnArrowMoved;

protected:
    virtual void BeginPlay() override;

    bool HasHover = false;
    
    UFUNCTION()
    void OnClick(UPrimitiveComponent* Target, FKey ButtonPressed);
    UFUNCTION()
    void OnHoverBegin(UPrimitiveComponent* Target);
    UFUNCTION()
    void OnHoverEnd(UPrimitiveComponent* Target);

    void Highlight();
    void Lowlight() const;

    virtual void Move() {}
    virtual void EndMove() {}

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* Material) override;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UArrowMoverLine : public UArrowMover {
    GENERATED_BODY()

    FVector MouseOffset = FVector(NAN);

public:
    UArrowMoverLine();

protected:
    virtual void Move() override;
    virtual void EndMove() override;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UArrowMoverUp : public UArrowMover {
    GENERATED_BODY()

    double MinZ = NAN;
    double MouseOffset = NAN;

public:
    UArrowMoverUp();

protected:
    virtual void Move() override;
    virtual void EndMove() override;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UArrowMoverRotate : public UArrowMover {
    GENERATED_BODY()

    double MouseOffset = NAN;

public:
    UArrowMoverRotate();

protected:
    virtual void Move() override;    
    virtual void EndMove() override;
};
