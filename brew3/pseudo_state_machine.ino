   /* PSEUDO CODE STATE MACHINE

  
  STANDBY
    Actuate Encoder1 = change settemp
    Long press = N/A
    Short press = next state
  WATER HEATING
    Actuate Encoder1 = change settemp
    Long press = Start temp reg, Encoder1 is now off
    Long press again = stop temp reg, Encoder1 is no on
    Short press = next state
  PAUSE
    Former "mash-in"
    Everything is off. Same as standby
  MASHING
    Pump is now on by default at default mashing pwm
    Long press = Pump power can now be directly set via Encoder2 FUCK THIS
    Long press again = pump power can no longer be set.
    Short press = next state
  BOIL
    Actuate Encoder1 = changes power (heater is still off)
    Long press = start heater at specified power level. Encoder1 now changes power directly
    Long press again = stop heater
    Short press = go to Standby
  
  ---------
  
  PUMP LOOP
    Actuate Encoder2 = changes pump power
    Short press = pump starts at pump power
    Short press again = pump stops
    Long press = N/A (unless in mashing state)
  
   */


  /*
  20x4 LCD format
  --------------------
  State: Standby
  T1: 20.7C   T2:21.1C
  Settemp: 67C
  Pump: 0%
  --------------------

  --------------------
  State: Standby
  T1: ERROR   T2:21.1C
  Settemp: 67C
  Pump: 0%
  --------------------

  --------------------
  State: Standby
  T1: 20.7C   T2:21.1C
  Power: 60%
  Pump: default
  --------------------

  */
