// Fill out your copyright notice in the Description page of Project Settings.

#include "TickTimer.h"

void UTickTimerHandle::Cancel() {
    Cast<UTickTimer>(GetOuter())->CancelTimer(this);
}

UTickTimerHandle* UTickTimer::AddTimer(int ticks) {
    const auto handle = NewObject<UTickTimerHandle>(this);
    handle->Ticks = ticks;
    Handles.Add(handle);
    return handle;
}

void UTickTimer::Tick(float DeltaTime) {
    // TODO if this never explodes, we can remove it at some point
    if (LastFrameNumberWeTicked == GFrameCounter) {
        checkNoEntry();
        return;
    }
    LastFrameNumberWeTicked = GFrameCounter;

    for (int i = Handles.Num()-1; i >= 0; i--) {
        if (--Handles[i]->Ticks < 0) {
            Handles[i]->OnTimerFinished.ExecuteIfBound();
            Handles.RemoveAtSwap(i);
        }
    }
}
