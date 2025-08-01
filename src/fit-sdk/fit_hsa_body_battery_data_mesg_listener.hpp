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


#if !defined(FIT_HSA_BODY_BATTERY_DATA_MESG_LISTENER_HPP)
#define FIT_HSA_BODY_BATTERY_DATA_MESG_LISTENER_HPP

#include "fit_hsa_body_battery_data_mesg.hpp"

namespace fit
{

class HsaBodyBatteryDataMesgListener
{
public:
    virtual ~HsaBodyBatteryDataMesgListener() {}
    virtual void OnMesg(HsaBodyBatteryDataMesg& mesg) = 0;
};

} // namespace fit

#endif // !defined(FIT_HSA_BODY_BATTERY_DATA_MESG_LISTENER_HPP)
