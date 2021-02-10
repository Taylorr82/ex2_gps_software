#include "skyTraq_binary.h"

#define header_size 4
#define footer_size 3
#include <string.h>
#include <stdlib.h>
#include "sci.h"
#include "skyTraq_binary.h"
#include "skytraq_binary_types.h"

//TODO: implement software download
//TODO: implement setting datum to WGS-84. Currently no datum configuration functions implemented
//TODO: determine if CNR mask will need to be configured
//TODO: determine if ephemeris needs to be uploaded manually
//TODO: determine if (and which) almanacs need to be uploaded manually
//TODO: find what the 3 timing modes mean

/* not implemented:
                    configure position pinning parameters
                    configure elevation and cnr mask
                    configure datum arbitrarily (assuming only WGS-84 will be needed in orbit)
                    configure DOP mask
                    configure 1pps cable delay
                    configure 1pps timing
                    set * almanac
                    set * ephemeris
                    set glonass time correction parameters
                    configure SBAS (seems to be mostly useful for planes)'
                    anything related to beidou

All functions associated with getting this information are implemented
This should help with determining if we need to implement those
*/

uint8_t calc_checksum(uint8_t *message, uint16_t payload_length) {
    // skip first 4 bytes of message
    message += 4;
    uint8_t checksum = 0;
    uint16_t i = 0;
    for (i; i<payload_length; i++) {
        checksum ^= *message;
    }
    return checksum;
}

bool skytraq_verify_checksum(uint8_t *message, uint16_t payload_length, uint8_t expected) {
    uint8_t cs = calc_checksum(message, payload_length);
    if (cs == expected) {
        return true;
    }
    return false;
}


// Sending a message will block until a reply is received
void skytraq_send_message(uint8_t *paylod, uint16_t size) {
    int total_size = size+header_size+footer_size;
    uint8_t *message = malloc(total_size);
    memset(message, 0, total_size);
    message[0] = 0xA0;
    message[1] = 0xA1;
    *(uint16_t *) &(message[2]) = size;
    memcpy(&(message[4]), paylod, size);
    message[total_size-3] = calc_checksum(message, size);
    message[total_size-1] = 0x0A;
    message[total_size-2] = 0x0D;

    sciSend(scilinREG, total_size, message);
}

void skytraq_restart_receiver(StartMode start_mode, uint16_t utc_year, uint8_t utc_month, uint8_t utc_day, uint8_t utc_hour, uint8_t utc_minute, uint8_t utc_second, int16_t latitude, int16_t longitude, int16_t altitude) {
        uint16_t length = 15;
        uint8_t payload[15];
        
        payload[0] = SYSTEM_RESTART;
        payload[1] = start_mode;
        *(uint16_t *) &(payload[2]) = utc_year;
        payload[4] = utc_month;
        payload[5] = utc_day;
        payload[6] = utc_hour;
        payload[7] = utc_minute;
        payload[8] = utc_second;
        *(int16_t *) &(payload[9]) = latitude;
        *(int16_t *) &(payload[11]) = longitude;
        *(int16_t *) &(payload[13]) = altitude;

        skytraq_send_message(payload, length);
}

void skytraq_query_software_version(void) {
    uint16_t length = 2;
    uint8_t payload[2];
    payload[0] = QUERY_SOFTWARE_VERSION;
    payload[1] = 1; // system code

    skytraq_send_message(payload, length);
}

void skytraq_query_software_CRC(void) {
    uint16_t length = 2;
    uint8_t payload[2];
    payload[0] = QUERY_SOFTWARE_CRC;
    payload[1] = 1; // system code

    skytraq_send_message(payload, length);
}

void skytraq_restore_factory_defaults(void) {
    uint16_t length = 2;
    uint8_t payload[2];
    payload[0] = SET_FACTORY_DEFAULTS;
    payload[1] = 1; // system code

    skytraq_send_message(payload, length);
}

void skytraq_configure_serial_port(skytraq_baud_rate rate, skytraq_update_attributes attribute) {
    uint16_t length = 4;
    uint8_t payload[4];

    payload[0] = CONFIGURE_SERIAL_PORT;
    payload[1] = 0; // COM port is always 0, only one COM port on skytraq receiver
    payload[2] = rate;
    payload[3] = attribute;

    skytraq_send_message(payload, length);
    
}

