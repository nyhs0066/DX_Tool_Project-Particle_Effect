#include "CPGCMathWrap.h"

bool CPM_Ray::operator==(const CPM_Ray& other) const
{
    if (stPos == other.stPos && direction == other.direction)
    {
        if (fabs(ext - other.ext) < CPGC_FZERO_BOUND) { return true; }
    }

    return false;
}

bool CPM_Ray::operator!=(const CPM_Ray& other) const
{
    if (!(*this == other)) { return true; }

    return false;
}
