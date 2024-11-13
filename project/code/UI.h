/*
 * UI.h
 *
 *  Created on: 2024Äê2ÔÂ28ÈÕ
 *      Author: Dove
 */

#ifndef UI_H_
#define UI_H_

typedef struct
{
    uint8 page;
    uint8 cursor;
} UI_CLASS;

void UI_DisplayPages(char strings[10][17]);
void UI_DisplayMenu(void);
void keyScan(void);
void img_show(void);
void num_add(float* num, float step);
void num_sub(float* num, float step);

#endif /* UI_H_ */
