#ifndef SKYTRAQ_BINARY_H
#define SKYTRAQ_BINARY_H

#include <stdint.h>
#include <stdbool.h>
#include "skytraq_binary_types.h"
;

typedef enum ErrorCode {
    SUCCESS,
    RESOURCE_BUSY,
    MESSAGE_INVALID,
    UNKNOWN_ERROR,
    INVALID_CHECKSUM,
    INVALID_CHECKSUM_RECEIVE
} ErrorCode;

bool skytraq_verify_checksum(uint8_t *message);

uint8_t calc_checksum(uint8_t *message, uint16_t payload_length);
// as per my discussion with Josh, minimal functionality will be implemented here
// And no justification will be added for functionality not implemented. If it is detemrmined
// something is needed, it will be done at that time.

// sending a message to a receiver will cause the caller to block
// until a reply is received. 


// utc time does not have to be perfectly accurate
// skytraq pretty much just wants it in the same relative leap second
ErrorCode skytraq_restart_receiver(StartMode start_mode, uint16_t utc_year, uint8_t utc_month, uint8_t utc_day, uint8_t utc_hour, uint8_t utc_minute, uint8_t utc_second, int16_t latitude, int16_t longitude, int16_t altitude);

ErrorCode skytraq_query_software_version(void);

ErrorCode skytraq_query_software_CRC(uint8_t *reply);

ErrorCode skytraq_restore_factory_defaults(void);

ErrorCode skytraq_configure_serial_port(skytraq_baud_rate rate, skytraq_update_attributes attribute);

ErrorCode skytraq_configure_nmea_output_rate(uint8_t GGA_interval, uint8_t GSA_interval, uint8_t GSV_interval, uint8_t GLL_interval, uint8_t RMC_interval, uint8_t VTG_interval, uint8_t ZDA_interval, skytraq_update_attributes attribute);

ErrorCode skytraq_configure_power_mode(skytraq_power_mode mode, skytraq_update_attributes attribute);

ErrorCode skytraq_get_gps_time(uint8_t *reply);

#endif // SKYTRAQ_BINARY_H