/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2025 Garmin International, Inc.
// Licensed under the Flexible and Interoperable Data Transfer (FIT) Protocol License; you
// may not use this file except in compliance with the Flexible and Interoperable Data
// Transfer (FIT) Protocol License.
/////////////////////////////////////////////////////////////////////////////////////////////
// ****WARNING****  This file is auto-generated!  Do NOT edit this file.
// Profile Version = 21.171.0Release
// Tag = production/release/21.171.0-0-g57fed75
/////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(FIT_TRAINING_SETTINGS_MESG_LISTENER_HPP)
#define FIT_TRAINING_SETTINGS_MESG_LISTENER_HPP

#include "fit_training_settings_mesg.hpp"

namespace fit
{

class TrainingSettingsMesgListener
{
public:
    virtual ~TrainingSettingsMesgListener() {}
    virtual void OnMesg(TrainingSettingsMesg& mesg) = 0;
};

} // namespace fit

#endif // !defined(FIT_TRAINING_SETTINGS_MESG_LISTENER_HPP)
