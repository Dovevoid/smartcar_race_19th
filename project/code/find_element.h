/*
 * find_element.h
 *
 *  Created on: 2024��8��2��
 *      Author: Izzo
 */

#ifndef FIND_ELEMENT_H_
#define FIND_ELEMENT_H_


extern int16 width[120];

void find_element();

void find_guai(void);
extern uint8 rightguai_row_down;
extern uint8 leftguai_row_down;
extern uint8 rightguai_column_down;
extern uint8 leftguai_column_down;
extern uint8 rightguai_row_up;
extern uint8 leftguai_row_up;
extern uint8 rightguai_column_up;
extern uint8 leftguai_column_up;
extern uint8 leftguaiflag_down;
extern uint8 leftguaiflag_up;
extern uint8 rightguaiflag_down;
extern uint8 rightguaiflag_up;

void Lcrossing_judge(void);
void Lcrossing(void);
void Rcrossing_judge(void);
void Rcrossing(void);
void Mcrossing_judge(void);
void Mcrossing(void);
extern uint8 Lcrossing_flag;
extern uint8 Rcrossing_flag;
extern uint8 Mcrossing_flag;

void huandao_judge();
void rightturn_s();
void leftturn_s();
void rightgo();
void leftgo();
uint8 youhuandao_judge(int16 row);
uint8 zuohuandao_judge(int16 row);
extern uint8 leftturncounter;
extern uint8 rightturncounter;
extern uint8 zuohuandao_flag;
extern uint8 youhuandao_flag;

void luzhang_judge(void);
void luzhang(void);
uint8 youluzhang_judge(int16 row);
uint8 zuoluzhang_judge(int16 row);
extern uint8 zuoluzhang_flag;
extern uint8 youluzhang_flag;
extern uint8 right_lucounter;
extern uint8 left_lucounter;

void bm_judge(void);
extern uint8 bm_judge_flag;
void bmstop(void);

void start(void);
extern float start_distance;
extern uint8 start_flag;

extern uint8 element_mode ;//������������,���������ϰ� 0�� 1����***
extern float luzhangpoint;          //�������ϵ��***
extern float luzhangangel;          //������ؽǶ�***
extern float huiangel;               //�ع������Ƕ�***
extern float huipoint;             //������ʶ��·�ϵ���ȫ����·��״̬�ľ����������ֹ·�Ϻ����������������ܲ�������***

extern uint8 straight_flag ;
extern float straight_speed;
void speed_decision();
extern uint8  is_open_upspeed ;

#endif /* FIND_ELEMENT_H_ */
