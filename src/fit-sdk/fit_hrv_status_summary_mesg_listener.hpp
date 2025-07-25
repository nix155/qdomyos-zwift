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


#if !defined(FIT_HRV_STATUS_SUMMARY_MESG_LISTENER_HPP)
#define FIT_HRV_STATUS_SUMMARY_MESG_LISTENER_HPP

#include "fit_hrv_status_summary_mesg.hpp"

namespace fit
{

class HrvStatusSummaryMesgListener
{
public:
    virtual ~HrvStatusSummaryMesgListener() {}
    virtual void OnMesg(HrvStatusSummaryMesg& mesg) = 0;
};

} // namespace fit

#endif // !defined(FIT_HRV_STATUS_SUMMARY_MESG_LISTENER_HPP)
