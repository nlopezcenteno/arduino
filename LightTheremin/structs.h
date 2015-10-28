/*
Estructuras usadas en LightTheremin.ino

creado 26 Oct 2015
por Nelson López Centeno (@nlopezcenteno)

*/

struct MusicNote {
	int Frequency;
	int Duration;
};

struct Calibration {
	int MinValue;
	int MaxValue;
};

struct Configuration {
	Calibration sensorNoteCalibration;
	Calibration sensorPaceCalibration;
};