/* This file is part of the Razor AHRS Firmware */

// I2C code to read the sensors

// Sensor I2C addresses
#define GYRO_ADDRESS ((int) 0x68) // 0x68 = 0xD0 / 2
#define ACC_MAG_ADDRESS  ((int) 0x1E) // 0x1E = 0x3C / 2

// Arduino backward compatibility macros
#if ARDUINO >= 100
  #define WIRE_SEND(b) Wire.write((byte) b) 
  #define WIRE_RECEIVE() Wire.read() 
#else
  #define WIRE_SEND(b) Wire.send(b)
  #define WIRE_RECEIVE() Wire.receive() 
#endif


void I2C_Init()
{
  Wire.begin();
}

void Acc_Mag_Init()
{

  //set acceleration data rate to 50Hz, enable all acc axes
  Wire.beginTransmission(ACC_MAG_ADDRESS);
  WIRE_SEND(0x20);//CTRL1 register
  WIRE_SEND(0x57);//01010111
  Wire.endTransmission();
  delay(5); 
  
  //set acceleration scale +/-16G
  Wire.beginTransmission(ACC_MAG_ADDRESS);
  WIRE_SEND(0x21);//CTRL2 register
  WIRE_SEND(0x20);//0010000
  Wire.endTransmission();
  delay(5); 
  
  //set magnetic resolution to high and data rate to 50Hz
  Wire.beginTransmission(ACC_MAG_ADDRESS);
  WIRE_SEND(0x24);//CTRL5 register
  WIRE_SEND(0x70);//01110000
  Wire.endTransmission();
  delay(5); 
  
  //set magnetic scale to +/-8 Gauss
  Wire.beginTransmission(ACC_MAG_ADDRESS);
  WIRE_SEND(0x25);//CTRL6 register
  WIRE_SEND(0x40);//01000000
  Wire.endTransmission();
  delay(5); 
  
  //set magnetometer to continuous mode
  Wire.beginTransmission(ACC_MAG_ADDRESS);
  WIRE_SEND(0x26);//CTRL7 register
  WIRE_SEND(0x00);//
  Wire.endTransmission();
  delay(5); 
 
}

void Read_Magn()
{
  int i = 0;
  byte buff[6];
  
  Wire.beginTransmission(ACC_MAG_ADDRESS); 
  WIRE_SEND(0x08);  // Sends address to read from
  Wire.endTransmission();
  
  Wire.beginTransmission(ACC_MAG_ADDRESS);
  Wire.requestFrom(ACC_MAG_ADDRESS, 6);  // Request 6 bytes
  while(Wire.available())  // ((Wire.available())&&(i<6))
  { 
    buff[i] = WIRE_RECEIVE();  // Read one byte
    i++;
  }
  Wire.endTransmission();
  
  if (i == 6)  // All bytes received?
  {
    magnetom[0] = ((((int) buff[2]) << 8) | buff[3]);    // X axis (internal sensor y axis)
    magnetom[1] = ((((int) buff[0]) << 8) | buff[1]);    // Y axis (internal sensor x axis)
    magnetom[2] = ((((int) buff[4]) << 8) | buff[5]);    // Z axis (internal sensor z axis)
  }
  else
  {
    num_magn_errors++;
    if (output_errors) Serial.println("!ERR: reading acc/mag");
  }
}

void Read_Accel(){
  int i = 0;
  byte buff[6];
  
  Wire.beginTransmission(ACC_MAG_ADDRESS); 
  WIRE_SEND(0x28);  // Sends address to read from
  Wire.endTransmission();
  
  Wire.beginTransmission(ACC_MAG_ADDRESS);
  Wire.requestFrom(ACC_MAG_ADDRESS, 6);  // Request 6 bytes
  while(Wire.available())  // ((Wire.available())&&(i<6))
  { 
    buff[i] = WIRE_RECEIVE();  // Read one byte
    i++;
  }
  Wire.endTransmission();
  
  if (i == 6)  // All bytes received?
  {
    accel[0] = ((((int) buff[2]) << 8) | buff[3]);    // X axis (internal sensor y axis)
    accel[1] = ((((int) buff[0]) << 8) | buff[1]);    // Y axis (internal sensor x axis)
    accel[2] = ((((int) buff[4]) << 8) | buff[5]);    // Z axis (internal sensor z axis)
  }
  else
  {
    num_accel_errors++;
    if (output_errors) Serial.println("!ERR: reading acc/mag");
  }
}

void Gyro_Init()
{
  // Power up reset defaults
  Wire.beginTransmission(GYRO_ADDRESS);
  WIRE_SEND(0x3E);
  WIRE_SEND(0x80);
  Wire.endTransmission();
  delay(5);
  
  // Select full-scale range of the gyro sensors
  // Set LP filter bandwidth to 42Hz
  Wire.beginTransmission(GYRO_ADDRESS);
  WIRE_SEND(0x16);
  WIRE_SEND(0x1B);  // DLPF_CFG = 3, FS_SEL = 3
  Wire.endTransmission();
  delay(5);
  
  // Set sample rato to 50Hz
  Wire.beginTransmission(GYRO_ADDRESS);
  WIRE_SEND(0x15);
  WIRE_SEND(0x0A);  //  SMPLRT_DIV = 10 (50Hz)
  Wire.endTransmission();
  delay(5);

  // Set clock to PLL with z gyro reference
  Wire.beginTransmission(GYRO_ADDRESS);
  WIRE_SEND(0x3E);
  WIRE_SEND(0x00);
  Wire.endTransmission();
  delay(5);
}

// Reads x, y and z gyroscope registers
void Read_Gyro()
{
  int i = 0;
  byte buff[6];
  
  Wire.beginTransmission(GYRO_ADDRESS); 
  WIRE_SEND(0x1D);  // Sends address to read from
  Wire.endTransmission();
  
  Wire.beginTransmission(GYRO_ADDRESS);
  Wire.requestFrom(GYRO_ADDRESS, 6);  // Request 6 bytes
  while(Wire.available())  // ((Wire.available())&&(i<6))
  { 
    buff[i] = WIRE_RECEIVE();  // Read one byte
    i++;
  }
  Wire.endTransmission();
  
  if (i == 6)  // All bytes received?
  {
    gyro[0] = -1 * ((((int) buff[2]) << 8) | buff[3]);    // X axis (internal sensor -y axis)
    gyro[1] = -1 * ((((int) buff[0]) << 8) | buff[1]);    // Y axis (internal sensor -x axis)
    gyro[2] = -1 * ((((int) buff[4]) << 8) | buff[5]);    // Z axis (internal sensor -z axis)
  }
  else
  {
    num_gyro_errors++;
    if (output_errors) Serial.println("!ERR: reading gyroscope");
  }
}
