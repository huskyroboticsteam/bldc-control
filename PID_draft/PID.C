//used http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/ as reference
//imports and stuff

//system assumes consistent intervals (discrete-time PID)

//unsure if we need just speed control (using hall sensor) or speed and position (using encoder?)
int kPosition = 0, kIntegral = 0, kDerivative = 0;

int maxSpeed = 32768; //taken from last year

int integral = 0;
int lastError = 0;

int maxOut;
int minOut;


void ClearPIDProgress() {
    integral = 0;
    lastError = 0;
}

void DisablePID() {
    enabledPID = 0;
}

void EnablePID() {
    enabledPID = 1;
}

uint8_t PIDIsEnabled() {
    return(enabledPID);
}

void setPosition(){
//using PID set position... unsure on where position data is coming from so leave blank for now

}

void setK_Values(int32_t kP, int32_t kI, int32_t kD){
	kPosition = kP;
	kIntegral = kI;
	kDerivative = kD;
}

void setMaxMin(int32_t max, int32_t min){
	maxOut = max;
	minOut = min;
}

int32_t PID(int32_t target){
    if(!PIDIsEnabled()){
        return(0);
    }

    int current = GetHallSensor(); //from hall sensor/wtv get some sort of data
    int error = target - current;

    integral = integral + error;

    if(integral > outMax){
    	integral = outMax;
    }else if(integral < outMin) {
    	integral = outMin;
    }

    int derivative = error - lastError;

    int PWMOut = error*kPosition/10 + integral*kIntegral/10 + derivative*kDerivative/10;

    if(PWMOut > outMax) {
    	PWMOut = outMax;
    }else if(Output < outMin) {
    	PWMOut = outMin;
    }

    lastError = error; //update error

	return PWMout;

}

