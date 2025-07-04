#include "qfit.h"

#include <cstdlib>
#include <fstream>
#include <ostream>
#include <QDir>

#include "QSettings"

#include "fit_date_time.hpp"
#include "fit_encode.hpp"

#include "fit_decode.hpp"
#include "fit_developer_field_description.hpp"
#include "fit_field_description_mesg.hpp"
#include "fit_developer_field.hpp"
#include "fit_mesg_broadcaster.hpp"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

using namespace std;

qfit::qfit(QObject *parent) : QObject(parent) {}

void qfit::save(const QString &filename, QList<SessionLine> session, bluetoothdevice::BLUETOOTH_TYPE type,
                uint32_t processFlag, FIT_SPORT overrideSport, QString workoutName, QString bluetooth_device_name) {
    QSettings settings;
    bool strava_virtual_activity =
        settings.value(QZSettings::strava_virtual_activity, QZSettings::default_strava_virtual_activity).toBool();
    bool strava_treadmill =
        settings.value(QZSettings::strava_treadmill, QZSettings::default_strava_treadmill).toBool();
    bool powr_sensor_running_cadence_half_on_strava =
        settings
            .value(QZSettings::powr_sensor_running_cadence_half_on_strava,
                   QZSettings::default_powr_sensor_running_cadence_half_on_strava)
            .toBool();
    std::list<fit::RecordMesg> records;
    fit::Encode encode(fit::ProtocolVersion::V20);
    if (session.isEmpty()) {
        return;
    }
    std::fstream file;
    uint32_t firstRealIndex = 0;
    for (int i = 0; i < session.length(); i++) {
        if ((session.at(i).speed > 0 && (type == bluetoothdevice::TREADMILL || type == bluetoothdevice::ELLIPTICAL)) ||
            (session.at(i).cadence > 0 && (type == bluetoothdevice::BIKE || type == bluetoothdevice::ROWING))) {
            firstRealIndex = i;
            break;
        }
    }
    double startingDistanceOffset = 0.0;
    if (!session.isEmpty()) {
        startingDistanceOffset = session.at(firstRealIndex).distance;
    }

#ifdef _WIN32
    file.open(QString(filename).toLocal8Bit().constData(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
#else
    file.open(filename.toStdString(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
#endif

    if (!file.is_open()) {
        qDebug() << "Error opening file stream";
        return;
    }

    bool fit_file_garmin_device_training_effect = settings.value(QZSettings::fit_file_garmin_device_training_effect, QZSettings::default_fit_file_garmin_device_training_effect).toBool();
    int fit_file_garmin_device_training_effect_device = settings.value(QZSettings::fit_file_garmin_device_training_effect_device, QZSettings::default_fit_file_garmin_device_training_effect_device).toInt();
    fit::FileIdMesg fileIdMesg; // Every FIT file requires a File ID message
    fileIdMesg.SetType(FIT_FILE_ACTIVITY);
    if(bluetooth_device_name.toUpper().startsWith("DOMYOS"))
        fileIdMesg.SetManufacturer(FIT_MANUFACTURER_DECATHLON);
    else {
        if(fit_file_garmin_device_training_effect)
            fileIdMesg.SetManufacturer(FIT_MANUFACTURER_GARMIN);
        else
            fileIdMesg.SetManufacturer(FIT_MANUFACTURER_DEVELOPMENT);
    }
    if(fit_file_garmin_device_training_effect) {
        fileIdMesg.SetProduct(fit_file_garmin_device_training_effect_device);
        fileIdMesg.SetSerialNumber(3313379353);
    } else {
        fileIdMesg.SetProduct(1);
        fileIdMesg.SetSerialNumber(12345);
    }
    fileIdMesg.SetTimeCreated(session.at(firstRealIndex).time.toSecsSinceEpoch() - 631065600L);

    fit::FileCreatorMesg fileCreatorMesg;
    if(fit_file_garmin_device_training_effect) {
        fileCreatorMesg.SetSoftwareVersion(975);
        fileCreatorMesg.SetHardwareVersion(255);
    } else {
        fileCreatorMesg.SetSoftwareVersion(2119);
    }

    fit::DeviceInfoMesg deviceInfoMesg;
    deviceInfoMesg.SetDeviceIndex(FIT_DEVICE_INDEX_CREATOR);
    if(fit_file_garmin_device_training_effect) {
        deviceInfoMesg.SetManufacturer(FIT_MANUFACTURER_GARMIN);
        deviceInfoMesg.SetSerialNumber(3313379353);
        deviceInfoMesg.SetProduct(fit_file_garmin_device_training_effect_device);
        deviceInfoMesg.SetGarminProduct(fit_file_garmin_device_training_effect_device);
        deviceInfoMesg.SetSoftwareVersion(21.19);
    } else {
        deviceInfoMesg.SetManufacturer(FIT_MANUFACTURER_DEVELOPMENT);
        deviceInfoMesg.SetSerialNumber(12345);
        deviceInfoMesg.SetProduct(1);
        deviceInfoMesg.SetSoftwareVersion(21.19);
    }
    deviceInfoMesg.SetSourceType(FIT_SOURCE_TYPE_LOCAL);

    bool gps_data = false;
    double max_alt = 0;
    double min_alt = 99999;
    double speed_acc = 0;
    int speed_count = 0;
    int lap_index = 0;
    double speed_avg = 0;
    for (int i = firstRealIndex; i < session.length(); i++) {
        if (session.at(i).coordinate.isValid()) {
            gps_data = true;
            break;
        }
    }
    for (int i = firstRealIndex; i < session.length(); i++) {
        if (gps_data) {
            if (session.at(i).coordinate.isValid()) {
                if (min_alt > session.at(i).coordinate.altitude())
                    min_alt = session.at(i).coordinate.altitude();
                if (max_alt < session.at(i).coordinate.altitude())
                    max_alt = session.at(i).coordinate.altitude();
            }
        } else {
            min_alt = 0;
            if (max_alt < session.at(i).elevationGain)
                max_alt = session.at(i).elevationGain;
        }

        if (session.at(i).speed > 0) {
            speed_count++;
            speed_acc += session.at(i).speed;
        }
    }

    if (speed_count > 0) {
        speed_avg = speed_acc / ((double)speed_count);
        qDebug() << "average speed from the fit file" << speed_avg;
    }

    fit::SessionMesg sessionMesg;
    sessionMesg.SetTimestamp(session.at(firstRealIndex).time.toSecsSinceEpoch() - 631065600L);
    sessionMesg.SetStartTime(session.at(firstRealIndex).time.toSecsSinceEpoch() - 631065600L);
    sessionMesg.SetTotalElapsedTime(session.last().elapsedTime);
    sessionMesg.SetTotalTimerTime(session.last().time.toSecsSinceEpoch() -
                                  session.at(firstRealIndex).time.toSecsSinceEpoch());
    sessionMesg.SetTotalDistance((session.last().distance - startingDistanceOffset) * 1000.0); // meters
    sessionMesg.SetTotalCalories(session.last().calories);
    sessionMesg.SetTotalMovingTime(session.last().elapsedTime);
    sessionMesg.SetMinAltitude(min_alt);
    sessionMesg.SetMaxAltitude(max_alt);
    sessionMesg.SetEvent(FIT_EVENT_SESSION);
    sessionMesg.SetEventType(FIT_EVENT_TYPE_STOP);
    sessionMesg.SetFirstLapIndex(0);
    sessionMesg.SetTrigger(FIT_SESSION_TRIGGER_ACTIVITY_END);
    sessionMesg.SetMessageIndex(FIT_MESSAGE_INDEX_RESERVED);

    if (overrideSport != FIT_SPORT_INVALID) {
        sessionMesg.SetSport(overrideSport);
        sessionMesg.SetSubSport(FIT_SUB_SPORT_GENERIC);
        qDebug() << "overriding FIT sport " << overrideSport;
    } else if (type == bluetoothdevice::TREADMILL) {
        if(session.last().stepCount > 0)
            sessionMesg.SetTotalStrides(session.last().stepCount);

        if (speed_avg == 0 || speed_avg > 6.5 || strava_virtual_activity)
            sessionMesg.SetSport(FIT_SPORT_RUNNING);
        else
            sessionMesg.SetSport(FIT_SPORT_WALKING);

        if (strava_virtual_activity) {
            sessionMesg.SetSubSport(FIT_SUB_SPORT_VIRTUAL_ACTIVITY);
        } else {
            if(strava_treadmill)
                sessionMesg.SetSubSport(FIT_SUB_SPORT_TREADMILL);
        }
    } else if (type == bluetoothdevice::ELLIPTICAL) {
        if (strava_virtual_activity) {
            if (speed_avg == 0 || speed_avg > 6.5)
                sessionMesg.SetSport(FIT_SPORT_RUNNING);
            else
                sessionMesg.SetSport(FIT_SPORT_WALKING);
            sessionMesg.SetSubSport(FIT_SUB_SPORT_VIRTUAL_ACTIVITY);
        } else {
            sessionMesg.SetSport(FIT_SPORT_FITNESS_EQUIPMENT);
            sessionMesg.SetSubSport(FIT_SUB_SPORT_ELLIPTICAL);
        }
    } else if (type == bluetoothdevice::ROWING) {

        sessionMesg.SetSport(FIT_SPORT_ROWING);
        sessionMesg.SetSubSport(FIT_SUB_SPORT_INDOOR_ROWING);
        if (session.last().totalStrokes)
            sessionMesg.SetTotalStrokes(session.last().totalStrokes);
        if (session.last().avgStrokesRate)
            sessionMesg.SetAvgStrokeCount(session.last().avgStrokesRate);
        if (session.last().maxStrokesRate)
            sessionMesg.SetMaxCadence(session.last().maxStrokesRate);
        if (session.last().avgStrokesLength)
            sessionMesg.SetAvgStrokeDistance(session.last().avgStrokesLength);
    } else if (type == bluetoothdevice::STAIRCLIMBER) {

        sessionMesg.SetSport(FIT_SPORT_GENERIC);
        sessionMesg.SetSubSport(FIT_SUB_SPORT_STAIR_CLIMBING);
    } else if (type == bluetoothdevice::JUMPROPE) {

        sessionMesg.SetSport(FIT_SPORT_JUMP_ROPE);
        sessionMesg.SetSubSport(FIT_SUB_SPORT_GENERIC);
        if (session.last().stepCount)
            sessionMesg.SetJumpCount(session.last().stepCount);
    } else {

        sessionMesg.SetSport(FIT_SPORT_CYCLING);
        if (strava_virtual_activity) {
            sessionMesg.SetSubSport(FIT_SUB_SPORT_VIRTUAL_ACTIVITY);
        }
    }

    fit::DeveloperDataIdMesg devIdMesg;
    // QZ companion app
    // 3746ce54-a9e9-42b0-9be9-b867f8d20f7d
    // Core App
    // 6957fe68-83fe-4ed6-8613-413f70624bb5
    devIdMesg.SetApplicationId(0, 0x37);
    devIdMesg.SetApplicationId(1, 0x46);
    devIdMesg.SetApplicationId(2, 0xce);
    devIdMesg.SetApplicationId(3, 0x54);
    devIdMesg.SetApplicationId(4, 0xa9);
    devIdMesg.SetApplicationId(5, 0xe9);
    devIdMesg.SetApplicationId(6, 0x42);
    devIdMesg.SetApplicationId(7, 0xb0);
    devIdMesg.SetApplicationId(8, 0x9b);
    devIdMesg.SetApplicationId(9, 0xe9);
    devIdMesg.SetApplicationId(10, 0xb8);
    devIdMesg.SetApplicationId(11, 0x67);
    devIdMesg.SetApplicationId(12, 0xf8);
    devIdMesg.SetApplicationId(13, 0xd2);
    devIdMesg.SetApplicationId(14, 0x0f);
    devIdMesg.SetApplicationId(15, 0x7d);
    devIdMesg.SetDeveloperDataIndex(0);
    devIdMesg.SetApplicationVersion(70);

           // Create developer field descriptions for custom temperature fields
    fit::FieldDescriptionMesg coreTemperatureFieldDesc;
    coreTemperatureFieldDesc.SetDeveloperDataIndex(0);
    coreTemperatureFieldDesc.SetFieldDefinitionNumber(0);
    coreTemperatureFieldDesc.SetFitBaseTypeId(FIT_BASE_TYPE_FLOAT32);
    coreTemperatureFieldDesc.SetFieldName(0, L"core_temperature");
    coreTemperatureFieldDesc.SetUnits(0, L"°C");
        coreTemperatureFieldDesc.SetNativeMesgNum(FIT_MESG_NUM_RECORD);
    coreTemperatureFieldDesc.SetNativeFieldNum(139);

    fit::FieldDescriptionMesg skinTemperatureFieldDesc;
    skinTemperatureFieldDesc.SetDeveloperDataIndex(0);
    skinTemperatureFieldDesc.SetFieldDefinitionNumber(1);
    skinTemperatureFieldDesc.SetFitBaseTypeId(FIT_BASE_TYPE_FLOAT32);
    skinTemperatureFieldDesc.SetFieldName(0, L"skin_temperature");
    skinTemperatureFieldDesc.SetUnits(0, L"°C");
        skinTemperatureFieldDesc.SetNativeMesgNum(FIT_MESG_NUM_RECORD);
    skinTemperatureFieldDesc.SetNativeFieldNum(255); // Use invalid field number to indicate custom field

    fit::FieldDescriptionMesg heatStrainIndexFieldDesc;
    heatStrainIndexFieldDesc.SetDeveloperDataIndex(0);
    heatStrainIndexFieldDesc.SetFieldDefinitionNumber(2);
    heatStrainIndexFieldDesc.SetFitBaseTypeId(FIT_BASE_TYPE_FLOAT32);
    heatStrainIndexFieldDesc.SetFieldName(0, L"heat_strain_index");
    heatStrainIndexFieldDesc.SetUnits(0, L"a.u.");
    heatStrainIndexFieldDesc.SetNativeMesgNum(FIT_MESG_NUM_RECORD);
    heatStrainIndexFieldDesc.SetNativeFieldNum(255); // Use invalid field number to indicate custom field

    fit::ActivityMesg activityMesg;
    activityMesg.SetTimestamp(session.at(firstRealIndex).time.toSecsSinceEpoch() - 631065600L);
    activityMesg.SetTotalTimerTime(session.last().elapsedTime);
    activityMesg.SetNumSessions(1);
    activityMesg.SetType(FIT_ACTIVITY_MANUAL);
    activityMesg.SetEvent(FIT_EVENT_WORKOUT);
    activityMesg.SetEventType(FIT_EVENT_TYPE_START);
    activityMesg.SetLocalTimestamp(fit::DateTime((time_t)session.last().time.toSecsSinceEpoch())
                                       .GetTimeStamp()); // seconds since 00:00 Dec d31 1989 in local time zone
    activityMesg.SetEvent(FIT_EVENT_ACTIVITY);
    activityMesg.SetEventType(FIT_EVENT_TYPE_STOP);

    fit::EventMesg eventMesg;
    eventMesg.SetEvent(FIT_EVENT_TIMER);
    eventMesg.SetEventType(FIT_EVENT_TYPE_START);
    eventMesg.SetData(0);
    eventMesg.SetEventGroup(0);
    eventMesg.SetTimestamp(session.at(firstRealIndex).time.toSecsSinceEpoch() - 631065600L);

    encode.Open(file);
    encode.Write(fileIdMesg);
    encode.Write(fileCreatorMesg);
    encode.Write(devIdMesg);
    encode.Write(coreTemperatureFieldDesc);
    encode.Write(skinTemperatureFieldDesc);
    encode.Write(heatStrainIndexFieldDesc);
    encode.Write(deviceInfoMesg);

    if (workoutName.length() > 0) {
        fit::TrainingFileMesg trainingFile;
        trainingFile.SetTimestamp(sessionMesg.GetTimestamp());
        trainingFile.SetTimeCreated(sessionMesg.GetTimestamp());
        trainingFile.SetType(FIT_FILE_WORKOUT);
        encode.Write(trainingFile);

        fit::WorkoutMesg workout;
        workout.SetSport(sessionMesg.GetSport());
        workout.SetSubSport(sessionMesg.GetSubSport());
#ifndef _WIN32
        workout.SetWktName(workoutName.toStdWString());
#endif
        workout.SetNumValidSteps(1);
        encode.Write(workout);

        fit::WorkoutStepMesg workoutStep;
        workoutStep.SetDurationTime(sessionMesg.GetTotalTimerTime());
        workoutStep.SetTargetValue(0);
        workoutStep.SetCustomTargetValueHigh(0);
        workoutStep.SetCustomTargetValueLow(0);
        workoutStep.SetMessageIndex(0);
        workoutStep.SetDurationType(FIT_WKT_STEP_DURATION_TIME);
        workoutStep.SetTargetType(FIT_WKT_STEP_TARGET_SPEED);
        workoutStep.SetIntensity(FIT_INTENSITY_INTERVAL);
        encode.Write(workoutStep);
    }

    encode.Write(eventMesg);

    fit::DateTime date((time_t)session.first().time.toSecsSinceEpoch());

    fit::LapMesg lapMesg;
    lapMesg.SetIntensity(FIT_INTENSITY_ACTIVE);
    lapMesg.SetStartTime(date.GetTimeStamp() + firstRealIndex);
    lapMesg.SetTimestamp(date.GetTimeStamp() + firstRealIndex);
    lapMesg.SetEvent(FIT_EVENT_WORKOUT);
    lapMesg.SetSubSport(FIT_SUB_SPORT_GENERIC);
    lapMesg.SetEventType(FIT_EVENT_TYPE_STOP);
    lapMesg.SetTotalElapsedTime(0);
    lapMesg.SetTotalTimerTime(0);
    if (overrideSport != FIT_SPORT_INVALID) {

        lapMesg.SetSport(FIT_SPORT_GENERIC);
    } else if (type == bluetoothdevice::TREADMILL) {

        lapMesg.SetSport(FIT_SPORT_RUNNING);
    } else if (type == bluetoothdevice::ELLIPTICAL) {

        lapMesg.SetSport(FIT_SPORT_RUNNING);
    } else if (type == bluetoothdevice::ROWING) {

        lapMesg.SetSport(FIT_SPORT_ROWING);
    } else if (type == bluetoothdevice::JUMPROPE) {

        lapMesg.SetSport(FIT_SPORT_JUMP_ROPE);
    } else if (type == bluetoothdevice::STAIRCLIMBER) {

        lapMesg.SetSport(FIT_SPORT_GENERIC);
        lapMesg.SetSubSport(FIT_SUB_SPORT_STAIR_CLIMBING);
    } else {

        lapMesg.SetSport(FIT_SPORT_CYCLING);
    }

    SessionLine sl;
    if (processFlag & QFIT_PROCESS_DISTANCENOISE) {
        double distanceOld = -1.0;
        int startIdx = -1;
        for (int i = firstRealIndex; i < session.length(); i++) {

            sl = session.at(i);
            if (sl.distance != distanceOld || i == session.length() - 1) {
                if (i == session.length() - 1 && sl.distance == distanceOld) {
                    i++;
                }
                if (startIdx >= 0) {
                    for (int j = startIdx; j < i; j++) {
                        session[j].distance += 0.1 * (j - startIdx) / (i - startIdx);
                    }
                }
                distanceOld = sl.distance;
                startIdx = i;
            }
        }
    }

    uint32_t lastLapTimer = 0;
    double lastLapOdometer = startingDistanceOffset;
    for (int i = firstRealIndex; i < session.length(); i++) {

        fit::RecordMesg newRecord;
        sl = session.at(i);
        // fit::DateTime date((time_t)session.at(i).time.toSecsSinceEpoch());
        newRecord.SetHeartRate(sl.heart);
        uint8_t cad = sl.cadence;
        if (powr_sensor_running_cadence_half_on_strava)
            cad = cad / 2;
        newRecord.SetCadence(cad);
        newRecord.SetDistance((sl.distance - startingDistanceOffset) * 1000.0); // meters
        newRecord.SetSpeed(sl.speed / 3.6);                                     // meter per second
        newRecord.SetPower(sl.watt);
        newRecord.SetResistance(sl.resistance);
        newRecord.SetCalories(sl.calories);
        if (type == bluetoothdevice::TREADMILL) {
            newRecord.SetStepLength(sl.instantaneousStrideLengthCM * 10);
            newRecord.SetVerticalOscillation(sl.verticalOscillationMM);
            newRecord.SetStanceTime(sl.groundContactMS);
        }

               // Add custom developer fields for temperature data
        if (sl.coreTemp) {
            fit::DeveloperField coreTemperatureField(coreTemperatureFieldDesc, devIdMesg);
            coreTemperatureField.SetFLOAT32Value((float)sl.coreTemp);
            newRecord.AddDeveloperField(coreTemperatureField);
        }
        if (sl.bodyTemp) {
            fit::DeveloperField skinTemperatureField(skinTemperatureFieldDesc, devIdMesg);
            skinTemperatureField.SetFLOAT32Value((float)sl.bodyTemp);
            newRecord.AddDeveloperField(skinTemperatureField);
        }
        if (sl.heatStrainIndex) {
            fit::DeveloperField heatStrainIndexField(heatStrainIndexFieldDesc, devIdMesg);
            heatStrainIndexField.SetFLOAT32Value((float)sl.heatStrainIndex);
            newRecord.AddDeveloperField(heatStrainIndexField);
        }

               // if a gps track contains a point without the gps information, it has to be discarded, otherwise the database
               // structure is corrupted and 2 tracks are saved in the FIT file causing mapping issue.
        if (!sl.coordinate.isValid() && gps_data) {
            continue;
        }

        if (sl.coordinate.isValid()) {
            newRecord.SetAltitude(sl.coordinate.altitude());
            newRecord.SetPositionLat(pow(2, 31) * (sl.coordinate.latitude()) / 180.0);
            newRecord.SetPositionLong(pow(2, 31) * (sl.coordinate.longitude()) / 180.0);
        } else {
            newRecord.SetAltitude(sl.elevationGain);
        }

               // using just the start point as reference in order to avoid pause time
               // strava ignore the elapsed field
               // this workaround could leads an accuracy issue.
        newRecord.SetTimestamp(date.GetTimeStamp() + i);
        encode.Write(newRecord);

        if (sl.lapTrigger) {

            lapMesg.SetTotalDistance((sl.distance - lastLapOdometer) * 1000.0); // meters
            lapMesg.SetTotalElapsedTime(sl.elapsedTime - lastLapTimer);
            lapMesg.SetTotalTimerTime(sl.elapsedTime - lastLapTimer);
            lapMesg.SetEvent(FIT_EVENT_LAP);
            lapMesg.SetEventType(FIT_EVENT_TYPE_STOP);
            lapMesg.SetMessageIndex(lap_index++);
            lapMesg.SetLapTrigger(FIT_LAP_TRIGGER_DISTANCE);
            if (type == bluetoothdevice::JUMPROPE)
                lapMesg.SetRepetitionNum(session.at(i - 1).inclination);
            lastLapTimer = sl.elapsedTime;
            lastLapOdometer = sl.distance;

            encode.Write(lapMesg);

            lapMesg.SetStartTime(date.GetTimeStamp() + i);
            lapMesg.SetTimestamp(date.GetTimeStamp() + i);
            lapMesg.SetEvent(FIT_EVENT_WORKOUT);
            lapMesg.SetEventType(FIT_EVENT_LAP);
        }
    }

    lapMesg.SetTotalDistance((session.last().distance - lastLapOdometer) * 1000.0); // meters
    lapMesg.SetTotalElapsedTime(session.last().elapsedTime - lastLapTimer);
    lapMesg.SetTotalTimerTime(session.last().elapsedTime - lastLapTimer);
    lapMesg.SetEvent(FIT_EVENT_LAP);
    lapMesg.SetEventType(FIT_EVENT_TYPE_STOP);
    lapMesg.SetLapTrigger(FIT_LAP_TRIGGER_SESSION_END);
    lapMesg.SetMessageIndex(lap_index++);
    encode.Write(lapMesg);
    encode.Write(sessionMesg);
    encode.Write(activityMesg);

    if (!encode.Close()) {

        printf("Error closing encode.\n");
        return;
    }
    file.close();

    printf("Encoded FIT file ExampleActivity.fit.\n");
    return;
}

class Listener : public fit::FileIdMesgListener,
                 public fit::UserProfileMesgListener,
                 public fit::MonitoringMesgListener,
                 public fit::DeviceInfoMesgListener,
                 public fit::MesgListener,
                 public fit::DeveloperFieldDescriptionListener,
                 public fit::RecordMesgListener {
  public:
    QList<SessionLine> *sessionOpening = nullptr;

    static void PrintValues(const fit::FieldBase &field) {
        for (FIT_UINT8 j = 0; j < (FIT_UINT8)field.GetNumValues(); j++) {
            // std::wcout << L"       Val" << j << L": ";
            switch (field.GetType()) {
            // Get float 64 values for numeric types to receive values that have
            // their scale and offset properly applied.
            case FIT_BASE_TYPE_ENUM:
            case FIT_BASE_TYPE_BYTE:
            case FIT_BASE_TYPE_SINT8:
            case FIT_BASE_TYPE_UINT8:
            case FIT_BASE_TYPE_SINT16:
            case FIT_BASE_TYPE_UINT16:
            case FIT_BASE_TYPE_SINT32:
            case FIT_BASE_TYPE_UINT32:
            case FIT_BASE_TYPE_SINT64:
            case FIT_BASE_TYPE_UINT64:
            case FIT_BASE_TYPE_UINT8Z:
            case FIT_BASE_TYPE_UINT16Z:
            case FIT_BASE_TYPE_UINT32Z:
            case FIT_BASE_TYPE_UINT64Z:
            case FIT_BASE_TYPE_FLOAT32:
            case FIT_BASE_TYPE_FLOAT64:
                // std::wcout << field.GetFLOAT64Value(j);
                break;
            case FIT_BASE_TYPE_STRING:
                // std::wcout << field.GetSTRINGValue(j);
                break;
            default:
                break;
            }
            // std::wcout << L" " << field.GetUnits().c_str() << L"\n";;
        }
    }

    void OnMesg(fit::Mesg &mesg) {
        // printf("On Mesg:\n");
        // std::wcout << L"   New Mesg: " << mesg.GetName().c_str() << L".  It has " << mesg.GetNumFields() << L"
        // field(s) and " << mesg.GetNumDevFields() << " developer field(s).\n";

        for (FIT_UINT16 i = 0; i < (FIT_UINT16)mesg.GetNumFields(); i++) {
            fit::Field *field = mesg.GetFieldByIndex(i);
            // std::wcout << L"   Field" << i << " (" << field->GetName().c_str() << ") has " << field->GetNumValues()
            // << L" value(s)\n";
            PrintValues(*field);
        }

        for (auto devField : mesg.GetDeveloperFields()) {
            // std::wcout << L"   Developer Field(" << devField.GetName().c_str() << ") has " << devField.GetNumValues()
            // << L" value(s)\n";
            PrintValues(devField);
        }
    }

    void OnMesg(fit::FileIdMesg &mesg) {
        printf("File ID:\n");
        if (mesg.IsTypeValid())
            printf("   Type: %d\n", mesg.GetType());
        if (mesg.IsManufacturerValid())
            printf("   Manufacturer: %d\n", mesg.GetManufacturer());
        if (mesg.IsProductValid())
            printf("   Product: %d\n", mesg.GetProduct());
        if (mesg.IsSerialNumberValid())
            printf("   Serial Number: %u\n", mesg.GetSerialNumber());
        if (mesg.IsNumberValid())
            printf("   Number: %d\n", mesg.GetNumber());
    }

    void OnMesg(fit::UserProfileMesg &mesg) {
        printf("User profile:\n");
        if (mesg.IsFriendlyNameValid())
            // std::wcout << L"   Friendly Name: " << mesg.GetFriendlyName().c_str() << L"\n";
            if (mesg.GetGender() == FIT_GENDER_MALE)
                printf("   Gender: Male\n");
        if (mesg.GetGender() == FIT_GENDER_FEMALE)
            printf("   Gender: Female\n");
        if (mesg.IsAgeValid())
            printf("   Age [years]: %d\n", mesg.GetAge());
        if (mesg.IsWeightValid())
            printf("   Weight [kg]: %0.2f\n", mesg.GetWeight());
    }

    void OnMesg(fit::DeviceInfoMesg &mesg) {
        printf("Device info:\n");

        if (mesg.IsTimestampValid())
            printf("   Timestamp: %d\n", mesg.GetTimestamp());

        switch (mesg.GetBatteryStatus()) {
        case FIT_BATTERY_STATUS_CRITICAL:
            printf("   Battery status: Critical\n");
            break;
        case FIT_BATTERY_STATUS_GOOD:
            printf("   Battery status: Good\n");
            break;
        case FIT_BATTERY_STATUS_LOW:
            printf("   Battery status: Low\n");
            break;
        case FIT_BATTERY_STATUS_NEW:
            printf("   Battery status: New\n");
            break;
        case FIT_BATTERY_STATUS_OK:
            printf("   Battery status: OK\n");
            break;
        default:
            printf("   Battery status: Invalid\n");
            break;
        }
    }

    void OnMesg(fit::MonitoringMesg &mesg) {
        printf("Monitoring:\n");

        if (mesg.IsTimestampValid()) {
            printf("   Timestamp: %d\n", mesg.GetTimestamp());
        }

        if (mesg.IsActivityTypeValid()) {
            printf("   Activity type: %d\n", mesg.GetActivityType());
        }

        switch (mesg.GetActivityType()) // The Cycling field is dynamic
        {
        case FIT_ACTIVITY_TYPE_WALKING:
        case FIT_ACTIVITY_TYPE_RUNNING: // Intentional fallthrough
            if (mesg.IsStepsValid()) {
                printf("   Steps: %d\n", mesg.GetSteps());
            }
            break;
        case FIT_ACTIVITY_TYPE_CYCLING:
        case FIT_ACTIVITY_TYPE_SWIMMING: // Intentional fallthrough
            if (mesg.IsStrokesValid()) {
                printf("Strokes: %d\n", mesg.GetStrokes());
            }
            break;
        default:
            if (mesg.IsCyclesValid()) {
                printf("Cycles: %d\n", mesg.GetCycles());
            }
            break;
        }
    }

    static void PrintOverrideValues(const fit::Mesg &mesg, FIT_UINT8 fieldNum) {
        std::vector<const fit::FieldBase *> fields = mesg.GetOverrideFields(fieldNum);
        const fit::Profile::FIELD *profileField = fit::Profile::GetField(mesg.GetNum(), fieldNum);
        FIT_BOOL namePrinted = FIT_FALSE;

        for (const fit::FieldBase *field : fields) {
            if (!namePrinted) {
                printf("   %s:\n", profileField->name.c_str());
                namePrinted = FIT_TRUE;
            }

            if (FIT_NULL != dynamic_cast<const fit::Field *>(field)) {
                // Native Field
                printf("      native: ");
            } else {
                // Developer Field
                printf("      override: ");
            }

            switch (field->GetType()) {
            // Get float 64 values for numeric types to receive values that have
            // their scale and offset properly applied.
            case FIT_BASE_TYPE_ENUM:
            case FIT_BASE_TYPE_BYTE:
            case FIT_BASE_TYPE_SINT8:
            case FIT_BASE_TYPE_UINT8:
            case FIT_BASE_TYPE_SINT16:
            case FIT_BASE_TYPE_UINT16:
            case FIT_BASE_TYPE_SINT32:
            case FIT_BASE_TYPE_UINT32:
            case FIT_BASE_TYPE_SINT64:
            case FIT_BASE_TYPE_UINT64:
            case FIT_BASE_TYPE_UINT8Z:
            case FIT_BASE_TYPE_UINT16Z:
            case FIT_BASE_TYPE_UINT32Z:
            case FIT_BASE_TYPE_UINT64Z:
            case FIT_BASE_TYPE_FLOAT32:
            case FIT_BASE_TYPE_FLOAT64:
                printf("%f\n", field->GetFLOAT64Value());
                break;
            case FIT_BASE_TYPE_STRING:
                printf("%ls\n", field->GetSTRINGValue().c_str());
                break;
            default:
                break;
            }
        }
    }

    void OnMesg(fit::RecordMesg &record) override {
        if (sessionOpening != nullptr) {
            SessionLine s;
            s.heart = record.GetHeartRate();
            s.cadence = record.GetCadence();
            s.distance = record.GetDistance() / 1000;
            s.speed = record.GetSpeed() * 3.6;
            s.watt = record.GetPower();
            s.resistance = record.GetResistance();
            s.calories = record.GetCalories();
            s.instantaneousStrideLengthCM = record.GetStepLength() / 10;
            s.verticalOscillationMM = record.GetVerticalOscillation();
            s.groundContactMS = record.GetStanceTime();
            s.coordinate.setAltitude(record.GetAltitude());
            s.coordinate.setLatitude((record.GetPositionLat() * 180) / pow(2, 31));
            s.coordinate.setLongitude((record.GetPositionLong() * 180) / pow(2, 31));
            if (!s.coordinate.isValid()) {
                s.elevationGain = record.GetAltitude();
            }
            s.time = QDateTime::fromSecsSinceEpoch(record.GetTimestamp());
            sessionOpening->append(s);
        }
    }

    void OnDeveloperFieldDescription(const fit::DeveloperFieldDescription &desc) override {
        printf("New Developer Field Description\n");
        printf("   App Version: %d\n", desc.GetApplicationVersion());
        printf("   Field Number: %d\n", desc.GetFieldDefinitionNumber());
    }
};

void qfit::open(const QString &filename, QList<SessionLine> *output) {
    std::fstream file;
#ifdef _WIN32
    file.open(QString(filename).toLocal8Bit().constData(), std::ios::in | std::ios::binary);
#else
    file.open(filename.toStdString(), std::ios::in | std::ios::binary);
#endif

    if (!file.is_open()) {

        std::system_error(errno, std::system_category(), "failed to open " + filename.toStdString());
        qDebug() << "opened " << filename << errno;
        printf("Error opening file ExampleActivity.fit\n");
        return;
    }

    fit::Decode decode;
    std::istream &s = file;
    fit::MesgBroadcaster mesgBroadcaster;
    Listener listener;
    listener.sessionOpening = output;
    mesgBroadcaster.AddListener((fit::FileIdMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::UserProfileMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::MonitoringMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::DeviceInfoMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::RecordMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::MesgListener &)listener);
    decode.Read(&s, &mesgBroadcaster, &mesgBroadcaster, &listener);

    file.close();
}
