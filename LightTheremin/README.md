# LightTheremin #

Esta es mi propia versi�n del ejemplo LightThermin del Arduino starter kit.

Utiliza un fotorresitor para las notas y otro fotorresistor para el ritmo.
Cada fotorresistor se calibra de forma independiente y la calibraci�n se guarda en la EPROM.
Un "pushbutton" permite volver a calibrar: 5 segundos de calibraci�n para las notas, una pausa de
de 3 segundos y 5 segundos de calibraci�n para el ritmo. El led 13 se enciende cuando se est� 
calibrando cada fotorresitor.  

### Circuito: ###
* fotorresistor en pin A0 (notas)
* fotorresistor en pin A1 (ritmo)
* pushbutton para calibraci�n en pin 12
* piezo en pin 8
