#include <stdio.h>
#include <string.h>
#include "libiot.h"

#define SLAVE_ADDR_SHT30   "44"
#define SLAVE_ADDR_QMP6988 "70"

#define QMP6988_RESET_REG        0xE0 /* Device reset register */
#define QMP6988_PRESSURE_MSB_REG 0xF7 /* Pressure MSB Register */

typedef struct sEnvIII{
  float ctmp; /* SHT30 (Temperature in Celsius)    */
  float ftmp; /*       (Temperature in Fahrenheit) */
  float humi; /*       (Relative Humidity)         */
  float pres; /* QMP6988 */
} EnvIII;

void software_reset(){
  unsigned char buf[2];
  buf[0] = QMP6988_RESET_REG;        // software reset(1)
  buf[1] = 0xe6;
  iotputb("device/i2c_a/" SLAVE_ADDR_QMP6988, buf, 2);
  buf[0] = QMP6988_RESET_REG;        // software reset(2)
  buf[1] = 0x00;
  iotputb("device/i2c_a/" SLAVE_ADDR_QMP6988, buf, 2);
}

void calc_pressure(EnvIII* env) { // https://github.com/m5stack/M5Unit-ENV/blob/355806b28718852b95c94d3965858202444ae3e7/src/QMP6988.cpp#L302
  software_reset();
  unsigned char buf[16];
  buf[0] = QMP6988_PRESSURE_MSB_REG; // read pressure
  iotputb("device/i2c_a/" SLAVE_ADDR_QMP6988, buf, 1);
  iotgetb("device/i2c_a/" SLAVE_ADDR_QMP6988, buf);
  for(int i =0; i < 16; i++) {
    printf("[%02d]=%02X\n", i, buf[i]);
  }
  env->pres = 0.0f; // TODO
}

void get_env(EnvIII* env) { // https://github.com/m5stack/M5Unit-ENV/blob/master/src/SHT3X.cpp#L10
  unsigned char buf[16];
  buf[0] = 0x2C;
  buf[1] = 0x06;
  iotputb("device/i2c_a/" SLAVE_ADDR_SHT30, buf, 2);
  iotgetb("device/i2c_a/" SLAVE_ADDR_SHT30, buf); // warning 16 bytes write! (but use only 6 bytes)
  env->ctmp = ((((buf[0] * 256.0f) + buf[1]) * 175.0f) / 65535.0f) - 45.0f;
  env->ftmp = (env->ctmp * 1.8f) + 32.0f;
  env->humi = ((((buf[3] * 256.0)  + buf[4]) * 100.0f) / 65535.0);
}

int init_env(char* addr) {
  int num = iotfindi("device/i2c_a");
  if (num == -1) { return -1; }
  char** str = malloc(sizeof(char*) * num);
  for(int i = 0; i < num; i++) {
    str[i] = malloc(sizeof(char) * 64);
  }
  int cnt = iotfinds("device/i2c_a", str, num);
  int found = -1;
  for(int i = 0; i < num; i++) {
    if (strcmp(str[i], addr) == 0){
      found = 0;
    }
    free(str[i]);
  }
  free(str);
  return found;
}

int main( int argc, char *argv[]) {
  if (init_env(SLAVE_ADDR_SHT30)) {
    printf("SHT30(0x%s) not found.", SLAVE_ADDR_SHT30);
    return -1;
  }
  if (init_env(SLAVE_ADDR_QMP6988)) {
    printf("QMP6988(0x%s) not found.", SLAVE_ADDR_QMP6988);
    return -1;
  }
  EnvIII env;
  get_env(&env);
  calc_pressure(&env);
  printf("temperature: %2.0f %cC\n", env.ctmp, 0xdf);
  printf("temperature: %2.0f %cF\n", env.ftmp, 0xdf);
  printf("humidity   : %2.0f %c\n",  env.humi, 0x25);
  printf("pressure   : %2.0f Pa",    env.pres);
  return 0;
}
