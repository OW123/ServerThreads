#define PREAMB 170


typedef struct
{
    int preamb = PREAMB;
    int  sensor;
    int data;
    int checksum;
}sFrame;
