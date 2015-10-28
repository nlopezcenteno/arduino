
/*
LightTheremin

Esta es mi propia versión del ejemplo LightThermin del Arduino starter kit.

Utiliza un fotorresitor para las notas y otro fotorresistor para el ritmo.
Cada fotorresistor se calibra de forma independiente y la calibración se guarda en la EPROM.
Un "pushbutton" permite volver a calibrar: 5 segundos de calibración para las notas, una pausa de
de 3 segundos y 5 segundos de calibración para el ritmo. El led 13 se enciende cuando se está 
calibrando cada fotorresitor.  

Circuito:
* fotorresistor en pin A0 (notas)
* fotorresistor en pin A1 (ritmo)
* pushbutton para calibración en pin 12
* piezo en pin 8

creado 26 Oct 2015
por Nelson López Centeno (@nlopezcenteno)

He usado código e ideas de todas partes, incluidos:
http://www.arduino.cc/starterKit
http://garagelab.com/profiles/blogs/how-to-use-tone-function-arduino-playing-the-james-bond-theme
http://forum.arduino.cc/index.php?topic=46900.0

*/

#include <EEPROM.h>
#include "pitches.h"
#include "structs.h"
//#define DEBUG
#include "debugUtils.h"


// Pines 
const int ledPin = 13; 
const unsigned int sensorNotePin = A0;
const unsigned int sensorPacePin = A1;
const int calibrationSwitchPin = 12;
const unsigned int piezoPin = 8;

// Tiempo que demora la calibración de cada fotoresistor
unsigned long calibrationMillis = 5000;

// Factores mínimo y máximo para calcular el tiempo de cada nota.
int minNotePace = 500;
int maxNotePace = 5000;

// Índice máximo del arreglo de notas musical (a continuación) 
int maxNoteIndex = 11;

// Arreglo con las notas musicales y el tempo de cada una.
MusicNote notes[] = {
	{ NOTE_C4, 4 },
	{ NOTE_D4, 4 },
	{ NOTE_E4, 4 },
	{ NOTE_F4, 4 },
	{ NOTE_G4, 4 },
	{ NOTE_A4, 4 },
	{ NOTE_B4, 4 },
	{ NOTE_C5, 4 },
	{ NOTE_D5, 4 },
	{ NOTE_E5, 4 },
	{ NOTE_F5, 4 },
	{ NOTE_G5, 4 }
};

// Lee la configuración de la EPROM
Configuration getConfiguration() {
	static Configuration cfg;
	EEPROM.get(0, cfg);
	return cfg;
}

// Guarda la configuración en la EPROM
Configuration putConfiguration(Configuration cfg) {
	return EEPROM.put(0, cfg);
}

// Para mostrar los valores de configuración
void logConfiguration(Configuration cfg) {
	DEBUG_PRINTLN("------------");
	DEBUG_PRINTLN("Configuration");
	DEBUG_PRINT("Sensor Note: ");
	DEBUG_PRINT("MinValue: ");
	DEBUG_PRINT(cfg.sensorNoteCalibration.MinValue);
	DEBUG_PRINT(" / MaxValue:");
	DEBUG_PRINTLN(cfg.sensorNoteCalibration.MaxValue);
	DEBUG_PRINT("Sensor Pace: ");
	DEBUG_PRINT("MinValue: ");
	DEBUG_PRINT(cfg.sensorPaceCalibration.MinValue);
	DEBUG_PRINT(" / MaxValue:");
	DEBUG_PRINTLN(cfg.sensorPaceCalibration.MaxValue);
	DEBUG_PRINTLN("------------");
}

// Calibra un sensor analógico
// Sensa un pin analógico durante un periódo de tiempo y devuelve los valores mínimo y máximo.
Calibration calibrateAnalogSensor(unsigned int pin, unsigned long calibrationMillis) {
	unsigned long startMillis = millis();
	Calibration calibration = { 1024,0 };
	while (millis() < startMillis + calibrationMillis) {
		int sensorValue = analogRead(pin);
		if (sensorValue > calibration.MaxValue) {
			calibration.MaxValue = sensorValue;
		}
		if (sensorValue < calibration.MinValue) {
			calibration.MinValue = sensorValue;
		}
	}

	DEBUG_PRINT("Analog sensor pin: ");
	DEBUG_PRINT(pin);
	DEBUG_PRINTLN();
	DEBUG_PRINT("   MinValue: ");
	DEBUG_PRINTLN(calibration.MinValue);
	DEBUG_PRINT("   MaxValue:");
	DEBUG_PRINTLN(calibration.MaxValue);

	return calibration;
}

