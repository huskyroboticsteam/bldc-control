/*
 * CAN.c
 *
 *  Created on: Feb 21, 2026
 *      Author: becca
 */



//Lowkey realized this is overkill for function of camera servo so ignore this for now and functionality moved instead to main.c
// - Becca

//--------------------------------------------------------------------------------------------
#include "main.h"
#include "CAN.h"

//packet structure = Priority bit + (7 bit (Board + Device)) + Domain + DLC + (1 bit ACK + 7 bit command ID + 7 byte data)
//       									^ UUID                                      ^data
int ProcessCAN(CANPacket_t* receivedPacket, CANPacket_t* packetToSend) {

	const uint8_t* data = CANGetDataConst(receivedPacket);
	uint8_t command = (*data & 0x7f); //removing first bit ACK

	uint16_t  header = CANGetPacketHeader(receivedPacket);

	uint8_t UUID = (header & 0x03F8)>>3; //get upper 7 bit UUID (potentially scuffed math)

	Print("\r\n\r\nBeginning of CAMERA_SERVO\r\n\r\n");

        if(UUID == CAN_UUID_TELEMETRY){
            switch(command) //check command ID
            {
                case(CAN_COMMAND_ID__STEPPER_DRIVE_REVS): {//idk what packets this motor takes in so decide once driver done
						CANPeripheralPacket_SetPWMDutyCycle_Decoded_t decode = CANPeripheralPacket_SetPWMDutyCycle_Decode(receivedPacket);
                		float duty_cycle = decode.dutyCycle;
                		//call function to change wtv needed for motor
                    break;
                } //just lmk which things you want to add to motor driver and i can add more case statements
                default:
                    Print("Default");
                    //err = ERROR_INVALID_TTC;
                    break;
            }

    }
    return 0;
    //return err;
}





