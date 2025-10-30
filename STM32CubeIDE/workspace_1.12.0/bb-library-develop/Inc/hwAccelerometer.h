#ifndef __HW_ACCELEROMETER_H__
#define __HW_ACCELEROMETER_H__

/**
 * Ajout NB 01/06/2019
 *
 * Ci-dessous, quelques prédicats
 *
 */

#include <stdint.h>

#define ACCELEROMETER_TOLERANCE 15 // en °

#define BLINKY_EST_DEBOUT(axeZ) ( (  ( axeZ > (90-ACCELEROMETER_TOLERANCE))  &&  ( axeZ < (90+ACCELEROMETER_TOLERANCE))   ) )

#define BLINKY_EST_COUCHE(axeZ) ( (  ( axeZ > (0-ACCELEROMETER_TOLERANCE))  &&  ( axeZ < (0+ACCELEROMETER_TOLERANCE))   ) )

#define BLINKY_EST_TETE_EN_BAS(axeZ) ( (  ( axeZ > (-90-ACCELEROMETER_TOLERANCE))  &&  ( axeZ < (-90+ACCELEROMETER_TOLERANCE))   ) )

#define ACCELEROMETER_XY_BLINKY_ANGLE 65 // en °

// Blincky couché sur sur face RIGHT :
#define BLINKY_IS_RIGHT_SIDE_ON_THE_FLOOR(axeX,axeY,axeZ) ( (                                                                             \
                                                                   ( axeX > (ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeX < (ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeY > (ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeY < (ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( BLINKY_EST_COUCHE(axeZ) )              ) )

// Blincky couché sur sur face FRONT  :
#define BLINKY_IS_FRONT_SIDE_ON_THE_FLOOR(axeX,axeY,axeZ) ( (                                                                             \
                                                                   ( axeX > (ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeX < (ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeY > (-ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeY < (-ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( BLINKY_EST_COUCHE(axeZ) )              ) )

// Blincky couché sur sur face BACK:
#define BLINKY_IS_BACK_SIDE_ON_THE_FLOOR(axeX,axeY,axeZ) ( (                                                                             \
                                                                   ( axeY > (ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeY < (ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeX > (-ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeX < (-ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( BLINKY_EST_COUCHE(axeZ) )              ) )

// Blincky couché sur sur face LEFT:
#define BLINKY_IS_LEFT_SIDE_ON_THE_FLOOR(axeX,axeY,axeZ) ( (                                                                             \
                                                                   ( axeX > (-ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeX < (-ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeY > (-ACCELEROMETER_XY_BLINKY_ANGLE-ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( axeY < (-ACCELEROMETER_XY_BLINKY_ANGLE+ACCELEROMETER_TOLERANCE))      \
                                                               &&  ( BLINKY_EST_COUCHE(axeZ) )              ) )

#define BLINKY_EST_TETE_EN_BAS_45DEG_FACE_L_PIVOT(axeX,axeY,axeZ)  ( (     ( axeX > (-45-ACCELEROMETER_TOLERANCE))      \
                                                                       &&  ( axeX < (-45+ACCELEROMETER_TOLERANCE))      \
                                                                       &&  ( axeY > (-45-ACCELEROMETER_TOLERANCE))      \
                                                                       &&  ( axeY < (-45+ACCELEROMETER_TOLERANCE))      \
                                                                       &&  ( axeZ > (-60-ACCELEROMETER_TOLERANCE))      \
                                                                       &&  ( axeZ < (-60+ACCELEROMETER_TOLERANCE))       ) )



/**
 * fin Ajout NB 01/06/2019
 */


//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
typedef struct pAccel
{
  uint8_t ucX;
  uint8_t ucY;
  uint8_t ucZ;
  uint8_t ucStatus;

  float angleX; // Ajout NB 01/06/2019
  float angleY; // Ajout NB 01/06/2019
  float angleZ; // Ajout NB 01/06/2019

} tAccel;


//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------
int newHWAccelData(void);
void getHWAccel(tAccel *ptAccelData);
void initHWAccel(void);

void refreshXYZ(tAccel * ptAccelData); // Ajout NB 01/06/2019

void setAccelRegister(uint8_t ucOne, uint8_t ucTwo);


#endif