// Calibra los dos sensores de luz
Configuration calibrateSensors(Configuration cfg) {
	// calibra el sensor para las notas
	digitalWrite(ledPin, HIGH);
	cfg.sensorNoteCalibration = calibrateAnalogSensor(sensorNotePin, calibrationMillis);
	digitalWrite(ledPin, LOW);
	// espera de 3 segundos entre calibraciones
	delay(3000);
	// calibra el sensor para el tempo
	digitalWrite(ledPin, HIGH);
	cfg.sensorPaceCalibration = calibrateAnalogSensor(sensorPacePin, calibrationMillis);
	digitalWrite(ledPin, LOW);
	return cfg;
}

// Calibra los dos sensores y guarda la configuración en EPROM
Configuration calibrateSensorsAndSaveConfiguration(Configuration cfg) {
	// Calibrar los dos fotosensores
	cfg = calibrateSensors(cfg);
	logConfiguration(cfg);
	// Guardando configuración en EPROM
	putConfiguration(cfg);
	logConfiguration(cfg);
	return cfg;
}

// Toca una nota musical
// See http://garagelab.com/profiles/blogs/how-to-use-tone-function-arduino-playing-the-james-bond-theme
void playNote(int thisNote, int pace) {
	int noteDuration = pace / notes[thisNote].Duration;
	tone(piezoPin, notes[thisNote].Frequency, noteDuration);
	int pauseBetweenNotes = noteDuration * 1.30;
	delay(pauseBetweenNotes);
}

// Lee un sensor analógico y devuelve el índice correspondiente del arreglo de notas musicales.
// Utiliza la correspondencia entre los valores mínimo y máximo de calibración con 0 y el valor máximo 
// del índice del arrgelo de notas musicales.   
int getNoteIndexFromSensor(unsigned int pin, Calibration sensorCalibration, int maxNoteIndexIndex) {
	int sensorValue = analogRead(pin);
	DEBUG_PRINT("Note sensor value: ");
	DEBUG_PRINTLN(sensorValue);
	int noteIndex = constrain(map(sensorValue, sensorCalibration.MinValue, sensorCalibration.MaxValue, 0, maxNoteIndexIndex), 0, maxNoteIndexIndex);
	DEBUG_PRINT("noteIndex: ");
	DEBUG_PRINTLN(noteIndex);
	return noteIndex;
}

// Lee un sensor analógico y devuelve un valor de tempo.
// Utiliza la correspondencia entre los valores mínimo y máximo de calibración con los valores mínimo y 
// máximo del tempo.
int getNotePaceFromSensor(unsigned int pin, Calibration sensorCalibration, int minPace, int maxPace) {
	int sensorValue = analogRead(pin);
	DEBUG_PRINT("Pace sensor value: ");
	DEBUG_PRINTLN(sensorValue);
	int pace = constrain(map(sensorValue, sensorCalibration.MinValue, sensorCalibration.MaxValue, minPace, maxPace), minPace, maxPace);
	DEBUG_PRINT("Pace: ");
	DEBUG_PRINTLN(pace);
	return pace;
}

// Variable global para guardar la configuración
Configuration cfg;

void setup() {
	Serial.begin(9600);
	pinMode(ledPin, OUTPUT);
	pinMode(calibrationSwitchPin, INPUT);

	// Leyendo configuración de EPROM
	cfg = getConfiguration();
	logConfiguration(cfg);

	// Si no se ha calibrado nunca
	if (cfg.sensorNoteCalibration.MaxValue == 0 || cfg.sensorPaceCalibration.MaxValue == 0) {
		// Calibra los dos fotosensores y guarda la configuración en EPROM
		cfg = calibrateSensorsAndSaveConfiguration(cfg);
	}
}

void loop() {
	// Se ha pulsado el pushbutton de calibración?
	int calibrationSwitchValue = digitalRead(calibrationSwitchPin);
	DEBUG_PRINT("calibrationSwitchValue: ");
	DEBUG_PRINTLN(calibrationSwitchValue);
	if (calibrationSwitchValue == HIGH) {
		// Calibra los dos fotosensores y guarda la configuración en EPROM
		cfg = calibrateSensorsAndSaveConfiguration(cfg);
	}

	// Leyendo fotoresistor de nota musical  
	int noteIndex = getNoteIndexFromSensor(sensorNotePin, cfg.sensorNoteCalibration, maxNoteIndex);
	// Leyendo fotorresistor de tempo
	int notePace = getNotePaceFromSensor(sensorPacePin, cfg.sensorPaceCalibration, minNotePace, maxNotePace);
	// Tocando la nota
	playNote(noteIndex, notePace);
}
