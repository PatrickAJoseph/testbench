/*
 * protocol.h
 *
 *  Created on: 03-Jul-2026
 *      Author: hp
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PROTOCOL_MAX_PAYLOAD_SIZE       (250U)

#define PROTOCOL_BUFFER_SIZE        (1U /* Start of frame */  \
                                    + 1U /* Command */ \
                                    + 1U /* Payload length */   \
                                    + PROTOCOL_MAX_PAYLOAD_SIZE /* Maximum Payload size */   \
                                    + 2U /* CRC16-CCITT */  \
                                    + 1U /* End of frame */)s

/* Helper macros */

#define PROTOCOL_WRITE_UINT8(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint8_t)data >> 0) & 255 ), (start_byte + 0) );      \
    }while(0);

#define PROTOCOL_WRITE_UINT16(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint16_t)data >> 8) & 255 ), (start_byte + 0) );      \
        protocol_write_payload_byte(context, ( ((uint16_t)data >> 0) & 255 ), (start_byte + 1) );      \
    }while(0);

#define PROTOCOL_WRITE_UINT32(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 24) & 255 ), (start_byte + 0) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 16) & 255 ), (start_byte + 1) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 8) & 255 ), (start_byte + 2) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 0) & 255 ), (start_byte + 3) );      \
    }while(0);

#define PROTOCOL_READ_UINT32(context, pdata, start_byte)                                \
    do{                                                                                 \
            *pdata = 0;                                                                 \
            (*pdata) |= ( (uint32_t)context->payload[start_byte + 0] << 24 );           \
            (*pdata) |= ( (uint32_t)context->payload[start_byte + 1] << 16 );           \
            (*pdata) |= ( (uint32_t)context->payload[start_byte + 2] << 8 );           \
            (*pdata) |= ( (uint32_t)context->payload[start_byte + 3] );           \
    }while(0);

#define PROTOCOL_READ_UINT16(context, pdata, start_byte)                                \
    do{                                                                                 \
            *pdata = 0;                                                                 \
            (*pdata) |= ( (uint16_t)context->payload[start_byte + 0] << 8 );           \
            (*pdata) |= ( (uint16_t)context->payload[start_byte + 1] );           \
    }while(0);


#define PROTOCOL_READ_FLOAT(context, pdata, start_byte)                                \
        PROTOCOL_READ_UINT32(context, pdata, start_byte)

#define PROTOCOL_WRITE_UINT64(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 56) & 255 ), (start_byte + 0) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 48) & 255 ), (start_byte + 1) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 40) & 255 ), (start_byte + 2) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 32) & 255 ), (start_byte + 3) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 24) & 255 ), (start_byte + 4) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 16) & 255 ), (start_byte + 5) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 8) & 255 ), (start_byte + 6) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 0) & 255 ), (start_byte + 7) );      \
    }while(0);

#define PROTOCOL_WRITE_INT8(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint8_t)data >> 0) & 255 ), (start_byte + 0) );      \
    }while(0);

#define PROTOCOL_WRITE_INT16(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint16_t)data >> 8) & 255 ), (start_byte + 0) );      \
        protocol_write_payload_byte(context, ( ((uint16_t)data >> 0) & 255 ), (start_byte + 1) );      \
    }while(0);

#define PROTOCOL_WRITE_INT32(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 24) & 255 ), (start_byte + 0) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 16) & 255 ), (start_byte + 1) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 8) & 255 ), (start_byte + 2) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)data >> 0) & 255 ), (start_byte + 3) );      \
    }while(0);

#define PROTOCOL_WRITE_INT64(context, data, start_byte)                                                \
    do{                                                                                                 \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 56) & 255 ), (start_byte + 0) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 48) & 255 ), (start_byte + 1) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 40) & 255 ), (start_byte + 2) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 32) & 255 ), (start_byte + 3) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 24) & 255 ), (start_byte + 4) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 16) & 255 ), (start_byte + 5) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 8) & 255 ), (start_byte + 6) );      \
        protocol_write_payload_byte(context, ( ((uint64_t)data >> 0) & 255 ), (start_byte + 7) );      \
    }while(0);

#define PROTOCOL_WRITE_FLOAT(context, data, start_byte)                                                \
    do{                                                                                                 \
        uint32_t* _data = (uint32_t*)&data;                                                             \
        protocol_write_payload_byte(context, ( ((uint32_t)*_data >> 24) & 255 ), (start_byte + 0) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)*_data >> 16) & 255 ), (start_byte + 1) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)*_data >> 8) & 255 ), (start_byte + 2) );      \
        protocol_write_payload_byte(context, ( ((uint32_t)*_data >> 0) & 255 ), (start_byte + 3) );      \
    }while(0);


struct protocol_context;

typedef void (*protocol_app_callback_t)(struct protocol_context*);

typedef void (*protocol_send_t)(uint8_t* data, size_t length);

typedef void (*protocol_send_byte_t)(uint8_t byte);

typedef struct app_callback_list_element
{
    uint8_t command;
    protocol_app_callback_t callback;
} protocol_app_callback_list_element_t;


typedef void (*protocol_event_callback_t)(void);


struct protocol_context
{
    uint8_t command;
    uint8_t address;
    uint8_t payload_length;
    uint8_t payload[PROTOCOL_MAX_PAYLOAD_SIZE];
    uint16_t crc;
    int index;
    int status;
    int state;
    bool processing_is_pending;
    protocol_event_callback_t event_callback;
    const protocol_app_callback_list_element_t* app_callback_list;
    protocol_send_t send;
    protocol_send_byte_t send_byte;
};

extern void protocol_init(struct protocol_context* context);
extern void protocol_handle_rx_byte(struct protocol_context* context, uint8_t rx_byte);
extern void protocol_set_response_payload_length(struct protocol_context* context, size_t length);
extern void protocol_write_payload_byte(struct protocol_context* context, uint8_t byte, int index);
extern int protocol_process(struct protocol_context* context);


#endif /* PROTOCOL_H_ */
