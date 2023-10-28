#pragma once

namespace Logging {
    static FString ToString(UObject* obj, bool includeEngineClasses = false, EFieldIterationFlags flags = EFieldIterationFlags::None);
}
