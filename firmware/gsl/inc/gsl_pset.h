#pragma once

// This file has functions which allow for persistent settings configurations
// across power cycles.
//
// Some definitions:
// * pset sector: This is the sector of flash memory used to store settings.
//                By default, it is the last sector in flash.
// * settings block: This is a single settings configuration that can be stored
//                   or recalled as necessary.
//
// To achieve this, settings are stored in the last sector of flash memory.
// This block is erased and rewritten as needed.  To avoid erasing the flash
// when not necessary, settings blocks are written consecutively.  When
// recalling the settings, the last settings block is recalled.  The pset
// sector is erased only when a new settings block needs to be written but
// cannot fit within the pset sector.
//
// The layout of the pset sector is as follows:
// * sync
// * settings block
// * sync
// * settings block
// * sync
// * etc...
// * last settings block
// * last sync
// * erased bytes (0xFF)
//
// A sync must follow a settings block in order for it to be valid.  All bytes
// following the final sync value must be erased.
//
// Within each settings block, the layout is as follows:
// * uint8_t identifier
// * uint16_t data_size
// * uint8 data[]
//
// The identifier is used to determine which settings block is stored or
// recalled.  This is zero by default and only needs to be used in the program
// is storing multiple different settings blocks.
//
// If multiple identifiers are used, they need to be remain stored after the
// pset sector is erased.
//
// All data within the sector are byte-aligned.  So uint16_t values will not
// necessarily fall within 2-byte boundaries.
//
// The identifier is used so that we can store multiple independent settings
// blocks and recall them as needed.
//
// The typical usage is as follows:
//   GSL_PSET_Recall(&data, sizeof(data));
//   GSL_PSET_Store(&data, sizeof(data));

#include "gsl_includes.h"

// possible error codes
enum GSL_PSET_ErrorEnum : uint8_t {
  // no error
  kGslPsetErrorNone = 0,
  // the pset sector is invalid
  kGslPsetErrorInvalidSector,
  // a sync was not found after a settings block
  kGslPsetErrorIncompleteBlock,
  // non-erased bytes were found after the last valid settings block
  kGslPsetErrorUnexpectedWrittenBytes,
};

// signature
//const char * const gsl_pset_signature = "GSL_PSET";

// start of settings
const uint8_t gsl_pset_sync[] = {0xFA, 0xF3, 0x20};

const uint16_t gsl_pset_sync_length =
    sizeof(gsl_pset_sync) / sizeof(*gsl_pset_sync);

uint32_t GSL_PSET_GetSettingsMemoryLength(void);
uint8_t * GSL_PSET_GetSettingsMemoryStart(void);

// pointer to the start of the settings block
uint8_t * const gsl_pset_mem_start = GSL_PSET_GetSettingsMemoryStart();

// pointer to the start of the settings block
uint8_t * const gsl_pset_mem_end =
    GSL_PSET_GetSettingsMemoryStart() + GSL_PSET_GetSettingsMemoryLength();

// return an error
void GSL_PSET_Error(GSL_PSET_ErrorEnum error) {
  ASSERT_NE(error, kGslPsetErrorNone);
  switch (error) {
    case kGslPsetErrorInvalidSector:
      HALT("Sector is invalid--possibly occupied by the program.");
      break;
    case kGslPsetErrorIncompleteBlock:
      HALT("Sector is incomplete.");
      break;
    case kGslPsetErrorUnexpectedWrittenBytes:
      HALT("Unexpected written bytes were encountered.");
      break;
    default:
      HALT("Unexpected value");
  }
}

// return the length of the settings block in bytes
uint32_t GSL_PSET_GetSettingsMemoryLength(void) {
  return 128 * 1024;
}

