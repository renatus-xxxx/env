#ifndef __LIBIOT_H__
#define __LIBIOT_H__

int iotgeti(const char* device_path);
int iotgets(const char* device_path, char* result);
int iotgetb(const char* device_path, char* bytes);
int iotputi(const char* device_path, int   value);
int iotputs(const char* device_path, char* value);
int iotputb(const char* device_path, char* bytes, char len);
int iotfindi(const char* device_path);
int iotfinds(const char* device_path, char** result, int num);

#endif
