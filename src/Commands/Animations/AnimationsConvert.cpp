/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsConvert.hpp"
#include "File/Animations/AnimationFile.hpp"
#include "Settings/Games.hpp"

namespace CAO {
CommandResult AnimationsConvert::process(File &file) const
{
    auto *havok = file.getFile<Resources::Animation>(true);
    if (!havok)
        return CommandResultFactory::getCannotCastFileResult();

    havok->pkFormat = *GameSettings::get(currentProfile().getGeneralSettings().eGame()).eAnimationsFormat();
    return CommandResultFactory::getSuccessfulResult();
}

CommandState AnimationsConvert::isApplicable(File &file) const
{
    if (!file.patternSettings().bAnimationsOptimization())
        return CommandState::NotRequired;

    const auto &format = GameSettings::get(currentProfile().getGeneralSettings().eGame()).eAnimationsFormat();

    if (!format.has_value())
        return CommandState::NotRequired;

    const auto *havok = file.getFile<Resources::Animation>();
    if (!havok)
        return CommandState::NotRequired;

    if (havok->pkFormat == format.value())
        return CommandState::NotRequired;

    return CommandState::Ready;
}

} // namespace CAO
