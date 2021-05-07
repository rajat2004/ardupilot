/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
	Simulator connector for Airsim: https://github.com/Microsoft/AirSim
*/

#pragma once

#include <AP_HAL/utility/Socket.h>
#include "SIM_Aircraft.h"

namespace SITL {

/*
	Airsim Simulator
*/

class AirSim : public Aircraft {
public:
	AirSim(const char *frame_str);

	/* update model by one time step */
	void update(const struct sitl_input &input) override;

	/* static object creator */
    static Aircraft *create(const char *frame_str) {
        return new AirSim(frame_str);
    }

    /*  Create and set in/out socket for Airsim simulator */
    void set_interface_ports(const char* address, const int port_in, const int port_out) override;

private:
    enum class OutputType {
        Copter = 1,
        Rover = 2
    } output_type;

    // Control packet for Rover
    struct rover_packet {
        float throttle;     // -1 to 1
        float steering;     // -1 to 1
    };

    // rotor control packet sent by Ardupilot
    static const int kArduCopterRotorControlCount = 11;

    struct servo_packet {
		uint16_t pwm[kArduCopterRotorControlCount];
	};

	// default connection_info_.ip_address
	const char *airsim_ip = "127.0.0.1";

	// connection_info_.ip_port
	uint16_t airsim_sensor_port = 9003;

	// connection_info_.sitl_ip_port
	uint16_t airsim_control_port = 9002;

	SocketAPM sock;

    double average_frame_time;
    uint64_t frame_counter;
    uint64_t last_frame_count;
    uint64_t last_timestamp;

    void output_copter(const sitl_input& input);
    void output_rover(const sitl_input& input);
    // Wrapper function over the above 2 output methods
    void output_servos(const sitl_input& input);

    void recv_fdm(const sitl_input& input);
    void report_FPS(void);

	uint16_t parse_sensors(const char *json);

	// buffer for parsing pose data in JSON format
    uint8_t sensor_buffer[65000];
    uint32_t sensor_buffer_len;

	enum data_type {
		DATA_UINT64,
        DATA_FLOAT,
        DATA_DOUBLE,
        DATA_VECTOR3F,
        DATA_VECTOR3F_ARRAY,
        DATA_FLOAT_ARRAY,
    };

    struct {
        uint64_t timestamp;
        struct {
            Vector3f angular_velocity;
            Vector3f linear_acceleration;
        } imu;
        struct {
            double lat, lon, alt;
        } gps;
        struct {
            float roll, pitch, yaw;
        } pose;
        struct {
            Vector3f world_linear_velocity;
        } velocity;
        struct {
            struct vector3f_array points;
        } lidar;
        struct {
            struct float_array rc_channels;
        } rc;
        struct {
            struct float_array rng_distances;
        } rng;
        Vector3f position;
    } state;

    // table to aid parsing of JSON sensor data
    struct keytable {
        const char *section;
        const char *key;
        void *ptr;
        enum data_type type;
        bool required;
    } keytable[14] = {
        { "", "timestamp", &state.timestamp, DATA_UINT64, true },
        { "imu", "angular_velocity",    &state.imu.angular_velocity, DATA_VECTOR3F, true },
        { "imu", "linear_acceleration", &state.imu.linear_acceleration, DATA_VECTOR3F, true },
        { "gps", "lat", &state.gps.lat, DATA_DOUBLE, false },
        { "gps", "lon", &state.gps.lon, DATA_DOUBLE, false },
        { "gps", "alt", &state.gps.alt, DATA_DOUBLE, false },
        { "pose", "roll",  &state.pose.roll, DATA_FLOAT, true },
        { "pose", "pitch", &state.pose.pitch, DATA_FLOAT, true },
        { "pose", "yaw",   &state.pose.yaw, DATA_FLOAT, true },
        { "velocity", "world_linear_velocity", &state.velocity.world_linear_velocity, DATA_VECTOR3F, true },
        { "lidar", "point_cloud", &state.lidar.points, DATA_VECTOR3F_ARRAY, false },
        { "rc", "channels", &state.rc.rc_channels, DATA_FLOAT_ARRAY, false },
        { "rng", "distances", &state.rng.rng_distances, DATA_FLOAT_ARRAY, false },
        { "", "position", &state.position, DATA_VECTOR3F, false },
    };

    enum DataKey {
        TIMESTAMP   = 1U << 0,
        GYRO        = 1U << 1,
        ACCEL_BODY  = 1U << 2,
        GPS_LAT     = 1U << 3,
        GPS_LON     = 1U << 4,
        GPS_ALT     = 1U << 5,
        ATT_ROLL    = 1U << 6,
        ATT_PITCH   = 1U << 7,
        ATT_YAW     = 1U << 8,
        VELOCITY    = 1U << 9,
        LIDAR       = 1U << 10,
        RC_CHANNELS = 1U << 11,
        RANGEFINDER = 1U << 12,
        POSITION    = 1U << 13,
    };
    uint16_t last_received_bitmask;
};

}
