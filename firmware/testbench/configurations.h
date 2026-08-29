
#ifndef __CONFIGURATIONS_H__
#define __CONFIGURATIONS_H__

#define CONFIGURATION_STRUCT_SIZE       (8192U)
#define CONFIGURATION_ENTRIES           (16U)
#define CONFIGURATION_START_ADDRESS     (0x60000U)

typedef union configuration_union
{
    uint8_t data[CONFIGURATION_STRUCT_SIZE];

    struct entry_struct {
        uint32_t number;
        uint32_t crc32;

        /* Configuration entries start from here. */
               

    }entry;
}configuration_t;

extern configuration_t user_configuration;

#define configuration_write_parameter(_name, _value)            \
    user_configuration._name = _value

#define configuration_read_parameter(_name)                     \
    user_configuration._name

extern void configuration_init();
extern void configuration_save();

#endif /* __CONFIGURATIONS_H__ */