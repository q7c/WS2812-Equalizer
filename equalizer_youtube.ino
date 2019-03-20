#include <Adafruit_NeoPixel.h>


struct Point
{
  char x, y;
  char r,g,b;
  bool active;
};

struct TopPoint
{
  int position;
  int pushed;
};



#define ROWS         10

#define COLUMNS      7

#define DATA_PIN     8

#define STROBE_PIN   2

#define RESET_PIN    3

#define ANALOG_PIN   0

#define NUMPIXELS    ROWS * COLUMNS

Point matrix[ROWS][COLUMNS];

TopPoint arrayTop[COLUMNS];

int spectrumValue[7];

long int loopCounter = 0;


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, DATA_PIN, NEO_RGB + NEO_KHZ800);

void setup() 
{
    pixels.begin();
    pixels.show();

    pinMode      (STROBE_PIN, OUTPUT);
    pinMode      (RESET_PIN,  OUTPUT);
    pinMode      (ANALOG_PIN,    INPUT);
   
    digitalWrite (RESET_PIN,  LOW);
    digitalWrite (STROBE_PIN, LOW);
    delay        (1);
   
    digitalWrite (RESET_PIN,  HIGH);
    delay        (1);
    digitalWrite (RESET_PIN,  LOW);
    digitalWrite (STROBE_PIN, HIGH);
    delay        (1);

}

void loop() 
{
  loopCounter++;
  
  clearMatrix();
  
  digitalWrite(RESET_PIN, HIGH);
  delay(5);
  digitalWrite(RESET_PIN, LOW);

  for(int i=0; i < 7; i++)
  {
    digitalWrite(STROBE_PIN, LOW);
    delay(10);
    spectrumValue[i] = analogRead(ANALOG_PIN);
    if(spectrumValue[i] < 100)spectrumValue[i] = 0;
    digitalWrite(STROBE_PIN, HIGH);

    spectrumValue[i] = constrain(spectrumValue[i], 0, 1023);
    spectrumValue[i] = map(spectrumValue[i], 0, 1023, 0, ROWS);
  }


  for(int j = 0; j < COLUMNS; j++)
  {
      for(int i = 0; i < spectrumValue[j]; i++)
      {
          matrix[i][COLUMNS - 1 - j].active = true;
          matrix[i][COLUMNS - 1 - j].r = 0; matrix[i][COLUMNS - 1 - j].g = 0; matrix[i][COLUMNS - 1 - j].b = 254;
      }
  


  if(spectrumValue[j] - 1 > arrayTop[j].position)
  {
    matrix[spectrumValue[j] - 1][COLUMNS - 1 - j].r = 254; matrix[spectrumValue[j] - 1][COLUMNS - 1 - j].g = 0; matrix[spectrumValue[j] - 1][COLUMNS - 1 - j].b = 0;
    arrayTop[j].position = spectrumValue[j] - 1;
    arrayTop[j].pushed = 5;
  }
  else
  {
      matrix[arrayTop[j].position ][COLUMNS - 1 - j].active = true;
      matrix[arrayTop[j].position][COLUMNS - 1 - j].r = 254; matrix[arrayTop[j].position][COLUMNS - 1 - j].g = 0; matrix[arrayTop[j].position][COLUMNS - 1 - j].b = 0;
  }


  }
  
  flushMatrix();

  if(loopCounter % 2 ==0)topSinking();
}



void topSinking()
{
  for(int j = 0; j < ROWS; j++)
  {
      if(arrayTop[j].position > 0 && arrayTop[j].pushed <= 0) arrayTop[j].position--;
      else if(arrayTop[j].pushed > 0) arrayTop[j].pushed--;  
  }
  
}

  void clearMatrix()
{
  for(int i = 0; i < ROWS; i++)
  {
    for(int j = 0; j < COLUMNS; j++)
    {
      matrix[i][j].active = false;
    }
    
  }
}


// xxxxxxxxxxxxxx
//            <--
// xxxxxxxxxxxxxx
// --> 
// xxxxxxxxxxxxxx
//            <--
// xxxxxxxxxxxxxx
// -->    
// xxxxxxxxxxxxxx    
//            <--

void flushMatrix()
{
  for(int j = 0; j < COLUMNS; j++)
  {

    if( j % 2 != 0)
    {
      for(int i = 0; i < ROWS; i++)
      {
        if(matrix[ROWS - 1 - i][j].active)
        {
            pixels.setPixelColor(j * ROWS + i, pixels.Color(matrix[ROWS - 1 - i][j].r, matrix[ROWS - 1 - i][j].g, matrix[ROWS - 1 - i][j].b));
          
        }
        else
        {
            pixels.setPixelColor( j * ROWS + i, 0, 0, 0);  
        }
      
      }
    }
    else
    {
      for(int i = 0; i < ROWS; i++)
      {
        if(matrix[i][j].active)
        {
            pixels.setPixelColor(j * ROWS + i, pixels.Color(matrix[i][j].r, matrix[i][j].g, matrix[i][j].b));
          
        }
        else
        {
            pixels.setPixelColor( j * ROWS + i, 0, 0, 0);  
        }
      }      
    } 
  }


  pixels.show();
}

