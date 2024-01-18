#ifndef INI_CONFIG_MOCK_H
#define INI_CONFIG_MOCK_H

#include <bdd-for-c-mocks.h>
#include <ini-config.h>

mock(INI_ConfigInitialize) INI_ConfigInitializeResult INI_ConfigInitialize(INI_ConfigItem _item[], const char* _config_path, u8 _argc, const char* _argv[]) {
	mock_return(INI_ConfigInitialize) INI_CONFIG_INITIALIZE_RESULT_OK;
}

mock(INI_ConfigSave) INI_ConfigSaveResult INI_ConfigSave(INI_ConfigItem _item[], const char* _config_path) {
	mock_return(INI_ConfigSave) 0;
}

mock(INI_ConfigGetCommandlineOption) const char* INI_ConfigGetCommandlineOption(uint8_t _argc, const char* _argv[], const char* _val) {
	mock_return(INI_ConfigGetCommandlineOption) "";
}

mock(INI_ConfigLoadU8) void INI_ConfigLoadU8(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigLoadU8);
}

mock(INI_ConfigLoadU16) void INI_ConfigLoadU16(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigLoadU16);
}

mock(INI_ConfigLoadU32) void INI_ConfigLoadU32(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigLoadU32);
}

mock(INI_ConfigLoadI8) void INI_ConfigLoadI8(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigLoadI8);
}

mock(INI_ConfigLoadI16) void INI_ConfigLoadI16(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigLoadI16);
}

mock(INI_ConfigLoadI32) void INI_ConfigLoadI32(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigLoadI32);
}

mock(INI_ConfigLoadString) void INI_ConfigLoadString(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigLoadString);
}

mock(INI_ConfigSaveU8) INI_ConfigSaveItemResult INI_ConfigSaveU8(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigSaveU8) INI_CONFIG_SAVE_ITEM_OK;
}

mock(INI_ConfigSaveU16) INI_ConfigSaveItemResult INI_ConfigSaveU16(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigSaveU16) INI_CONFIG_SAVE_ITEM_OK;
}

mock(INI_ConfigSaveU32) INI_ConfigSaveItemResult INI_ConfigSaveU32(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigSaveU32) INI_CONFIG_SAVE_ITEM_OK;
}

mock(INI_ConfigSaveI8) INI_ConfigSaveItemResult INI_ConfigSaveI8(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigSaveI8) INI_CONFIG_SAVE_ITEM_OK;
}

mock(INI_ConfigSaveI16) INI_ConfigSaveItemResult INI_ConfigSaveI16(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigSaveI16) INI_CONFIG_SAVE_ITEM_OK;
}

mock(INI_ConfigSaveI32) INI_ConfigSaveItemResult INI_ConfigSaveI32(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigSaveI32) INI_CONFIG_SAVE_ITEM_OK;
}

mock(INI_ConfigSaveString) INI_ConfigSaveItemResult INI_ConfigSaveString(INI_ConfigItem* _item, const char* _config_path, void* _item_array) {
	mock_return(INI_ConfigSaveString) INI_CONFIG_SAVE_ITEM_OK;
}

mock(INI_ConfigOverrideU8) void INI_ConfigOverrideU8(INI_ConfigItem* _item, uint8_t _argc, const char* _argv[]) {
	mock_return(INI_ConfigOverrideU8);
}

mock(INI_ConfigOverrideU16) void INI_ConfigOverrideU16(INI_ConfigItem* _item, uint8_t _argc, const char* _argv[]) {
	mock_return(INI_ConfigOverrideU16);
}

mock(INI_ConfigOverrideU32) void INI_ConfigOverrideU32(INI_ConfigItem* _item, uint8_t _argc, const char* _argv[]) {
	mock_return(INI_ConfigOverrideU32);
}

mock(INI_ConfigOverrideI8) void INI_ConfigOverrideI8(INI_ConfigItem* _item, uint8_t _argc, const char* _argv[]) {
	mock_return(INI_ConfigOverrideI8);
}

mock(INI_ConfigOverrideI16) void INI_ConfigOverrideI16(INI_ConfigItem* _item, uint8_t _argc, const char* _argv[]) {
	mock_return(INI_ConfigOverrideI16);
}

mock(INI_ConfigOverrideI32) void INI_ConfigOverrideI32(INI_ConfigItem* _item, uint8_t _argc, const char* _argv[]) {
	mock_return(INI_ConfigOverrideI32);
}

mock(INI_ConfigOverrideString) void INI_ConfigOverrideString(INI_ConfigItem* _item, uint8_t _argc, const char* _argv[]) {
	mock_return(INI_ConfigOverrideString);
}


#endif
