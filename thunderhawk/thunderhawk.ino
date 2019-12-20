#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
  public:

    // Member Variables:
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position

    uint32_t Color1, Color2, ColorActive;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    uint16_t type2active;

    uint16_t type2;
    uint16_t Offset;

    void (*OnComplete)();  // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
      : Adafruit_NeoPixel(pixels, pin, type)
    {
      OnComplete = callback;
    }

    // Update the pattern
    void Update(uint16_t type2)
    {
      if ((millis() - lastUpdate) > Interval) // time to update
      {
        lastUpdate = millis();

        ScannerUpdate(type2);

      }
    }

    // Increment the Index and reset at the end
    void Increment()
    {
      if (Direction == FORWARD)
      {
        Index++;
        if (Index >= TotalSteps)
        {
          Index = 0;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
      else // Direction == REVERSE
      {
        --Index;
        if (Index <= 0)
        {
          Index = TotalSteps - 1;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
    }

    // Reverse pattern direction
    void Reverse()
    {
      if (Direction == FORWARD)
      {
        Direction = REVERSE;
        Index = TotalSteps - 1;
      }
      else
      {
        Direction = FORWARD;
        Index = 0;
      }
    }


    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint32_t color2, uint8_t interval)
    {
      ActivePattern = SCANNER;
      Interval = interval;
      TotalSteps = (numPixels() - 1) + 10;
      Color1 = color1;
      Color2 = color2;
      ColorActive = color1;
      Index = 0;
      Offset = 0;
    }

    // Update the Scanner Pattern
    void ScannerUpdate(uint16_t type2active)
    {
      if (type2active == false) {
        ColorActive = Color1;
        Offset = 5;
        for (int i = 0; i < Offset; i++){
          setPixelColor(i, Color(0, 0, 0));
        }
      } else {
        ColorActive = Color2;
        Offset = 0;
      }

      for (int i = 0 + Offset; i < numPixels(); i++)
      {
        if (i+ Offset == Index+ Offset) // Scan Pixel to the right
        {
          setPixelColor(i, ColorActive);
        }

        else // Fading tail
        {
          setPixelColor(i, DimColor(getPixelColor(i)));
        }
      }
      show();
      Increment();
    }


    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
      // Shift R, G and B components one bit to the right
      uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
      return dimColor;
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, color);
      }
      show();
    }

    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
      return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
      return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
      return color & 0xFF;
    }


};

void StickComplete();

// Define some NeoPatterns for the two rings and the stick
//  as well as some completion routines
NeoPatterns Stick(10, 11, NEO_GRB + NEO_KHZ800, &StickComplete);

// Initialize everything and prepare to start
void setup()
{
 

  pinMode(8, INPUT_PULLUP);


  // Initialize all the pixelStrips

  Stick.begin();

  // Kick off a pattern

  Stick.Scanner(Stick.Color(200, 0, 100), Stick.Color(250, 0, 0), 55);
}

// Main loop
void loop()
{

  // Switch patterns on a button press:
  if (digitalRead(8) == LOW) // Button #1 pressed
  {

    // Set stick to all red
    Stick.Update(true);
  }

  else // Back to normal operation
  {
    // Restore all pattern parameters to normal values

    // And update tbe stick
    Stick.Update(false);
  }
}

//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------

// Stick Completion Callback
void StickComplete()
{
  // Random color change for next scan
  //    Stick.Color1 = Stick.Wheel(random(255));
}
