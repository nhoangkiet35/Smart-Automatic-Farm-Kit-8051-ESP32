/*
 * Created Date: Saturday, July 5th 2025, 9:27:29 pm
 * Author: Hoang Kiet
 *
 * Purpose: Interfaces with the DHT11 sensor to measure air temperature and humidity.
 */
#ifndef __DHT11_H__
#define __DHT11_H__

#include <REGX52.H>

// Define pins to communicate with DHT11
sbit DHT11 = P1 ^ 7; // Pin P1.7 connects to DHT11

// Global variable to store data from DHT11
extern int I_RH;     // Integer humidity
extern int D_RH;     // Decimal humidity
extern int I_Temp;   // Integer temperature
extern int D_Temp;   // Decimal temperature
extern int CheckSum; // Error check byte

// Function declaration
void timer_delay20ms(void); // Create 20ms delay for request signal
void timer_delay30us(void); // Create 30µs delay to read data bit
void Request(void);         // Send request signal to DHT11
void Response(void);        // Receive response from DHT11
int Receive_data(void);     // Receive 1 data byte (8 bits)

unsigned int getTemperature(void);
unsigned int getHumidity(void);

#endif