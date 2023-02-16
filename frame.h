#define PREAMB 170


typedef struct
{
    uint8_t preamb;
    uint8_t sensor;
    uint8_t axis;
    uint8_t checksum;
}reqFrame;

typedef struct{
    uint8_t preamb;
    uint8_t sensor;
    uint8_t size;
    uint8_t data[9];
    uint8_t checksum;
}resFrame;
