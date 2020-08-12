#include <LiquidCrystal.h>
LiquidCrystal lcd(1, 2, 5, 6, 8,9);

/*For identifying the wave type, derivative method was used. 
 * Depending on the derivative of square, triangle, chain saw and sine wave the type was determined.
*/

//For taking the derivative 2 values were used.
float value_first = 0;
float value_second = 0;
float derivative = 0;

//Count of the zeros and negative values of the derivatives.
int zero_count = 0;
int negative_count = 0; 
//Array for storing the derivative values.
float d_values[50];

//For the calculation of the frequency.
int high_time = 0;              
int low_time = 0;    
float period = 0;            
float frequency = 0;   


void setup() {
  
  lcd.begin(16, 2);
  lcd.print("FreqCounter:");
  
  //Schmit Trigger input
  pinMode(12,INPUT);
  
  //DAC input
  pinMode(A0, INPUT);
  delay(500);
}

void loop() {
  
    lcd.clear();
    //For finding the signals' frequency, Schmitt Trigger (SNx4HC14 Hex Schmitt-Trigger Inverter) was used. Every coming signal now is a square wave.  
    //The signals are an analog signals but using a digital pin for the analog signal, on and off part of the signals were easily tracked. 
    high_time = pulseIn(12, HIGH);      
    low_time = pulseIn(12, LOW);
    period = high_time + low_time;
    
    //It is divided by 1000000 to get Hertz (The time is in miliseconds).
    frequency = 1000000 / period;
    
    lcd.setCursor(0,0);
    lcd.print("Freq: ");
    lcd.print(frequency);//This method is for calculating the number of zeros inside the d_value array. 
    lcd.print("Hz");

    //This part is for identifiying the wave type.
    for (int i = 0; i < 50; i++){
        value_first = analogRead(A0);
        //time is 1 ms
        delay(1);
        value_second = analogRead(A0);
        derivative = (value_second - value_first) / ((1e-3));
        d_values[i] = derivative;
    }
    
    zero_count = returnZeroCount(d_values, 50);
    negative_count = returnNegativeCount(d_values, 50);
    
    //depending on the number of zeros and negative values, the waves are identified. 
    if (zero_count > 35){
        lcd.setCursor(0,1);
        lcd.print("Square");
    }else if (zero_count < 5 && negative_count < 5){
        lcd.setCursor(0,1);
        lcd.print("Saw Tooth");
    } else if (zero_count < 5 && negative_count < 21){
        lcd.setCursor(0,1);
        lcd.print("Triangle");
    } else if (zero_count < 5 && negative_count > 20){
        lcd.setCursor(0,1);
        lcd.print("Sine");
    }
    delay(500);
}

/*
 * This method is for calculating the number of zeros inside the d_value array. 
 */

int returnZeroCount(float arr[], int size_of_array){
  int count = 0;
   for (int i = 0; i < size_of_array; i++){
    if (arr[i] == 0){
      count++;
    }
  }
  return count;
}

/*
 * This method is for calculating the number of negative values inside the d_value array. 
 */
int returnNegativeCount(float arr[], int size_of_array){
  int count = 0;
   for (int i = 0; i < size_of_array; i++){
    if (arr[i] < 0){
      count++;
    }
  }
  return count;
}
