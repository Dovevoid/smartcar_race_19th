/*
 * motor.h
 *
 *  Created on: 2024��8��1��
 *      Author: Izzo
 */

#ifndef _FINDLINEB_H
#define _FINDLINEB_H

//�궨��
#define image_h 120//ͼ��߶�
#define image_w 160//ͼ����

#define white_pixel 255
#define black_pixel 0

#define bin_jump_num    1//�����ĵ���
#define border_max  image_w-2 //�߽����ֵ
#define border_min  1   //�߽���Сֵ

extern uint8 mt9v03x_image_new[image_h][image_w];//ͼ������
extern int16 qianzhan , qianzhan_init ;
extern uint8 stop_flag;

extern float image_k_theta[2];

typedef struct
{

    int16 midline[120];

    int16 leftline[120];
    int16 rightline[120];
    int16 leftlineflag[120];
    int16 rightlineflag[120];

    int16 newleftline[120];
    int16 newrightline[120];

    int16 leftstartpoint;
    int16 rightstartpoint;
    int16 endline;
    int8 loseflag;
    int16 upline[160];
    int16 uplineflag[160];
    float err[60];
    float kp1;
    float kd1;
    float U;
    float U1;
    float llasterr;
    int16 r_lose;
    int16 l_lose;

} findline_TypeDef;

typedef struct
{
  int16 leftup[2];
  int16 rightup[2];
  int16 leftdown[2];
  int16 rightdown[2];
  int16 counter[1];
  float lenth;
}fp_TypeDef;

extern fp_TypeDef Findp;

extern findline_TypeDef Findline;


int my_abs(int value);
int16 limit1(int16 x, int16 y);
uint8 get_start_point(uint8 start_row);

void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest);

void get_left(uint16 total_L);
void get_right(uint16 total_R);
void image_filter(uint8(*mt9v03x_image_new)[image_w]);//��̬ѧ�˲�������˵�������ͺ͸�ʴ��˼��
void image_draw_rectan(uint8(*image)[image_w]);

void findlineb(void); //ֱ�����жϻ�ѭ������ô˳���Ϳ���ѭ��ִ����
float Slope_Calculate(uint8 begin, uint8 end, uint8 *border);
void calculate_s_i(uint8 start, uint8 end, uint8 *border, float *slope_rate, float *intercept);

void findline(void);
extern uint8 widthlose;
void dynamic_qianzhan(void);
void caculate_err(void);
void EmergencyStop(void);
#endif /*_IMAGE_H*/
