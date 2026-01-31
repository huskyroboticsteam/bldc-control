//used http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/ as reference
//imports and stuff


int32_t kPosition = 0, kIntegral = 0, kDerivative = 0;

uint32_t maxSpeed = 32768; //taken from last year

int integral = 0;
int lastError = 0;

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



}

int32_t positionPID(int32 targetDeg){
    if(!PIDIsEnabled()){
        return(0);
    }

    int32 current = GetPositionmDeg();
    int32 error = targetDeg - current;




    lastError = error; //update error

	return PWMout;

}