// return a pointer to the start of the settings block
uint8_t * GSL_PSET_GetSettingsMemoryStart(void) {
  // if 2 MByte flash -> 0x081E0000 and sector 23
  // if 1 MByte and DB1M=0 -> 0x080E0000 and sector 11
  // if 1 MByte and DB1M=1 -> 0x080E0000 and sector 19
  // if 512kB and stm32f411 -> 0x08060000 and sector 7
  return (uint8_t *) 0x08060000;
  if (GSL_GEN_GetFlashSize() == 2048) {
    return (uint8_t *) 0x081E0000;
  } else if (GSL_GEN_GetFlashSize() == 1024) {
    return (uint8_t *) 0x080E0000;
  } else if (GSL_GEN_GetFlashSize() == 512) {
    return (uint8_t *) 0x08060000;
  } else {
    HALT("Unexpected value");
  }
}

// return true if the pset sector is initialized
bool GSL_PSET_IsSectorInitialized(void) {
  return memcmp(gsl_pset_mem_start,
                gsl_pset_sync,
                gsl_pset_sync_length) == 0;
}

// return true if the pset sector is erased
bool GSL_PSET_IsSectorErased(void) {
  for (auto address = gsl_pset_mem_start;
      address < gsl_pset_mem_end;
      ++address) {
    if (*address != 0xFF) {
      return false;
    }
  }
  return true;
}

// return true if the pset sector is valid
// the sector is valid if it is erased or if it contains the sync at the start
bool GSL_PSET_IsSectorValid(void) {
  return GSL_PSET_IsSectorInitialized() || GSL_PSET_IsSectorErased();
}

// erase the settings block
void GSL_PSET_EraseSector(void) {
  // before erasing, ensure we don't overwrite a program by ensuring the sector
  // is a valid pset sector
  ASSERT(GSL_PSET_IsSectorValid());

  HAL_FLASH_Unlock();
  uint32_t sector_error = 0;
  FLASH_EraseInitTypeDef flash_erase;
  flash_erase.TypeErase = FLASH_TYPEERASE_SECTORS;
  flash_erase.Banks = 0;
  flash_erase.Sector = FLASH_SECTOR_TOTAL - 1;
  flash_erase.NbSectors = 1;
  // since we don't care about speed, set it to the lowest voltage range
  flash_erase.VoltageRange = FLASH_VOLTAGE_RANGE_1;
  HAL_RUN(HAL_FLASHEx_Erase(&flash_erase, &sector_error));
  HAL_FLASH_Lock();
}

// program the given data into flash at the given memory address
void GSL_PSET_WriteToFlash(const uint8_t * const address,
                           const void * data,
                           uint16_t length) {
  // program the data one byte at a time
  HAL_RUN(HAL_FLASH_Unlock());
  for (uint16_t i = 0; i < length; ++i) {
    HAL_RUN(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
                              (uint32_t) (address + i),
                              ((uint8_t *) data)[i]));
  }
  HAL_RUN(HAL_FLASH_Lock());
}

// write the given structure to flash
template <class T>
void GSL_PSET_WriteToFlash(const uint8_t * const address, const T & object) {
  GSL_PSET_WriteToFlash(address, &object, sizeof(object));
}

// read from flash into the given
// program the given data into flash at the given memory address
void GSL_PSET_ReadFromFlash(const uint8_t * const address,
                            void * data,
                            uint16_t length) {
  memcpy(data, address, length);
}

// read from flash into the given structure
template <class T>
void GSL_PSET_ReadFromFlash(const uint8_t * const address, T & object) {
  memcpy(&object, address, sizeof(object));
}

// initialize the settings block if necessary
void GSL_PSET_Initialize(void) {
  // if it's already initialized, do nothing
  if (GSL_PSET_IsSectorInitialized()) {
    return;
  }
  // make sure it's erased
  if (!GSL_PSET_IsSectorErased()) {
    GSL_PSET_EraseSector();
  }
  ASSERT(GSL_PSET_IsSectorErased());
  // write first sync
  GSL_PSET_WriteToFlash(gsl_pset_mem_start,
                        gsl_pset_sync,
                        gsl_pset_sync_length);
  ASSERT(GSL_PSET_IsSectorInitialized());
}

