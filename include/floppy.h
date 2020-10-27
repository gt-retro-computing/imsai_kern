#pragma once

#include "imsai.h"

typedef uint8_t FDC_ERR;

#define FDC_OK              0
#define FDC_ERR_OTHER       1
#define FDC_ERR_LOST_DATA   2
#define FDC_ERR_NOT_FOUND   3

// 0 = first drive, etc...
FDC_ERR fdc_select(uint8_t index);
FDC_ERR fdc_select_dd(uint8_t index);

FDC_ERR fdc_home();

// first track = 0
FDC_ERR fdc_seek(uint8_t track);

// first sector = 1
FDC_ERR fdc_read(uint8_t sector, uint8_t *buf, size_t size);
FDC_ERR fdc_write(uint8_t sector, uint8_t *buf, size_t size);

FDC_ERR fdc_write_track(uint8_t *buffer, size_t size);
