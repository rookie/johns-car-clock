



  
  

double parseThermistor(double engineTempSensor) {

  //Read and convert for display  
  engineTempSensor = log(10000.0*((1024.0/engineTempSensor-1))); // for pull-up configuration
  engineTempSensor = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * engineTempSensor * engineTempSensor ))* engineTempSensor );
  engineTempSensor = engineTempSensor - 273.15;            // Convert Kelvin to Celcius
  engineTempSensor = (engineTempSensor * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
  
  return engineTempSensor;
}
