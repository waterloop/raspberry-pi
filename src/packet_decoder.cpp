#include <stdint.h>

const uint32_t IMU_1_ACCEL_ID[] = {1024, 1026};
const uint32_t IMU_1_GYRO_ID[] = {1025, 1027};
const uint32_t TEMP_1_ID[] = {1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037};
const uint32_t TEMP_2_ID[] = {1038, 1039};
const uint32_t PHOTO_2_ID[] = {1040, 1041};
const uint32_t HALL_1_ID[] = {1042, 1043, 1044, 1045, 1046, 1047};
const uint32_t PIEZO_1_ID[] = {1048, 1049};

const uint32_t *SENSORS[] = {IMU_1_ACCEL_ID, IMU_1_GYRO_ID, TEMP_1_ID, TEMP_2_ID, PHOTO_2_ID, HALL_1_ID, PIEZO_1_ID};

// map between can_id -> sensor, instance

struct sensor {
    
    
};

struct sensor_data {
    uint32_t can_id;
    
    uint8_t sensor_type;
    uint8_t sensor_inst;

    uint8_t raw_data[8];
    uint8_t raw_data_len;

    virtual void decode();
};

struct sensor_mcp6050_accel_decoder : public sensor_data {
    double x, y, z;

    void decode() {
        // read raw_data
        // write into x, y, z
        x = (raw_data[0] << 8) | raw_data[1];
        y = (raw_data[2] << 8) | raw_data[3];
        z = (raw_data[4] << 8) | raw_data[5];
    }
};

struct sensor_mcp6050_gyro_decoder : public sensor_data {
    double x, y, z;

    void decode() {
        x = (raw_data[0] << 8) | raw_data[1];
        y = (raw_data[2] << 8) | raw_data[3];
        z = (raw_data[4] << 8) | raw_data[5];
    }
};

struct sensor_tmp411_decoder : public sensor_data {
    double t;

    void decode() {
        t = (raw_data[0] << 8) | raw_data[1];
    }
};

struct sensor_temp2_decoder : public sensor_data {
    double t;

    void decode() {
        t = (raw_data[0] << 8) | raw_data[1];
    }
};

struct sensor_photo2_decoder : public sensor_data {
    double p;

    void decode() {
        p = (raw_data[0] << 24) | (raw_data[1] << 16) | (raw_data[2] << 8) | raw_data[3];
    }
};

struct sensor_hall1_decoder : public sensor_data {
    double s;

    void decode() {
        s = (raw_data[0] << 8) | raw_data[1];
    }
};

struct sensor_piezo1_decoder : public sensor_data {
    double v;

    void decode() {
        v = (raw_data[0] << 8) | raw_data[1];
    }
}