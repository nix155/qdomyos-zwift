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


#if !defined(FIT_HSA_SPO2_DATA_MESG_HPP)
#define FIT_HSA_SPO2_DATA_MESG_HPP

#include "fit_mesg.hpp"

namespace fit
{

class HsaSpo2DataMesg : public Mesg
{
public:
    class FieldDefNum final
    {
    public:
       static const FIT_UINT8 Timestamp = 253;
       static const FIT_UINT8 ProcessingInterval = 0;
       static const FIT_UINT8 ReadingSpo2 = 1;
       static const FIT_UINT8 Confidence = 2;
       static const FIT_UINT8 Invalid = FIT_FIELD_NUM_INVALID;
    };

    HsaSpo2DataMesg(void) : Mesg(Profile::MESG_HSA_SPO2_DATA)
    {
    }

    HsaSpo2DataMesg(const Mesg &mesg) : Mesg(mesg)
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
    // Units: s
    ///////////////////////////////////////////////////////////////////////
    FIT_DATE_TIME GetTimestamp(void) const
    {
        return GetFieldUINT32Value(253, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set timestamp field
    // Units: s
    ///////////////////////////////////////////////////////////////////////
    void SetTimestamp(FIT_DATE_TIME timestamp)
    {
        SetFieldUINT32Value(253, timestamp, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of processing_interval field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsProcessingIntervalValid() const
    {
        const Field* field = GetField(0);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid();
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns processing_interval field
    // Units: s
    // Comment: Processing interval length in seconds
    ///////////////////////////////////////////////////////////////////////
    FIT_UINT16 GetProcessingInterval(void) const
    {
        return GetFieldUINT16Value(0, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set processing_interval field
    // Units: s
    // Comment: Processing interval length in seconds
    ///////////////////////////////////////////////////////////////////////
    void SetProcessingInterval(FIT_UINT16 processingInterval)
    {
        SetFieldUINT16Value(0, processingInterval, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns number of reading_spo2
    ///////////////////////////////////////////////////////////////////////
    FIT_UINT8 GetNumReadingSpo2(void) const
    {
        return GetFieldNumValues(1, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of reading_spo2 field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsReadingSpo2Valid(FIT_UINT8 index) const
    {
        const Field* field = GetField(1);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid(index);
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns reading_spo2 field
    // Units: percent
    // Comment: SpO2 Reading: [70,100] Blank: 240
    ///////////////////////////////////////////////////////////////////////
    FIT_UINT8 GetReadingSpo2(FIT_UINT8 index) const
    {
        return GetFieldUINT8Value(1, index, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set reading_spo2 field
    // Units: percent
    // Comment: SpO2 Reading: [70,100] Blank: 240
    ///////////////////////////////////////////////////////////////////////
    void SetReadingSpo2(FIT_UINT8 index, FIT_UINT8 readingSpo2)
    {
        SetFieldUINT8Value(1, readingSpo2, index, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns number of confidence
    ///////////////////////////////////////////////////////////////////////
    FIT_UINT8 GetNumConfidence(void) const
    {
        return GetFieldNumValues(2, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Checks the validity of confidence field
    // Returns FIT_TRUE if field is valid
    ///////////////////////////////////////////////////////////////////////
    FIT_BOOL IsConfidenceValid(FIT_UINT8 index) const
    {
        const Field* field = GetField(2);
        if( FIT_NULL == field )
        {
            return FIT_FALSE;
        }

        return field->IsValueValid(index);
    }

    ///////////////////////////////////////////////////////////////////////
    // Returns confidence field
    // Comment: SpO2 Confidence: [0,254]
    ///////////////////////////////////////////////////////////////////////
    FIT_UINT8 GetConfidence(FIT_UINT8 index) const
    {
        return GetFieldUINT8Value(2, index, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

    ///////////////////////////////////////////////////////////////////////
    // Set confidence field
    // Comment: SpO2 Confidence: [0,254]
    ///////////////////////////////////////////////////////////////////////
    void SetConfidence(FIT_UINT8 index, FIT_UINT8 confidence)
    {
        SetFieldUINT8Value(2, confidence, index, FIT_SUBFIELD_INDEX_MAIN_FIELD);
    }

};

} // namespace fit

#endif // !defined(FIT_HSA_SPO2_DATA_MESG_HPP)
