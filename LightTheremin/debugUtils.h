/*
debugUtils.h - Debugging macros.

creado 26 Oct 2015
por Nelson López Centeno (@nlopezcenteno)

Basado en:
http://forum.arduino.cc/index.php?topic=46900.0

*/

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#ifdef DEBUG
#define DEBUG_PRINT(str)	Serial.print(str);
#define DEBUG_PRINTLN(str)    Serial.println(str);
#else
#define DEBUG_PRINT(str)
#define DEBUG_PRINTLN(str)
#endif

#endif
