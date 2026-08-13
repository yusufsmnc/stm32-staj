/*
 * load_types.h
 *
 *  Created on: Aug 13, 2026
 *      Author: Yusuf
 */

#ifndef INC_LOAD_TYPES_H_
#define INC_LOAD_TYPES_H_

#include "stdint.h"

typedef enum {
    LOAD_RESISTIVE    = 0,
    LOAD_INDUCTIVE_30,
    LOAD_INDUCTIVE_60,
    LOAD_CAPACITIVE,
    LOAD_COUNT
} LoadType_t;

extern const float LOAD_FAZ_TABLE[LOAD_COUNT];
extern const char* LOAD_NAME_TABLE[LOAD_COUNT];


#endif /* INC_LOAD_TYPES_H_ */
