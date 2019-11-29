#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H

#include <memory>
#include <stdint.h>
#include <registry.h>

static const uint32_t IMU_1_ACCEL_CAN_ID[] = {1024, 1026};
static const uint32_t IMU_1_GYRO_CAN_ID[] = {1025, 1027};
static const uint32_t TEMP_1_CAN_ID[] = {1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037};
static const uint32_t TEMP_2_CAN_ID[] = {1038, 1039};
static const uint32_t PHOTO_2_CAN_ID[] = {1040, 1041};
static const uint32_t HALL_1_CAN_ID[] = {1042, 1043, 1044, 1045, 1046, 1047};
static const uint32_t PIEZO_1_CAN_ID[] = {1048, 1049};

static const uint32_t *SENSOR_CAN_ID[] = {IMU_1_ACCEL_CAN_ID, IMU_1_GYRO_CAN_ID, TEMP_1_CAN_ID, TEMP_2_CAN_ID, PHOTO_2_CAN_ID, HALL_1_CAN_ID, PIEZO_1_CAN_ID};
static const uint8_t SENSOR_NUM_INST[] = {2, 2, 10, 2, 2, 6, 2};
static const uint8_t SENSOR_NUM_TYPES = 7;

struct sensor_id {
    uint32_t can_id;
    uint8_t sensor_type;
    uint8_t sensor_inst;
};

struct sensor_data {
    sensor_id sensor;

    uint8_t raw_data[8];
    uint8_t raw_data_len;

    virtual void decode() = 0;
};

struct sensor_imu1accel_data : public sensor_data {
    double x, y, z;

    void decode() {
        x = (raw_data[0] << 8) | raw_data[1];
        y = (raw_data[2] << 8) | raw_data[3];
        z = (raw_data[4] << 8) | raw_data[5];
    }
};

struct sensor_imu1gyro_data : public sensor_data {
    double x, y, z;

    void decode() {
        x = (raw_data[0] << 8) | raw_data[1];
        y = (raw_data[2] << 8) | raw_data[3];
        z = (raw_data[4] << 8) | raw_data[5];
    }
};

struct sensor_temp1_data : public sensor_data {
    double temp;

    void decode() {
        temp = (raw_data[0] << 8) | raw_data[1];
    }
};

struct sensor_temp2_data : public sensor_data {
    double temp;

    void decode() {
        temp = (raw_data[0] << 8) | raw_data[1];
    }
};

struct sensor_photo2_data : public sensor_data {
    double count;

    void decode() {
        count = (raw_data[0] << 24) | (raw_data[1] << 16) | (raw_data[2] << 8) | raw_data[3];
    }
};

struct sensor_hall1_data : public sensor_data {
    double strength;

    void decode() {
        strength = (raw_data[0] << 8) | raw_data[1];
    }
};

struct sensor_piezo1_data : public sensor_data {
    double v;

    void decode() {
        v = (raw_data[0] << 8) | raw_data[1];
    }
};

std::shared_ptr<sensor_data> decode_data(uint32_t can_id, uint8_t *buffer, uint8_t len);

#endif