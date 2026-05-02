/*
 * CAN.c
 *
 *  Created on: Feb 21, 2026
 *      Author: becca
 */


#include "CAN.h"

//packet structure = Priority bit + (7 bit (Board + Device)) + Domain + DLC + (1 bit ACK + 7 bit command ID + 7 byte data)
//       									^ UUID                                      ^data
int ProcessCAN(CANPacket_t* receivedPacket, CANPacket_t* packetToSend) {

	const uint8_t* data = CANGetDataConst(receivedPacket);
	uint8_t command = (*data & 0x7F); //removing first bit ACK

	uint16_t  header = CANGetPacketHeader(receivedPacket);

	uint8_t UUID = (header & 0x0380)>>7; //get upper 3 bit UUID (potentially scuffed math)

	Print("\r\n\r\nBeginning of BLDC_CAN\r\n\r\n");

    switch(UUID){
        // Universal Packet
       /*
        case(CAN_COMMAND_ID_E_STOP):
            Print("\r\n\r\nSTOP\r\n\r\n");
            err = ESTOP_ERR_GENERAL;
            break;
       */
        case(0x03): //BLDC UUID (maybe remove if issues with CAN)
            switch(command) //check command ID
            {
                case(CAN_COMMAND_ID__BLDC_INPUT_MODE): //
					//open/closed loop?
                    break;
                case(CAN_COMMAND_ID__BLDC_INPUT_POSITION)://

                    break;
                case(CAN_COMMAND_ID__BLDC_INPUT_VELOCITY)://
					//set PID speed
                    break;
                case(CAN_COMMAND_ID__BLDC_DIRECT_WRITE):

                    break;
                case(CAN_COMMAND_ID__BLDC_DIRECT_READ):

                    break;
                case(CAN_COMMAND_ID__BLDC_DIRECT_READ_RESULT):

                    break;
                case(CAN_COMMAND_ID__BLDC_ENCODER_ESTIMATE_GET):

                    break;
                case(CAN_COMMAND_ID__BLDC_ENCODER_ESTIMATE):
					//return speed
					return Hall_GetSpeed();
                    break;
                case(CAN_COMMAND_ID__BLDC_AXIS_STATE):
					//enable/disable motor
                    break;
                default:
                    Print("Default");
                    //err = ERROR_INVALID_TTC;
                    break;
            }


        default: //recieved Packet with non-valid ID
            //return ERROR_INVALID_PACKET;
    }

    //return err;
}





