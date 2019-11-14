#define maxSO 0
#define maxSCK 8
#define DSO 18
#define SCKSHIFT 14
#define N_CHANNELS 10

void init_shift_reg(int dso, int sckshift);
void step_shift_reg(int dso, int sckshfit);
int* shift_reg_temp(int dso, int sckshift, int maxso, int maxsck);

void setup() {
  
  Serial.begin(9600);
  init_shift_reg(DSO, SCKSHIFT);
  pinMode(maxSO,INPUT);
  pinMode(maxSCK, OUTPUT);

}

void loop() {  
  digitalWriteFast(DSO, LOW);
  step_shift_reg(DSO, SCKSHIFT);

  int tempData[N_CHANNELS] = {shift_reg_temp(DSO, SCKSHIFT, maxSO, maxSCK)};

  for (int i = 0; i < N_CHANNELS; i++){
    Serial.println(tempData[i]);
  }


}

void init_shift_reg(int dso, int sckshift){
  int i;
  pinMode(dso, OUTPUT);
  pinMode(sckshift, OUTPUT);

  digitalWriteFast(dso, HIGH);

  for( i = 0; i < 16; i++){
    digitalWriteFast(sckshift, HIGH);
    digitalWriteFast(sckshift, LOW);
  };

  digitalWriteFast(dso, LOW);
  
};


void step_shift_reg(int dso, int sckshift){
  digitalWriteFast(sckshift, HIGH);
  digitalWriteFast(sckshift, LOW);

  digitalWriteFast(dso, HIGH);
  
};


int* shift_reg_temp(int dso, int sckshift, int maxso, int maxsck){
  int result[N_CHANNELS];

  for (int i = 0; i < N_CHANNELS; i++){
    //Serial.println();
    //Serial.println(i);
    uint32_t tempBit = 0;
    step_shift_reg(dso, sckshift);
    
    for (int j = 0; j < 32; j++){
      digitalWriteFast(maxsck, HIGH);
      digitalWriteFast(maxsck, LOW);
      tempBit <<= 1;
      if (digitalReadFast(maxso)) {
        tempBit |= 1;
      }
      //Serial.print(digitalReadFast(maxso));
    }
    result[i] = tempBit;
    //Serial.println(i);
    //Serial.println(tempBit);
  }

  return result;

}
