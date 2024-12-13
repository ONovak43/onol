#include "dynamic_types.hpp"

bool areEqual(const Object* lhs, const Object* rhs) {
    if (lhs == rhs) {
        return true;
    }

    if (!lhs || !rhs) {
        return false;
    }

    if (auto lhsString = dynamic_cast<const ObjString*>(lhs)) {
        if (auto rhsString = dynamic_cast<const ObjString*>(rhs)) {
            return lhsString->value == rhsString->value;
        }
    }

    return false;
}