// return true if there's a valid settings block at the given address
bool GSL_PSET_IsBlockValid(uint8_t * address) {
  if (address == nullptr) {
    return false;
  }
  // ensure there's enough space assuming zero data length
  if (address + 1 + 2 + gsl_pset_sync_length > gsl_pset_mem_end) {
    return false;
  }
  // skip past identifier
  address += 1;
  // get block length
  uint16_t data_length;
  GSL_PSET_ReadFromFlash(address, data_length);
  address += sizeof(data_length);
  address += data_length;
  // ensure there's enough space now that we know the data length
  if (address + gsl_pset_sync_length > gsl_pset_mem_end) {
    return false;
  }
  // check for sync after the block
  if (memcmp(address, gsl_pset_sync, gsl_pset_sync_length) != 0) {
    return false;
  }
  return true;
}

// return a pointer to the data of the given settings block
uint8_t * GSL_PSET_GetData(uint8_t * settings_block) {
  return settings_block + 3;
}

// return the data length of the given settings block
uint16_t GSL_PSET_GetDataLength(uint8_t * settings_block) {
  uint16_t data_length;
  GSL_PSET_ReadFromFlash(settings_block + 1, data_length);
  return data_length;
}

// return the data length of the given settings block
uint8_t GSL_PSET_GetIdentifier(uint8_t * settings_block) {
  return *settings_block;
}

// return the address just past the given settings block
uint8_t * GSL_PSET_SkipSettingsBlock(uint8_t * address) {
  ASSERT(GSL_PSET_IsBlockValid(address));
  address += 1 + 2 + GSL_PSET_GetDataLength(address) + gsl_pset_sync_length;
  return address;
}

// get pointer to last settings block of the given identifier
// return nullptr if none found
uint8_t * GSL_PSET_GetSettingsPtr(uint8_t identifier = 0) {
  if (!GSL_PSET_IsSectorInitialized()) {
    return nullptr;
  }
  uint8_t * address = gsl_pset_mem_start;
  address += gsl_pset_sync_length;
  uint8_t * settings_ptr = nullptr;
  // look though all settings blocks and return the last one found, if any
  while (GSL_PSET_IsBlockValid(address)) {
    if (GSL_PSET_GetIdentifier(address) == identifier) {
      settings_ptr = address;
    }
    address = GSL_PSET_SkipSettingsBlock(address);
  }
  return settings_ptr;
}

// get pointer to the next settings block address to write
uint8_t * GSL_PSET_GetNewSettingsPtr(void) {
  uint8_t * address = gsl_pset_mem_start + gsl_pset_sync_length;
  while (GSL_PSET_IsBlockValid(address)) {
    address = GSL_PSET_SkipSettingsBlock(address);
  }
  // ensure the flash from this point forward is erased
  for (auto check = address; check < gsl_pset_mem_end; ++check) {
    if (*check != 0xFF) {
      GSL_PSET_Error(kGslPsetErrorUnexpectedWrittenBytes);
    }
  }
  return address;
}

// write a settings block to the given address
void GSL_PSET_WriteSettingsBlock(const uint8_t * address,
                                 uint8_t id,
                                 const void * data,
                                 uint16_t data_size) {
  auto original_address = address;
  GSL_PSET_WriteToFlash(address, id);
  address += sizeof(id);
  GSL_PSET_WriteToFlash(address, data_size);
  address += sizeof(data_size);
  GSL_PSET_WriteToFlash(address, data, data_size);
  address += data_size;
  GSL_PSET_WriteToFlash(address, gsl_pset_sync, gsl_pset_sync_length);
  ASSERT(GSL_PSET_IsBlockValid((uint8_t *) original_address));
}

