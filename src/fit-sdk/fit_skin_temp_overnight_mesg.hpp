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


#if !defined(FIT_SKIN_TEMP_OVERNIGHT_MESG_HPP)
#define FIT_SKIN_TEMP_OVERNIGHT_MESG_HPP

#include "fit_mesg.hpp"

namespace fit
{

class SkinTempOvernightMesg : public Mesg
{
public:
    class FieldDefNum final
    {
    public:
       static const FIT_UINT8 Timestamp = 253;
       static const FIT_UINT8 LocalTimestamp = 0;
       static const FIT_UINT8 AverageDeviation = 1;
       static const FIT_UINT8 Average7DayDeviation = 2;
       static const FIT_UINT8 NightlyValue = 4;
       static const FIT_UINT8 Invalid = FIT_FIELD_NUM_INVALID;
    };

    SkinTempOvernightMesg(void) : Mesg(Profile::MESG_SKIN_TEMP_OVERNIGHT)
    {
    }

    SkinTempOvernightMesg(const Mesg &mesg) : Mesg(mesg)
    {
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of timestamp field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsTimestampValid() const
    {
        const Field* field = GetField(253);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid();
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns timestamp field
    ///////////////////////////////////////////////////////////////////////
    FIT_DATE_TIME GetTimestamp(void) const
    {
        return GetFieldUINT32Value(253, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set timestamp field
    ///////////////////////////////////////////////////////////////////////
    void SetTimestamp(FIT_DATE_TIME timestamp)
    {
        SetFieldUINT32Value(253, timestamp, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of local_timestamp field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsLocalTimestampValid() const
    {
        const Field* field = GetField(0);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid();
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns local_timestamp field
    ///////////////////////////////////////////////////////////////////////
    FIT_LOCAL_DATE_TIME GetLocalTimestamp(void) const
    {
        return GetFieldUINT32Value(0, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set local_timestamp field
    ///////////////////////////////////////////////////////////////////////
    void SetLocalTimestamp(FIT_LOCAL_DATE_TIME localTimestamp)
    {
        SetFieldUINT32Value(0, localTimestamp, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of average_deviation field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsAverageDeviationValid() const
    {
        const Field* field = GetField(1);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid();
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns average_deviation field
    // Comment: The average overnight deviation from baseline temperature in degrees C
    ///////////////////////////////////////////////////////////////////////
    FIT_FLOAT32 GetAverageDeviation(void) const
    {
        return GetFieldFLOAT32Value(1, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set average_deviation field
    // Comment: The average overnight deviation from baseline temperature in degrees C
    ///////////////////////////////////////////////////////////////////////
    void SetAverageDeviation(FIT_FLOAT32 averageDeviation)
    {
        SetFieldFLOAT32Value(1, averageDeviation, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of average_7_day_deviation field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsAverage7DayDeviationValid() const
    {
        const Field* field = GetField(2);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid();
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns average_7_day_deviation field
    // Comment: The average 7 day overnight deviation from baseline temperature in degrees C
    ///////////////////////////////////////////////////////////////////////
    FIT_FLOAT32 GetAverage7DayDeviation(void) const
    {
        return GetFieldFLOAT32Value(2, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set average_7_day_deviation field
    // Comment: The average 7 day overnight deviation from baseline temperature in degrees C
    ///////////////////////////////////////////////////////////////////////
    void SetAverage7DayDeviation(FIT_FLOAT32 average7DayDeviation)
    {
        SetFieldFLOAT32Value(2, average7DayDeviation, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of nightly_value field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsNightlyValueValid() const
    {
        const Field* field = GetField(4);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid();
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns nightly_value field
    // Comment: Final overnight temperature value
    ///////////////////////////////////////////////////////////////////////
    FIT_FLOAT32 GetNightlyValue(void) const
    {
        return GetFieldFLOAT32Value(4, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set nightly_value field
    // Comment: Final overnight temperature value
    ///////////////////////////////////////////////////////////////////////
    void SetNightlyValue(FIT_FLOAT32 nightlyValue)
    {
        SetFieldFLOAT32Value(4, nightlyValue, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

};

} // namespace fit

#endif // !defined(FIT_SKIN_TEMP_OVERNIGHT_MESG_HPP)
