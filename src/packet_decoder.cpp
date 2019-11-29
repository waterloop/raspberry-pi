#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <memory>
#include <task_manager/message.h>
#include "packet_decoder.h"

static std::unordered_map<uint32_t, sensor_id> sensor_map;
static bool sensor_map_populated;

static void build_sensor_map() {
    for(uint8_t type = 0; type < SENSOR_NUM_TYPES; ++type) {
        for(uint8_t inst = 0; inst < SENSOR_NUM_INST[type]; ++inst) {
            sensor_id sensor = {
                .can_id = SENSOR_CAN_ID[type][inst],
                .sensor_type = type,
                .sensor_inst = inst,
            };
            sensor_map[sensor.can_id] = sensor;
        }
    }
    sensor_map_populated = true;
};

static std::shared_ptr<sensor_data> create_sensor_data(uint8_t sensor_type) {
    sensor_data *data;
    switch(sensor_type) {
        case SENSOR_TYPE_IMU1ACCEL:
            data = new sensor_imu1accel_data();
            break;
        case SENSOR_TYPE_IMU1GYRO:
            data = new sensor_imu1gyro_data();
            break;
        case SENSOR_TYPE_TEMP1:
            data = new sensor_temp1_data();
            break;
        case SENSOR_TYPE_TEMP2:
            data = new sensor_temp2_data();
            break;
        case SENSOR_TYPE_PHOTO2:
            data = new sensor_photo2_data();
            break;
        case SENSOR_TYPE_HALL1:
            data = new sensor_hall1_data();
            break;
        case SENSOR_TYPE_PIEZO1:
            data = new sensor_piezo1_data();
            break;
    }
    return std::shared_ptr<sensor_data>(data);
}

std::shared_ptr<sensor_data> decode_data(uint32_t can_id, uint8_t *buffer, uint8_t len) {
    if(!sensor_map_populated)
        build_sensor_map();

    auto it = sensor_map.find(can_id);
    if(it == sensor_map.end()) {
        return nullptr;
    }
    sensor_id &id = it->second;
    std::shared_ptr<sensor_data> data = create_sensor_data(id.sensor_type);

    data->sensor = id;

    memcpy(data->raw_data, buffer, len);
    data->raw_data_len = len;

    data->decode();

    return data;
}