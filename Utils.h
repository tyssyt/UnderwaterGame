#pragma once
#include "CommonUILibrary.h"

class UWidget;

namespace UI {
    template <class T> static T* GetParentWidget(UWidget* Widget) {
        return Cast<T>(UCommonUILibrary::FindParentWidgetOfType(Widget, T::StaticClass()));
    }
}