// erase the pset sector and copy over settings other than the given
// identifier
void GSL_PSET_EraseAndTransferSettings(uint8_t identifier_to_ignore) {
  GSL_BITVECTOR id_present(256);
  uint8_t * address = gsl_pset_mem_start + gsl_pset_sync_length;
  // find all identifiers present
  while (GSL_PSET_IsBlockValid(address)) {
    address = GSL_PSET_SkipSettingsBlock(address);
    id_present.Set(GSL_PSET_GetIdentifier(address));
  }
  // ignore the specified identifier
  id_present.Clear(identifier_to_ignore);
  // number of bytes needed to store all current settings blocks needed to
  // transfer
  uint32_t memory_needed = 0;
  // add up memory required to store all the given blocks
  // to store them, we prepend them with the identifier and the size (uiuint16_t)
  for (uint16_t i = 0; i < 256; ++i) {
    if (!id_present.Get(i)) {
      continue;
    }
    uint8_t * address = GSL_PSET_GetSettingsPtr(i);
    memory_needed += 3;
    memory_needed += GSL_PSET_GetDataLength(address);
  }
  // allocate temporary memory
  uint8_t * const buffer_start =
      (uint8_t *) GSL_BUF_CreateTemporary(memory_needed);
  uint8_t * const buffer_end = buffer_start + memory_needed;
  // store data from each identifier
  uint8_t * buffer = buffer_start;
  for (uint16_t i = 0; i < 256; ++i) {
    if (id_present.Get(i)) {
      uint8_t * address = GSL_PSET_GetSettingsPtr(i);
      uint16_t data_size = GSL_PSET_GetDataLength(address);
      uint8_t * data = GSL_PSET_GetData(address);
      // store id, data size, and data
      *buffer = i;
      ++buffer;
      memcpy(buffer, &data_size, sizeof(data_size));
      buffer += sizeof(data_size);
      memcpy(buffer, data, data_size);
      buffer += data_size;
    }
  }
  ASSERT(buffer == buffer_end);
  // now erase block
  GSL_PSET_EraseSector();
  // initialize it
  GSL_PSET_Initialize();
  // write all settings blocks that need transfered
  buffer = buffer_start;
  while (buffer != buffer_end) {
    uint8_t id = *buffer;
    ++buffer;
    uint16_t data_size;
    memcpy(&data_size, buffer, sizeof(data_size));
    buffer += sizeof(data_size);
    GSL_PSET_WriteSettingsBlock(GSL_PSET_GetNewSettingsPtr(),
                                id,
                                buffer,
                                data_size);
    buffer += data_size;
  }
  // double check all blocks that should exist do exist
  for (uint16_t i = 0; i < 256; ++i) {
    if (id_present.Get(i)) {
      ASSERT(GSL_PSET_GetSettingsPtr(i) != nullptr);
    } else {
      ASSERT(GSL_PSET_GetSettingsPtr(i) == nullptr);
    }
  }
}

// store a new settings block
void GSL_PSET_Store(const void * data,
                    uint16_t data_length,
                    uint8_t identifier = 0) {
  // initialize
  GSL_PSET_Initialize();
  // find last entry for this data and see if it matches the current data
  // if so, no need to write it again
  {
    uint8_t * address = GSL_PSET_GetSettingsPtr(identifier);
    // see if it matches the current data we want to store
    if (address != nullptr) {
      if (GSL_PSET_GetDataLength(address) == data_length &&
          memcmp(data,
                 GSL_PSET_GetData(address),
                 data_length) == 0) {
        // data matches the last entry, no need to rewrite it
        return;
      }
    }
  }
  // find location of new block to write
  uint8_t * address = GSL_PSET_GetNewSettingsPtr();
  // ensure there's enough space to write a new block
  uint8_t * end = address + 1 + 2 + data_length + gsl_pset_sync_length;
  if (end > gsl_pset_mem_end) {
    HALT("TODO");
    GSL_PSET_EraseAndTransferSettings(identifier);
    GSL_PSET_Store(data, data_length, identifier);
    return;
  }
  // write new settings block
  uint8_t * settings_ptr = address;
  GSL_PSET_WriteSettingsBlock(address, identifier, data, data_length);
  // ensure we return the correct value
  ASSERT(GSL_PSET_GetSettingsPtr(identifier) == settings_ptr);
}

// store a settings block
template <class T>
void GSL_PSET_StoreItem(const T & data,
                        uint8_t identifier = 0) {
  GSL_PSET_Store(&data, sizeof(data), identifier);
}

