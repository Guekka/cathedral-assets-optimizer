/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "source/Commands/Animations/AnimationsConvert.hpp"

namespace CAO {
AnimationsConvert::AnimationsConvert()
{
    _name = "Animation Convert";
    _type = CommandType::Animation;
    _priority = Medium;
}

CommandResult AnimationsConvert::process(File& file)
{
    auto havok = dynamic_cast<AnimationResource *>(&file.getFile(true));
    if (!havok)
        return _resultFactory.getCannotCastFileResult();

    havok->pkFormat = file.generalSettings().eAnimationsFormat();
    return _resultFactory.getSuccessfulResult();
}

bool AnimationsConvert::isApplicable(File& file)
{
    if (!file.patternSettings().bAnimationsOptimization())
        return false;

    auto havok = dynamic_cast<const AnimationResource *>(&file.getFile());
    if (!havok)
        return false;

    if (havok->pkFormat == file.generalSettings().eAnimationsFormat())
        return false;

    return true;
}

} // namespace CAO
