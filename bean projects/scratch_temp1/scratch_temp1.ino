void setup()
{
  // We don't need to set anything up
}

void loop()
{
    int8_t temperature = Bean.getTemperature();
    Bean.setScratchNumber(1, temperature);
    Bean.sleep(2000);  
}
