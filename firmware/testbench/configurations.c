
#include "ti_sdk_dl_config.h"
#include "configurations.h"

#include <string.h>

uint32_t user_configuration_nvm_entry_index;
uint32_t user_configuration_nvm_entry_number;

configuration_t user_configuration;
configuration_t* user_configurations = (configuration_t*)CONFIGURATION_START_ADDRESS;

void configuration_init()
{
    uint32_t index;
    uint32_t entry_number;
    uint32_t entry_numbers[CONFIGURATION_ENTRIES] = {0};
    uint32_t max_entry_number = 0;
    bool valid_configuration_found = false;

    (void)index;
    (void)entry_number;
    (void)entry_numbers;
    (void)max_entry_number;

    for( index = 0 ; index < CONFIGURATION_ENTRIES ; index++ )
    {
        (void)index;

        entry_number = ((configuration_t*)(CONFIGURATION_START_ADDRESS + (index * CONFIGURATION_STRUCT_SIZE)))->entry.number;

        if(entry_number != 0xFFFFFFFFU)
        {
            valid_configuration_found = true;
            entry_numbers[index] = entry_number;
        }
    }

    user_configuration_nvm_entry_index = 0U;

    for( index = 0 ; index < CONFIGURATION_ENTRIES ; index++ )
    {
        if( entry_numbers[index] > max_entry_number )
        {
            user_configuration_nvm_entry_index = index;
            max_entry_number = entry_numbers[index];
        }
    }

    user_configuration = *((configuration_t*)(CONFIGURATION_START_ADDRESS + (user_configuration_nvm_entry_index * CONFIGURATION_STRUCT_SIZE)));

    memcpy(&user_configuration, (void*)(CONFIGURATION_START_ADDRESS + (user_configuration_nvm_entry_index * CONFIGURATION_STRUCT_SIZE)), sizeof(configuration_t));

    user_configuration_nvm_entry_number = user_configuration.entry.number;
    user_configuration_nvm_entry_number++;

    if(valid_configuration_found)
    {
        user_configuration_nvm_entry_index++;
    }

    user_configuration_nvm_entry_index %= CONFIGURATION_ENTRIES;
}

void configuration_save()
{
    uint32_t status;

    user_configuration.entry.number = user_configuration_nvm_entry_number;

    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_eraseSector(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index));
    
    if(status)
    {
        __asm volatile("bkpt 0");
    }

    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_eraseSector(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index) + DL_FLASHCTL_SECTOR_SIZE);   
    
    if(status)
    {
        __asm volatile("bkpt 0");
    }
    
    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_eraseSector(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index) + (2U*DL_FLASHCTL_SECTOR_SIZE));

    if(status)
    {
        __asm volatile("bkpt 0");
    }

    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_eraseSector(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index) + (3U*DL_FLASHCTL_SECTOR_SIZE));

    if(status)
    {
        __asm volatile("bkpt 0");
    }

    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_program(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index), 
                        user_configuration.data, DL_FLASHCTL_SECTOR_SIZE);

    if(status)
    {
        __asm volatile("bkpt 0");
    }

    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_program(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index) + DL_FLASHCTL_SECTOR_SIZE, 
                        &user_configuration.data[DL_FLASHCTL_SECTOR_SIZE], DL_FLASHCTL_SECTOR_SIZE);

    if(status)
    {
        __asm volatile("bkpt 0");
    }

    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_program(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index) + (2U*DL_FLASHCTL_SECTOR_SIZE), 
                        &user_configuration.data[2U*DL_FLASHCTL_SECTOR_SIZE], DL_FLASHCTL_SECTOR_SIZE);
    
    if(status)
    {
        __asm volatile("bkpt 0");
    }

    __DMB();
    __DSB();
    __ISB();

    status = DL_Flash_program(CONFIGURATION_START_ADDRESS + (CONFIGURATION_STRUCT_SIZE * user_configuration_nvm_entry_index) + (3U*DL_FLASHCTL_SECTOR_SIZE), 
                        &user_configuration.data[3U*DL_FLASHCTL_SECTOR_SIZE], DL_FLASHCTL_SECTOR_SIZE);
    
    if(status)
    {
        __asm volatile("bkpt 0");
    }

    __DMB();
    __DSB();
    __ISB();


    user_configuration_nvm_entry_index++;
    user_configuration_nvm_entry_number++;
    user_configuration_nvm_entry_index %= CONFIGURATION_ENTRIES;
}