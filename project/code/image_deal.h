/*
 * Image_deal.h
 *
 *  Created on: 2024Äê2ÔÂ27ÈÕ
 *      Author: Dove
 */

#ifndef IMAGE_DEAL_H_
#define IMAGE_DEAL_H_

extern uint8 mt9v03x_image_new[MT9V03X_H][MT9V03X_W];
extern uint8  threshold;

uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row);
uint8 GuDiThreshold(uint16 width, uint16 height);
uint8 OneDimensionalThreshold(uint16 width, uint16 height);
void Binarazation(void);
void Bin_Image_Filter (void);

void Image_deal(uint8 MODE);

#endif /* IMAGE_DEAL_H_ */
