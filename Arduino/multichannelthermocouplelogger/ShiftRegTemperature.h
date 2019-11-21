#define maxSO 0
#define maxSCK 8
#define DSO 18
#define SCKSHIFT 14
#define N_CHANNELS 10

void init_shift_reg(int dso, int sckshift);
void step_shift_reg(int dso, int sckshfit);
void shift_reg_temp(int dso, int sckshift, int maxso, int maxsck, int result[N_CHANNELS]);
double readCelsius(int encoding, double* result);
double readFahrenheit(double cels);
//int internalTemp(int encoding, double* result)