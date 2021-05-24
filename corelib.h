#include "PrintStream.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

tsPoint_t       _tsLCDPoints[3];
tsPoint_t       _tsTSPoints[3];
tsMatrix_t      _tsMatrix;

bool ota_enabled = false;

void brightness(int val) {
  tft.PWM1out(val);
}

void powermode(int mode) { //change cpu clock speed and tft brightness for battery saving.
  if (mode == LOW) {
    brightness(60);
    setCpuFrequencyMhz(80);
  }
  if (mode == HIGH) {
    brightness(255);
    setCpuFrequencyMhz(240);
  }
}

int calibrateTSPoint( tsPoint_t * displayPtr, tsPoint_t * screenPtr, tsMatrix_t * matrixPtr )
{
  int  retValue = 0 ;

  if ( matrixPtr->Divider != 0 )
  {
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) +
                      (matrixPtr->Bn * screenPtr->y) +
                      matrixPtr->Cn
                    ) / matrixPtr->Divider ;

    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) +
                      (matrixPtr->En * screenPtr->y) +
                      matrixPtr->Fn
                    ) / matrixPtr->Divider ;
  }
  else
  {
    return -1;
  }

  return ( retValue );
}


int setCalibrationMatrix( tsPoint_t * displayPtr, tsPoint_t * screenPtr, tsMatrix_t * matrixPtr)
{
  int  retValue = 0;

  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

  if ( matrixPtr->Divider == 0 )
  {
    retValue = -1 ;
  }
  else
  {
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;

    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;

    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;

    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;

  }

  return ( retValue ) ;
}


void tsCalibrate(void)
{
  tsPoint_t data;

  /* ----------------- First Dot ------------------ */
  // 10% over and 10% down
  _tsLCDPoints[0].x = tft.width() / 10;
  _tsLCDPoints[0].y = tft.height() / 10;
  _tsTSPoints[0].x = 151;
  _tsTSPoints[0].y = 186;
  Serial.print("Point 1 - LCD");
  Serial.print(" X: ");
  Serial.print(_tsLCDPoints[0].x);
  Serial.print(" Y: ");
  Serial.print(_tsLCDPoints[0].y);
  Serial.print(" TS X: ");
  Serial.print(_tsTSPoints[0].x);
  Serial.print(" Y: ");
  Serial.println(_tsTSPoints[0].y);
  delay(250);

  /* ---------------- Second Dot ------------------ */
  // 50% over and 90% down
  _tsLCDPoints[1].x = tft.width() / 2;
  _tsLCDPoints[1].y = tft.height() - tft.height() / 10;
  _tsTSPoints[1].x = 519;
  _tsTSPoints[1].y = 874;
  Serial.print("Point 2 - LCD");
  Serial.print(" X: ");
  Serial.print(_tsLCDPoints[1].x);
  Serial.print(" Y: ");
  Serial.print(_tsLCDPoints[1].y);
  Serial.print(" TS X: ");
  Serial.print(_tsTSPoints[1].x);
  Serial.print(" Y: ");
  Serial.println(_tsTSPoints[1].y);
  delay(250);

  /* ---------------- Third Dot ------------------- */
  // 90% over and 50% down
  _tsLCDPoints[2].x = tft.width() - tft.width() / 10;
  _tsLCDPoints[2].y = tft.height() / 2;
  _tsTSPoints[2].x = 888;
  _tsTSPoints[2].y = 525;
  Serial.print("Point 3 - LCD");
  Serial.print(" X: ");
  Serial.print(_tsLCDPoints[2].x);
  Serial.print(" Y: ");
  Serial.print(_tsLCDPoints[2].y);
  Serial.print(" TS X: ");
  Serial.print(_tsTSPoints[2].x);
  Serial.print(" Y: ");
  Serial.println(_tsTSPoints[2].y);
  delay(250);

  /* Clear the screen */
  tft.fillScreen(RA8875_WHITE);

  // Do matrix calculations for calibration and store to EEPROM
  setCalibrationMatrix(&_tsLCDPoints[0], &_tsTSPoints[0], &_tsMatrix);
}

tsPoint_t getCalibratedTouch() {

  uint16_t x, y;
  uint16_t d, m;
  tsPoint_t raw;
  tsPoint_t calibrated;

  tft.touchRead(&x, &y);
  delay(55);
  while (tft.touched())
  {
    tft.touchRead(&d, &m);
    delay(55);
  }

  /* Make sure this is really a touch event */

  //tft.touchRead(&x, &y);
  raw.x = x;
  raw.y = y;

  /* Calcuate the real X/Y position based on the calibration matrix */
  calibrateTSPoint(&calibrated, &raw, &_tsMatrix );

  calibrated.x = 800 - calibrated.x;
  calibrated.y = 480 - calibrated.y;

  return calibrated;


}
