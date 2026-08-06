/*
 * protocol.c
 *
 *  Created on: 03-Jul-2026
 *      Author: hp
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "protocol.h"

#define PROTOCOL_SOF        (0xFF)
#define PROTOCOL_EOF        (0xAA)


#define PROTOCOL_STATE_WAIT_FOR_SOF             (0U)
#define PROTOCOL_STATE_WAIT_FOR_ADDRESS         (1U)
#define PROTOCOL_STATE_WAIT_FOR_COMMAND         (2U)
#define PROTOCOL_STATE_WAIT_FOR_PAYLOAD_LENGTH  (3U)
#define PROTOCOL_STATE_READ_PAYLOAD             (4U)
#define PROTOCOL_STATE_READ_CRC_BYTE_1          (5U)
#define PROTOCOL_STATE_READ_CRC_BYTE_2          (6U)
#define PROTOCOL_STATE_WAIT_FOR_EOF             (7U)

#define PROTOCOL_STATUS_OK                      (0U)
#define PROTOCOL_STATUS_CRC_ERROR               (1U)
#define PROTOCOL_STATUS_UNSUPPORTED_COMMAND     (2U)
#define PROTOCOL_STATUS_PROCESSING_DONE         (3U)


void protocol_init(struct protocol_context* context)
{
    memset( context->payload, 0, sizeof(context->payload) );

    context->index = 0;
    context->status = PROTOCOL_STATUS_OK;
    context->state = PROTOCOL_STATE_WAIT_FOR_SOF;
    context->processing_is_pending = false;
}



static void protocol_busy_notify(struct protocol_context* context)
{
    if( context->send_byte )
    {
        context->send_byte(PROTOCOL_SOF);
        context->send_byte(0xFF);
        context->send_byte(0x00);
        context->send_byte(0x00);
        context->send_byte(0x00);
        context->send_byte(PROTOCOL_EOF);
    }
}

void protocol_handle_rx_byte(struct protocol_context* context, uint8_t rx_byte)
{
    switch(context->state)
    {
        case PROTOCOL_STATE_WAIT_FOR_SOF:
        {
            if(rx_byte == PROTOCOL_SOF && !context->processing_is_pending)
            {
                memset( context->payload, 0, sizeof(context->payload) );

                context->index = 0;
                context->state = PROTOCOL_STATE_WAIT_FOR_ADDRESS;

                return;
            }
            else if(context->processing_is_pending)
            {
                protocol_busy_notify(context);
            }
        }
        break;

        case PROTOCOL_STATE_WAIT_FOR_ADDRESS:
        {
            if( rx_byte == context->address )
            {
                context->state = PROTOCOL_STATE_WAIT_FOR_COMMAND;
            }
            else
            {
                context->state = PROTOCOL_STATE_WAIT_FOR_SOF;
            }
        }
        break;

        case PROTOCOL_STATE_WAIT_FOR_COMMAND:
        {
            context->command = rx_byte;
            context->state = PROTOCOL_STATE_WAIT_FOR_PAYLOAD_LENGTH;
        }
        break;


        case PROTOCOL_STATE_WAIT_FOR_PAYLOAD_LENGTH:
        {
            context->payload_length = rx_byte;
            context->state = PROTOCOL_STATE_READ_PAYLOAD;
        }
        break;


        case PROTOCOL_STATE_READ_PAYLOAD:
        {
            context->payload[context->index] = rx_byte;
            context->index++;

            if(context->index == context->payload_length)
            {
                context->state = PROTOCOL_STATE_READ_CRC_BYTE_1;
            }
        }
        break;

        case PROTOCOL_STATE_READ_CRC_BYTE_1:
        {
            context->crc = 0;
            context->crc |= ( ( (uint16_t)rx_byte ) << 8 );
            context->state = PROTOCOL_STATE_READ_CRC_BYTE_2;
        }
        break;


        case PROTOCOL_STATE_READ_CRC_BYTE_2:
        {
            context->crc |= ( ( (uint16_t)rx_byte ) );
            context->state = PROTOCOL_STATE_WAIT_FOR_EOF;
        }
        break;


        case PROTOCOL_STATE_WAIT_FOR_EOF:
        {
            if( rx_byte == PROTOCOL_EOF )
            {
                context->state = PROTOCOL_STATE_WAIT_FOR_SOF;
                context->processing_is_pending = true;

                if(context->event_callback)
                {
                    context->event_callback();
                }

                return;
            }
        }
        break;
    }
}

/* CRC-16/CCITT-FALSE lookup table (Polynomial 0x1021) */
static const uint16_t crc16_ccitt_table[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

uint16_t crc16_ccitt_update(uint8_t byte, uint16_t crc)
{
    uint8_t index = (uint8_t)((crc >> 8) ^ byte);
    return (uint16_t)((crc << 8) ^ crc16_ccitt_table[index]);
}



static bool protocol_check_crc16(struct protocol_context* context)
{
    int index;

    uint16_t crc = 0xFFFF;

    crc = crc16_ccitt_update(context->command, crc);
    crc = crc16_ccitt_update(context->payload_length, crc);

    for( index = 0;  index < (int)context->payload_length ; index++ )
    {
        crc = crc16_ccitt_update(context->payload[index], crc);
    }

    crc = crc16_ccitt_update( (uint8_t)( (context->crc >> 8) & 0xFF ), crc);
    crc = crc16_ccitt_update( (uint8_t)( (context->crc >> 0) & 0xFF ), crc);

    return (crc == 0);
}



static void protocol_calculate_crc16(struct protocol_context* context)
{
    int index;
    uint16_t crc = 0xFFFF;

    crc = crc16_ccitt_update(context->command, crc);
    crc = crc16_ccitt_update(context->payload_length, crc);

    for( index = 0;  index < (int)context->payload_length ; index++ )
    {
        crc = crc16_ccitt_update(context->payload[index], crc);
    }

    context->crc = crc;
}


void protocol_set_response_payload_length(struct protocol_context* context, size_t length)
{
    context->payload_length = (uint8_t)length;
}


void protocol_write_payload_byte(struct protocol_context* context, uint8_t byte, int index)
{
    context->payload[index] = byte;
}

int protocol_process(struct protocol_context* context)
{
    bool command_found = false;
    const protocol_app_callback_list_element_t* callback_element = context->app_callback_list;

    context->status = PROTOCOL_STATUS_OK;

    if(context->processing_is_pending == false)
    {
        return context->status;
    }

    if( !protocol_check_crc16(context) )
    {
        context->status = PROTOCOL_STATUS_CRC_ERROR;
        context->processing_is_pending = false;
    }
    else
    {
        if( !context->app_callback_list )
        {
            context->status = PROTOCOL_STATUS_UNSUPPORTED_COMMAND;
            context->processing_is_pending = false;
        }
        else
        {
            while( callback_element->callback )
            {
                if(callback_element->command == context->command)
                {
                    command_found = true;
                    break;
                }

                callback_element++;
            }

            if(!command_found)
            {
                context->status = PROTOCOL_STATUS_UNSUPPORTED_COMMAND;
                context->processing_is_pending = false;
            }
            else
            {
                callback_element->callback(context);
                protocol_calculate_crc16(context);

                if(context->send && context->send_byte)
                {
                    context->send_byte(PROTOCOL_SOF);
                    context->send_byte(context->address);
                    context->send(&context->command, 1);
                    context->send(&context->payload_length, 1);
                    context->send(context->payload, context->payload_length);
                    context->send_byte((context->crc >> 8) & 0xFF);
                    context->send_byte((context->crc >> 0) & 0xFF);
                    context->send_byte(PROTOCOL_EOF);
                }

                context->processing_is_pending = false;
            }
        }
    }

    return context->status;
}

