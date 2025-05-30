/******************************************************************************
SparkFunDS1307RTC.cpp
Jim Lindblom @ SparkFun Electronics
original creation date: October 2, 2016
https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library

Implementation of DS1307 real time clock functions

Resources:
Wire.h - Arduino I2C Library

Development environment specifics:
Arduino 1.6.8
SparkFun RedBoard
SparkFun Real Time Clock Module (v14)
******************************************************************************/

#include "DS1307RTC.h"

DS1307::DS1307() {
	for (int i=0; i<TIME_ARRAY_LENGTH; i++)
		_time[i] = 0;

	_pm = false;
}

// setTime -- Set time and date/day registers of DS1307
bool DS1307::setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint8_t year) {
	_time[TIME_SECONDS] = DECtoBCD(sec);
	_time[TIME_MINUTES] = DECtoBCD(min);
	_time[TIME_HOURS] = DECtoBCD(hour);
	_time[TIME_DAY] = DECtoBCD(day);
	_time[TIME_DATE] = DECtoBCD(date);
	_time[TIME_MONTH] = DECtoBCD(month);
	_time[TIME_YEAR] = DECtoBCD(year);
	
	return setTime(_time, TIME_ARRAY_LENGTH);
}

// setTime -- Set time and date/day registers of DS1307 (using data array)
bool DS1307::setTime(uint8_t *time, uint8_t len) {
	if (len != TIME_ARRAY_LENGTH)
		return false;
	
	return i2cWriteBytes(DS1307_RTC_ADDRESS, DS1307_REGISTER_BASE, time, TIME_ARRAY_LENGTH);
}

// update -- Read all time/date registers and update the _time array
bool DS1307::update(void) {
	uint8_t rtcReads[7];
	bool success = false;
	int tries = 0;

	while (!success && tries<10) {
		success = i2cReadBytes(DS1307_RTC_ADDRESS, DS1307_REGISTER_SECONDS, rtcReads, 7);
		tries++;
		delay(10);
	}

	if (!success) return false;
	
	for (int i=0; i<TIME_ARRAY_LENGTH; i++)
		_time[i] = rtcReads[i];
	
	_time[TIME_SECONDS] &= 0x7F; // Mask out CH bit
	
	if (_time[TIME_HOURS] & TWELVE_HOUR_MODE) {
		if (_time[TIME_HOURS] & TWELVE_HOUR_PM)
			_pm = true;
		else
			_pm = false;
		_time[TIME_HOURS] &= 0x1F; // Mask out 24-hour bit from hours
	}
	
	return true;
}

// getSecond -- read/return seconds register of DS1307
uint8_t DS1307::getSecond(void) {
	_time[TIME_SECONDS] = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_SECONDS);
	_time[TIME_SECONDS] &= 0x7F; // Mask out CH bit

	return BCDtoDEC(_time[TIME_SECONDS]);
}

// getMinute -- read/return minutes register of DS1307
uint8_t DS1307::getMinute(void) {
	_time[TIME_MINUTES] = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_MINUTES);

	return BCDtoDEC(_time[TIME_MINUTES]);	
}

// getHour -- read/return hour register of DS1307
uint8_t DS1307::getHour(void) {
	uint8_t hourRegister = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_HOURS);
	
	if (hourRegister & TWELVE_HOUR_MODE)
		hourRegister &= 0x1F; // Mask out am/pm, 24-hour bit
	_time[TIME_HOURS] = hourRegister;

	return BCDtoDEC(_time[TIME_HOURS]);
}

// getDay -- read/return day register of DS1307
uint8_t DS1307::getDay(void) {
	_time[TIME_DAY] = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_DAY);

	return BCDtoDEC(_time[TIME_DAY]);		
}

// getDate -- read/return date register of DS1307
uint8_t DS1307::getDate(void) {
	_time[TIME_DATE] = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_DATE);

	return BCDtoDEC(_time[TIME_DATE]);		
}

// getMonth -- read/return month register of DS1307
uint8_t DS1307::getMonth(void) {
	_time[TIME_MONTH] = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_MONTH);

	return BCDtoDEC(_time[TIME_MONTH]);	
}

// getYear -- read/return year register of DS1307
uint8_t DS1307::getYear(void) {
	_time[TIME_YEAR] = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_YEAR);

	return BCDtoDEC(_time[TIME_YEAR]);		
}

// setSecond -- set the second register of the DS1307
bool DS1307::setSecond(uint8_t s) {
	if (s <= 59) {
		uint8_t _s = DECtoBCD(s);
		return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_SECONDS, _s);
	}
	
	return false;
}

// setMinute -- set the minute register of the DS1307
bool DS1307::setMinute(uint8_t m) {
	if (m <= 59) {
		uint8_t _m = DECtoBCD(m);
		return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_MINUTES, _m);		
	}
	
	return false;
}

// setHour -- set the hour register of the DS1307
bool DS1307::setHour(uint8_t h) {
	if (h <= 23) {
		uint8_t _h = DECtoBCD(h);
		return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_HOURS, _h);
	}
	
	return false;
}

// setDay -- set the day register of the DS1307
bool DS1307::setDay(uint8_t d) {
	if ((d >= 1) && (d <= 7)) {
		uint8_t _d = DECtoBCD(d);
		return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_DAY, _d);
	}	
	
	return false;
}

// setDate -- set the date register of the DS1307
bool DS1307::setDate(uint8_t d) {
	if (d <= 31) {
		uint8_t _d = DECtoBCD(d);
		return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_DATE, _d);
	}	
	
	return false;
}

// setMonth -- set the month register of the DS1307
bool DS1307::setMonth(uint8_t mo) {
	if ((mo >= 1) && (mo <= 12)) {
		uint8_t _mo = DECtoBCD(mo);
		return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_MONTH, _mo);
	}	
	
	return false;	
}

