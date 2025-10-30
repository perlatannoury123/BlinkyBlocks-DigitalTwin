//------------------------------------------------------------------------------
// include
//------------------------------------------------------------------------------
#include <hwLED.h>
#include <abstraction.h>
//------------------------------------------------------------------------------
// define
//------------------------------------------------------------------------------

extern I2C_HandleTypeDef hi2c1;


//------------------------------------------------------------------------------
// void setHWLED(unsigned char ucRedPower, unsigned char ucGreenPower, unsigned char ucBluePower)
// updated by bpiranda, 2018-06-05
//------------------------------------------------------------------------------
void set_RGB(uint8_t red, uint8_t green, uint8_t blue) {
    static uint8_t ucPreviousRedPower = 101; // impossible value to start with a new color
    static uint8_t ucPreviousGreenPower = 101;
    static uint8_t ucPreviousBluePower = 101;
    static uint8_t ucPreviousReg = 0xFF; // impossible value to turn ON/OFF on start
    uint8_t tab_uc[2] = {0,0};
    uint8_t ucEnReg=(ucPreviousReg==0xFF)?0x3F:ucPreviousReg;


    // Maximum Control
    if (red > 100) red = 100;
    if (green > 100) green = 100;
    if (blue > 100) blue = 100;

//---------------------------------------------------------------------------------------------------------------------
//ATTENTION LE MODE 0 -> ROUGE = 1 BLEU = 0 VERT = 2
//Inversement des bits par rapport au schéma les 2 bits de poids faibles seront donc pour le bleu et non le rouge !!
//---------------------------------------------------------------------------------------------------------------------
    // RED
    if (red != ucPreviousRedPower) {
        ucPreviousRedPower=red;
        if (red==0) {
          ucEnReg &= 0x33;
        } else {
            ucEnReg |= 0x0C;
            // register adress for RED component
            tab_uc[0] = 1;
            tab_uc[1] = (uint8_t)(((int)(red-1) * MAX_CURRENT)/99);
            HAL_I2C_Master_Transmit(&hi2c1, LED_ADDR, &tab_uc[0], sizeof(tab_uc), 500);
        }
    }
    // GREEN, warning C1 && C2 connectors
    if (green != ucPreviousGreenPower) {
        ucPreviousGreenPower=green;
        if (green==0) {
            ucEnReg &= 0x0F;
        } else {
            ucEnReg |= 0x30;
            // register adress for GREEN component
            tab_uc[0] = 2; // ports C
            tab_uc[1] = (uint8_t)(((int)(green-1) * MAX_CURRENT)/99);
            HAL_I2C_Master_Transmit(&hi2c1, LED_ADDR, &tab_uc[0], sizeof(tab_uc), 500);
        }
    }
    // BLUE, warning B1 && B2 connectors
    if (blue != ucPreviousBluePower) {
        ucPreviousBluePower = blue;
        if (blue==0) {
            ucEnReg &= 0x3C;
        } else {
            ucEnReg |= 0x03;
            // register adress for BLUE component
            tab_uc[0] = 0; // ports C
            tab_uc[1] = (uint8_t)(((int)(blue-1) * MAX_CURRENT)/99);
            HAL_I2C_Master_Transmit(&hi2c1, LED_ADDR, &tab_uc[0], sizeof(tab_uc), 500);
        }
    }
    // Turn ON/OFF LEDs
    if (ucEnReg!=ucPreviousReg) {
        ucPreviousReg = ucEnReg;
        tab_uc[0] = 3; // ports ON/OFF
        tab_uc[1] = ucEnReg;
        HAL_I2C_Master_Transmit(&hi2c1, LED_ADDR, &tab_uc[0], sizeof(tab_uc), 500);
    }
}

