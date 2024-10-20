#ifndef BUTTONS_CUST
#define BUTTONS_CUST

unsigned char* applyGrayscaleFormula1(unsigned char* data, int width, int height, int channels);
unsigned char* applyGrayscaleFormula2(unsigned char* data, int width, int height, int channels);
unsigned char* applyGrayDifference(unsigned char* data1, unsigned char* data2, int width, int height, int channels);

#endif // !BUTTONS_CUST