// setYear -- set the year register of the DS1307
bool DS1307::setYear(uint8_t y) {
	if (y <= 99) {
		uint8_t _y = DECtoBCD(y);
		return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_YEAR, _y);
	}	
	
	return false;	
}

// set12Hour -- set (or not) to 12-hour mode) | enable12 defaults to  true
bool DS1307::set12Hour(bool enable12) {
	if (enable12)
		return set24Hour(false);
	else
		return set24Hour(true);
}

// set24Hour -- set (or not) to 24-hour mode) | enable24 defaults to  true
bool DS1307::set24Hour(bool enable24) {
	uint8_t hourRegister = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_HOURS);
	
	bool hour12 = hourRegister & TWELVE_HOUR_MODE;
	if ((hour12 && !enable24) || (!hour12 && enable24))
		return true;
	
	uint8_t oldHour = hourRegister & 0x1F; // Mask out am/pm and 12-hour mode
	oldHour = BCDtoDEC(oldHour); // Convert to decimal
	uint8_t newHour = oldHour;
	
	if (enable24) {
		bool hourPM = hourRegister & TWELVE_HOUR_PM;
		if ((hourPM) && (oldHour >= 1)) newHour += 12;
		else if (!(hourPM) && (oldHour == 12)) newHour = 0;
		newHour = DECtoBCD(newHour);
	}
	else {
		if (oldHour == 0) 
			newHour = 12;
		else if (oldHour >= 13)
			newHour -= 12;
		
		newHour = DECtoBCD(newHour);
		newHour |= TWELVE_HOUR_MODE; // Set bit 6 to set 12-hour mode
		if (oldHour >= 12)
			newHour |= TWELVE_HOUR_PM; // Set PM bit if necessary
	}
	
	return i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_HOURS, newHour);
}

// is12Hour -- check if the DS1307 is in 12-hour mode
bool DS1307::is12Hour(void) {
	uint8_t hourRegister = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_HOURS);
	
	return hourRegister & TWELVE_HOUR_MODE;
}

// pm -- Check if 12-hour state is AM or PM
// Read bit 5 in hour byte
bool DS1307::pm(void) {
	uint8_t hourRegister = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_HOURS);
	
	return hourRegister & TWELVE_HOUR_PM;	
}

// enable -- enable the DS1307's oscillator
// Write 0 to CH bit
void DS1307::enable(void) {
	uint8_t secondRegister = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_SECONDS);
	
	secondRegister &= ~(1<<7);
	
	i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_SECONDS, secondRegister);
}

// disable -- disable the DS1307's oscillator
// Write 1 to CH bit
void DS1307::disable(void) {
	uint8_t secondRegister = i2cReadByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_SECONDS);
	
	secondRegister |= (1<<7);
	
	i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_SECONDS, secondRegister);
}

// writeSQW -- Set the SQW pin high or low
void DS1307::writeSQW(uint8_t high) {
	if (high) 
		writeSQW(SQW_HIGH);
	else 
		writeSQW(SQW_LOW);
}
	
// writeSQW -- Set the SQW pin high, low, or to one of the square wave frequencies
void DS1307::writeSQW(sqw_rate value) {
	uint8_t controlRegister = 0;

	if (value == SQW_HIGH)
		controlRegister |= CONTROL_BIT_OUT;
	else {
		controlRegister |= CONTROL_BIT_SQWE;	
		controlRegister |= value;
	}
	
	i2cWriteByte(DS1307_RTC_ADDRESS, DS1307_REGISTER_CONTROL, controlRegister);
}

// BCDtoDEC -- convert binary-coded decimal (BCD) to decimal
uint8_t DS1307::BCDtoDEC(uint8_t val) {
	return ( ( val / 0x10) * 10 ) + ( val % 0x10 );
}

// BCDtoDEC -- convert decimal to binary-coded decimal (BCD)
uint8_t DS1307::DECtoBCD(uint8_t val) {
	return ( ( val / 10 ) * 0x10 ) + ( val % 10 );
}

// i2cWriteBytes -- write a set number of bytes to an i2c device, incrementing from a register
bool DS1307::i2cWriteBytes(uint8_t deviceAddress, ds1307_registers reg, uint8_t *values, uint8_t len) {
	Wire.beginTransmission(deviceAddress);
	Wire.write(reg);

	for (int i=0; i<len; i++)
		Wire.write(values[i]);

	if (Wire.endTransmission()>2) return false;
	
	return true;
}

// i2cWriteByte -- write a byte value to an i2c device's register
bool DS1307::i2cWriteByte(uint8_t deviceAddress, ds1307_registers reg, uint8_t value) {
	Wire.beginTransmission(deviceAddress);
	Wire.write(reg);
	Wire.write(value);
	if (Wire.endTransmission()>2) return false;
	
	return true;
}

// i2cReadByte -- read a byte from an i2c device's register
uint8_t DS1307::i2cReadByte(uint8_t deviceAddress, ds1307_registers reg) {
	Wire.beginTransmission(deviceAddress);
	Wire.write(reg);
	if (Wire.endTransmission()) return false;

	Wire.requestFrom(deviceAddress, (uint8_t) 1);
	
	return Wire.read();
}

// i2cReadBytes -- read a set number of bytes from an i2c device, incrementing from a register
bool DS1307::i2cReadBytes(uint8_t deviceAddress, ds1307_registers reg, uint8_t *dest, uint8_t len) {  
	Wire.beginTransmission(deviceAddress);
	Wire.write(reg);
	if (Wire.endTransmission()>2) return false;

	if (Wire.requestFrom(deviceAddress, len) != len ) return false;

	for (int i=0; i<len; i++)
		dest[i] = Wire.read();
  
	return true;
}