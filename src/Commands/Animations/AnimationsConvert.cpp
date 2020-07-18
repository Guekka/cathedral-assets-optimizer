/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsConvert.hpp"

namespace CAO {
CommandResult AnimationsConvert::process(File &file) const
{
    auto havok = dynamic_cast<AnimationResource *>(&file.getFile(true));
    if (!havok)
        return _resultFactory.getCannotCastFileResult();

    havok->pkFormat = file.patternSettings().eAnimationsFormat();
    return _resultFactory.getSuccessfulResult();
}

bool AnimationsConvert::isApplicable(File &file) const
{
    if (!file.patternSettings().bAnimationsOptimization())
        return false;

    auto havok = dynamic_cast<const AnimationResource *>(&file.getFile());
    if (!havok)
        return false;

    if (havok->pkFormat == file.patternSettings().eAnimationsFormat())
        return false;

    return true;
}

} // namespace CAO
