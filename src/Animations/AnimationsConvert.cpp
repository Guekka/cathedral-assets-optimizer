/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsConvert.hpp"

namespace CAO {
AnimationsConvert::AnimationsConvert()
{
    _name = "Animation Convert";
    _type = CommandType::Animation;
    _priority = Medium;
}

CommandResult AnimationsConvert::process(File &file, const Settings &settings)
{
    auto havok = dynamic_cast<const AnimationResource *>(&file.getFile());
    if (!havok)
        return _resultFactory.getCannotCastFileResult();

    AnimationResource *res = new AnimationResource(*havok);
    res->pkFormat = settings.getMandatoryValue<hkPackFormat>(AdvancedSettings::eAnimationsFormat);
    file.setFile(*res);
    return _resultFactory.getSuccessfulResult();
}

bool AnimationsConvert::isApplicable(File &file, const Settings &settings)
{
    if (!settings.getMandatoryValue<bool>(StandardSettings::bAnimationsOptimization))
        return false;

    auto havok = dynamic_cast<const AnimationResource *>(&file.getFile());
    if (!havok)
        return false;

    if (havok->pkFormat == settings.getMandatoryValue<hkPackFormat>(AdvancedSettings::eAnimationsFormat))
        return false;

    return true;
}

} // namespace CAO