// recall settings block
// return true if successful
// this will return false if the data lengths do not match
bool GSL_PSET_Recall(void * data,
                     uint16_t data_length,
                     uint8_t identifier = 0) {
  auto address = GSL_PSET_GetSettingsPtr(identifier);
  if (address == nullptr) {
    return false;
  }
  if (GSL_PSET_GetDataLength(address) != data_length) {
    return false;
  }
  GSL_PSET_ReadFromFlash(GSL_PSET_GetData(address), data, data_length);
  return true;
}

// recall a settings block
template <class T>
bool GSL_PSET_RecallItem(T & data,
                         uint8_t identifier = 0) {
  return GSL_PSET_Recall(&data, sizeof(data), identifier);
}

// return the length in bytes of the pset sector in use
uint32_t GSL_PSET_GetBytesInUse(void) {
  if (!GSL_PSET_IsSectorInitialized()) {
    return 0;
  }
  return GSL_PSET_GetNewSettingsPtr() - gsl_pset_mem_start;
}

// return the available bytes in the pset sector
uint32_t GSL_PSET_GetBytesAvilable(void) {
  if (!GSL_PSET_IsSectorInitialized()) {
    return 0;
  }
  return gsl_pset_mem_end - GSL_PSET_GetNewSettingsPtr();
}

// log the status of the PSET sector
void GSL_PSET_LogStatus(void) {
  LOG("\n\nPSET sector status:");
  if (!GSL_PSET_IsSectorInitialized()) {
    LOG("\n- sector is NOT initialized.");
    if (GSL_PSET_IsSectorErased()) {
      LOG("\n- sector is erased.");
    } else {
      LOG("\n- sector is NOT erased.");
    }
    return;
  }
  LOG("\n- sector is initialized");
  // loop through and print the identifier and data length of all valid blocks
  uint8_t * address = gsl_pset_mem_start;
  address += gsl_pset_sync_length;
  // look though all settings blocks and return the last one found, if any
  while (GSL_PSET_IsBlockValid(address)) {
    LOG("\n- Valid block at ", GSL_OUT_Hex((uint32_t) address));
    LOG(": ID=",
        GSL_PSET_GetIdentifier(address),
        ", data_length=",
        GSL_PSET_GetDataLength(address));
    address = GSL_PSET_SkipSettingsBlock(address);
  }
  LOG("\n- End of sector");
}

// do some checks on these functions
void GSL_PSET_PerformSanityChecks(void) {
  LOG("\n\nPerforming PSET checks");
  if (GSL_PSET_IsSectorInitialized()) {
    LOG("\n- Sector is initialized");
  } else {
    LOG("\n- Sector is NOT initialized");
    if (GSL_PSET_IsSectorErased()) {
      LOG("\n- Sector is erased");
    } else {
      LOG("\n- Sector is NOT erased");
    }
  }
  // erase sector
  GSL_PSET_Initialize();
  ASSERT(GSL_PSET_IsSectorInitialized());
  LOG("\n- Sector successfully initialized");
  GSL_PSET_EraseSector();
  ASSERT(GSL_PSET_IsSectorErased());
  ASSERT(!GSL_PSET_IsSectorInitialized());
  LOG("\n- Sector successfully erased");
  GSL_PSET_Initialize();
  LOG("\n- Sector is initialized");
  // write stuff
  uint8_t info[4] = {1, 2, 3, 4};
  uint8_t info_check[4];
  GSL_PSET_Store(info, sizeof(info));
  LOG("\n- Settings stored");
  ASSERT(GSL_PSET_Recall(info_check, sizeof(info_check)));
  ASSERT(memcmp(info, info_check, sizeof(info)) == 0);
  LOG("\n- Settings matched");
  ++info[0];
  GSL_PSET_Store(info, sizeof(info));
  LOG("\n- Settings stored again");
  ASSERT(GSL_PSET_Recall(info_check, sizeof(info_check)));
  ASSERT(memcmp(info, info_check, sizeof(info)) == 0);
  LOG("\n- Settings matched");
  GSL_PSET_EraseSector();
  ASSERT(GSL_PSET_IsSectorErased());
  LOG("\n- checks passed");
}
