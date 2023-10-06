#include "Utils.h"

template <class IntType>
bool IntToString(FProperty* prop, UObject* obj, FString& string) {
    if (const auto intProp = CastField<TProperty_Numeric<IntType>>(prop)) {
        string += FString::Printf(TEXT("%d"), *intProp->template ContainerPtrToValuePtr<IntType>(obj));
        return true;
    }
    return false;
}
template <class FloatType>
bool FloatToString(FProperty* prop, UObject* obj, FString& string) {
    if (const auto floatProp = CastField<TProperty_Numeric<FloatType>>(prop)) {
        string += FString::Printf(TEXT("%f"), *floatProp->template ContainerPtrToValuePtr<FloatType>(obj));
        return true;
    }
    return false;
}

FString Logging::ToString(UObject* obj, bool includeEngineClasses, EFieldIterationFlags flags) {
    if (!obj)
        return TEXT("null");

    if (!includeEngineClasses) {
        const auto packageName = obj->GetClass()->GetPackage()->GetName();
        if (packageName.StartsWith(TEXT("/Engine")) || packageName.StartsWith(TEXT("/Script/Engine")))
            return obj->GetName() + "{UE}";
    }

    FString string = obj->GetName() + TEXT('{');
    
    for (TFieldIterator<FProperty> It(obj->GetClass(), flags); It; ++It) {
        const auto rawProp = *It;
        string += rawProp->GetName();
        string += TEXT('=');

        // obj pointer
        if (const auto objProp = CastField<FObjectProperty>(rawProp))
            string += ToString(*objProp->ContainerPtrToValuePtr<UObject*>(obj));
        // string types
        else if (const auto textProp = CastField<FTextProperty>(rawProp))
            string += *textProp->ContainerPtrToValuePtr<FText>(obj)->ToString();
        else if (const auto nameProp = CastField<FNameProperty>(rawProp))
            string += *nameProp->ContainerPtrToValuePtr<FName>(obj)->ToString();
        else if (const auto strProp = CastField<FStrProperty>(rawProp))
            string += *strProp->ContainerPtrToValuePtr<FString>(obj);
        // int types
        else if (const auto boolProp = CastField<FIntProperty>(rawProp))
            string +=  *boolProp->ContainerPtrToValuePtr<bool>(obj) ? TEXT("true") : TEXT("false");
        else if (IntToString<int8>(rawProp, obj, string)) {}
        else if (IntToString<int16>(rawProp, obj, string)) {}
        else if (IntToString<int32>(rawProp, obj, string)) {}
        else if (IntToString<int64>(rawProp, obj, string)) {}
        else if (IntToString<uint8>(rawProp, obj, string)) {}
        else if (IntToString<uint16>(rawProp, obj, string)) {}
        else if (IntToString<uint32>(rawProp, obj, string)) {}
        else if (IntToString<uint64>(rawProp, obj, string)) {}
        // float types
        else if (FloatToString<float>(rawProp, obj, string)) {}
        else if (FloatToString<double>(rawProp, obj, string)) {}
        else
            string += TEXT("?");

        // TODO array, map, set, what about pair?
        
        string += TEXT(", ");
    }

    string.RemoveFromEnd(TEXT(", "));
    return string + TEXT('}');
}