void skytraq_configure_nmea_output_rate(uint8_t GGA_interval, uint8_t GSA_interval, uint8_t GSV_interval, uint8_t GLL_interval, uint8_t RMC_interval, uint8_t VTG_interval, uint8_t ZDA_interval, skytraq_update_attributes attribute) {
    uint16_t length = 9;
    uint8_t payload[9];
    payload[0] = CONFIGURE_NMEA;
    payload[1] = GGA_interval;
    payload[2] = GSA_interval;
    payload[3] = GSV_interval;
    payload[4] = GLL_interval;
    payload[5] = RMC_interval;
    payload[6] = VTG_interval;
    payload[7] = ZDA_interval;
    payload[8] = attribute;

    skytraq_send_message(payload, length);
}

void configure_message_type(skytraq_message_type type, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_MESSAGE_TYPE;
    payload[1] = type;
    payload[2] = attribute;

    skytraq_send_message(payload, length);
}

void skytraq_configure_power_mode(skytraq_power_mode mode, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_POWER_MODE;
    payload[1] = mode;
    payload[2] = attribute;

    skytraq_send_message(payload, length);
}

void skytraq_get_gps_time(void) {
    uint16_t length = 2;
    uint8_t payload[2];

    * (uint16_t *) &(payload[0]) = QUERY_GPS_TIME;
    

    skytraq_send_message(payload, length);
}

void skytraq_configure_utc_reference(enable_disable status, uint16_t utc_year, uint8_t utc_month, uint8_t utc_day, skytraq_update_attributes attribute) {
    uint16_t length = 8;
    uint8_t payload[8];

    * (uint16_t *) &(payload[0]) = CONFIGURE_UTC_REFERENCE;
    payload[2] = status;
    *(uint16_t *) &(payload[3]) = utc_year;
    payload[5] = utc_month;
    payload[6] = utc_day;
    payload[7] = attribute;

    skytraq_send_message(payload, length);
}

/*
void configure_system_position_rate(uint8_t rate, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_UPDATE_RATE;
    payload[1] = rate;
    payload[2] = attribute;


    skytraq_send_message(payload, length);
}

void query_position_update_rate() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_UPDATE_RATE;


    skytraq_send_message(payload, length);
}

void query_power_mode() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_POWER_MODE;


    skytraq_send_message(payload, length);
}

void query_datum() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_DATUM;


    skytraq_send_message(payload, length);
}

void query_dop_mask() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_DOP_MASK;


    skytraq_send_message(payload, length);
}

void query_elevation_cnr_mask() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_ELEVATION_CNR_MASK;


    skytraq_send_message(payload, length);
}

void get_gps_ephemeris() {
    uint16_t length = 2;
    uint8_t payload[2];

    payload[0] = GET_GPS_EPHEMERIS;
    payload[1] = 0;


    skytraq_send_message(payload, length);
}

void get_glonass_ephemeris() {
    uint16_t length = 2;
    uint8_t payload[2];

    payload[0] = GET_GLONASS_EPHEMERIS;
    payload[1] = 0;


    skytraq_send_message(payload, length);
}

// may be unnecessary after using skytraq's own software
void configure_position_pinning(position_pinning mode, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_POSITION_PINNING;
    payload[1] = mode;
    payload[2] = attribute;

    skytraq_send_message(payload, length);
}

void query_position_pinning() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_POSITION_PINNING;


    skytraq_send_message(payload, length);
}

void query_1pps_timing() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_1PPS_TIMING;


    skytraq_send_message(payload, length);
}

void query_1pps_cable_delay() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_1PPS_CABLE_DELAY;


    skytraq_send_message(payload, length);
}

void configure_nmea_talker_ID(nmea_talker_IDs id, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_NMEA_TALKER_ID;
    payload[1] = id;
    payload[2] = attribute;


    skytraq_send_message(payload, length);
}

void query_nmea_talker_id() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_NMEA_TALKER_ID;


    skytraq_send_message(payload, length);
}

void get_gps_almanac() {
    uint16_t length = 2;
    uint8_t payload[2];

    payload[0] = GET_GPS_ALMANAC;
    payload[1] = 0;

    skytraq_send_message(payload, length);
}

void get_glonass_almanac() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = GET_GPS_ALMANAC;
    payload[1] = 0;

    skytraq_send_message(payload, length);
}

void query_1pps_timing() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_1PPS_TIMING;


    skytraq_send_message(payload, length);
}

void get_glonass_time_correction() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = GET_GLONASS_TIME_CORRECT_;

    skytraq_send_message(payload, length);
}
*/
