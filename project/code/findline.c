#include "zf_common_headfile.h"
#include "headfile.h"

float lastError;//ƫ��
uint8 stop_flag;//�����ź�
int16 lose_line = 0 ;

findline_TypeDef Findline = {0};
fp_TypeDef Findp= {0};
int16 qianzhan_init = 72;//70
int16 qianzhan = 72;//ǰհ�޸�

float lastError;
float chuerr=21.25;  //����������***
float huierr=18;     //����������***

uint8 widthlose=0;

/*
�������ƣ�int my_abs(int value)
����˵���������ֵ
����˵����
�������أ�����ֵ
�޸�ʱ�䣺2022��9��8��
��    ע��
example��  my_abs( x)��
 */
int my_abs(int value)
{
if(value>=0) return value;
else return -value;
}

int16 limit_a_b(int16 x, int a, int b)
{
    if(x<a) x = a;
    if(x>b) x = b;
    return x;
}

/*
�������ƣ�int16 limit(int16 x, int16 y)
����˵������x,y�е���Сֵ
����˵����
�������أ�������ֵ�е���Сֵ
�޸�ʱ�䣺2022��9��8��
��    ע��
example��  limit( x,  y)
 */
int16 limit1(int16 x, int16 y)
{
    if (x > y)             return y;
    else if (x < -y)       return -y;
    else                return x;
}



/*
�������ƣ�void get_start_point(uint8 start_row)
����˵����Ѱ�������߽�ı߽����Ϊ������ѭ������ʼ��
����˵����������������
�������أ���
�޸�ʱ�䣺2022��9��8��
��    ע��
example��  get_start_point(image_h-2)
 */
uint8 start_point_l[2] = { 0 };//�������x��yֵ
uint8 start_point_r[2] = { 0 };//�ұ�����x��yֵ
uint8 get_start_point(uint8 start_row)
{
    uint8 i = 0,l_found = 0,r_found = 0;
    //����
    start_point_l[0] = 0;//x
    start_point_l[1] = 0;//y

    start_point_r[0] = 0;//x
    start_point_r[1] = 0;//y

        //���м�����ߣ��������
    for (i = image_w / 2; i > border_min; i--)
    {
        start_point_l[0] = i;//x
        start_point_l[1] = start_row;//y
        if (mt9v03x_image_new[start_row][i] == 255 && mt9v03x_image_new[start_row][i - 1] == 0)
        {
            //printf("�ҵ�������image[%d][%d]\n", start_row,i);
            l_found = 1;
            break;
        }
    }

    for (i = image_w / 2; i < border_max; i++)
    {
        start_point_r[0] = i;//x
        start_point_r[1] = start_row;//y
        if (mt9v03x_image_new[start_row][i] == 255 && mt9v03x_image_new[start_row][i + 1] == 0)
        {
            //printf("�ҵ��ұ����image[%d][%d]\n",start_row, i);
            r_found = 1;
            break;
        }
    }

    if(l_found&&r_found)return 1;
    else {
        //printf("δ�ҵ����\n");
        return 0;
    }
}

/*
�������ƣ�void search_l_r(uint16 break_flag, uint8(*image)[image_w],uint16 *l_stastic, uint16 *r_stastic,
                            uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y,uint8*hightest)

����˵������������ʽ��ʼ���ұߵ�ĺ�������������е�࣬���õ�ʱ��Ҫ©�ˣ������������һ�������ꡣ
����˵����
break_flag_r            �������Ҫѭ���Ĵ���
(*image)[image_w]       ����Ҫ�����ҵ��ͼ�����飬�����Ƕ�ֵͼ,�����������Ƽ���
                       �ر�ע�⣬��Ҫ�ú궨��������Ϊ����������������ݿ����޷����ݹ���
*l_stastic              ��ͳ��������ݣ����������ʼ�����Ա����ź�ȡ��ѭ������
*r_stastic              ��ͳ���ұ����ݣ����������ʼ�����Ա����ź�ȡ��ѭ������
l_start_x               �������������
l_start_y               ��������������
r_start_x               ���ұ���������
r_start_y               ���ұ����������
hightest                ��ѭ���������õ�����߸߶�
�������أ���
�޸�ʱ�䣺2022��9��25��
��    ע��
example��
    search_l_r((uint16)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */
#define USE_num image_h*3   //�����ҵ�������Ա��������˵300�����ܷ��£�������Щ�������ȷʵ�Ѷ����ඨ����һ��

 //��ŵ��x��y����
uint16 points_l[(uint16)USE_num][2] = { {  0 } };//����
uint16 points_r[(uint16)USE_num][2] = { {  0 } };//����
uint16 dir_r[(uint16)USE_num] = { 0 };//�����洢�ұ���������
uint16 dir_l[(uint16)USE_num] = { 0 };//�����洢�����������
uint16 data_stastics_l = 0;//ͳ������ҵ���ĸ���
uint16 data_stastics_r = 0;//ͳ���ұ��ҵ���ĸ���
uint8 hightest = 0;//��ߵ�
void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
{

    uint8 i = 0, j = 0;

    //��߱���
    uint8 search_filds_l[8][2] = { {  0 } };
    uint8 index_l = 0;
    uint8 temp_l[8][2] = { {  0 } };
    uint8 center_point_l[2] = {  0 };
    uint16 l_data_statics;//ͳ�����
    //����˸�����
    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //�����˳ʱ��

    //�ұ߱���
    uint8 search_filds_r[8][2] = { {  0 } };
    uint8 center_point_r[2] = { 0 };//���������
    uint8 index_r = 0;//�����±�
    uint8 temp_r[8][2] = { {  0 } };
    uint16 r_data_statics;//ͳ���ұ�
    //����˸�����
    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //�������ʱ��

    l_data_statics = *l_stastic;//ͳ���ҵ��˶��ٸ��㣬��������ѵ�ȫ��������
    r_data_statics = *r_stastic;//ͳ���ҵ��˶��ٸ��㣬��������ѵ�ȫ��������

    //��һ�θ��������  ���ҵ������ֵ������
    center_point_l[0] = l_start_x;//x
    center_point_l[1] = l_start_y;//y
    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

        //��������ѭ��
    while (break_flag--)
    {

        //���
        for (i = 0; i < 8; i++)//����8F����
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
        }
        //�����������䵽�Ѿ��ҵ��ĵ���
        points_l[l_data_statics][0] = center_point_l[0];//x
        points_l[l_data_statics][1] = center_point_l[1];//y
        l_data_statics++;//������һ

        //�ұ�
        for (i = 0; i < 8; i++)//����8F����
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //�����������䵽�Ѿ��ҵ��ĵ���
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y

        index_l = 0;//�����㣬��ʹ��
        for (i = 0; i < 8; i++)
        {
            temp_l[i][0] = 0;//�����㣬��ʹ��
            temp_l[i][1] = 0;//�����㣬��ʹ��
        }

        //����ж�
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
            {
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
                dir_l[l_data_statics - 1] = (i);//��¼��������
            }

            if (index_l)
            {
                //���������
                center_point_l[0] = temp_l[0][0];//x
                center_point_l[1] = temp_l[0][1];//y
                for (j = 0; j < index_l; j++)
                {
                    if (center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];//x
                        center_point_l[1] = temp_l[j][1];//y
                    }
                }
            }

        }
        if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
            && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
            ||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
                && points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
        {
            //printf("���ν���ͬһ���㣬�˳�\n");
            break;
        }
        if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
            && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
            )
        {
            //printf("\n���������˳�\n");
            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//ȡ����ߵ�
            //printf("\n��y=%d���˳�\n",*hightest);
            break;
        }
        if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
        {
            //printf("\n�����߱��ұ߸��ˣ���ߵȴ��ұ�\n");
            continue;//�����߱��ұ߸��ˣ���ߵȴ��ұ�
        }
        if (dir_l[l_data_statics - 1] == 7
            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//��߱��ұ߸����Ѿ�����������
        {
            //printf("\n��߿�ʼ�����ˣ��ȴ��ұߣ��ȴ���... \n");
            center_point_l[0] = points_l[l_data_statics - 1][0];//x
            center_point_l[1] = points_l[l_data_statics - 1][1];//y
            l_data_statics--;
        }
        r_data_statics++;//������һ

        index_r = 0;//�����㣬��ʹ��
        for (i = 0; i < 8; i++)
        {
            temp_r[i][0] = 0;//�����㣬��ʹ��
            temp_r[i][1] = 0;//�����㣬��ʹ��
        }

        //�ұ��ж�
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
            {
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;//������һ
                dir_r[r_data_statics - 1] = (i);//��¼��������
                //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
            }
            if (index_r)
            {

                //���������
                center_point_r[0] = temp_r[0][0];//x
                center_point_r[1] = temp_r[0][1];//y
                for (j = 0; j < index_r; j++)
                {
                    if (center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];//x
                        center_point_r[1] = temp_r[j][1];//y
                    }
                }

            }
        }


    }


    //ȡ��ѭ������
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;

}
/*
�������ƣ�void get_left(uint16 total_L)
����˵�����Ӱ�����߽�����ȡ��Ҫ�ı���
����˵����
total_L ���ҵ��ĵ������
�������أ���
�޸�ʱ�䣺2022��9��25��
��    ע��
example�� get_left(data_stastics_l );
 */
void get_left(uint16 total_L)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    //��ʼ��
    for (i = 0;i<image_h;i++)
    {
        Findline.leftline[i] = border_min;
    }
    h = image_h - 2;
    //���
    for (j = 0; j < total_L; j++)
    {
        //printf("%d\n", j);
        if (points_l[j][1] == h)
        {
            Findline.leftline[h] = points_l[j][0]+1;
        }
        else continue; //ÿ��ֻȡһ���㣬û����һ�оͲ���¼
        h--;
        if (h == 0)
        {
            break;//�����һ���˳�
        }
    }
}
/*
�������ƣ�void get_right(uint16 total_R)
����˵�����Ӱ�����߽�����ȡ��Ҫ�ı���
����˵����
total_R  ���ҵ��ĵ������
�������أ���
�޸�ʱ�䣺2022��9��25��
��    ע��
example��get_right(data_stastics_r);
 */
void get_right(uint16 total_R)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    for (i = 0; i < image_h; i++)
    {
        Findline.rightline[i] = border_max;//�ұ��߳�ʼ���ŵ����ұߣ�����߷ŵ�����ߣ�����������պ�����������߾ͻ����м䣬������ŵõ�������
    }
    h = image_h - 2;
    //�ұ�0
    for (j = 0; j < total_R; j++)
    {
        if (points_r[j][1] == h)
        {
            Findline.rightline[h] = points_r[j][0] - 1;
        }
        else continue;//ÿ��ֻȡһ���㣬û����һ�оͲ���¼
        h--;
        if (h == 0)break;//�����һ���˳�
    }
}

//�������ͺ͸�ʴ����ֵ����
#define threshold_max   255*5//�˲����ɸ����Լ����������
#define threshold_min   255*2//�˲����ɸ����Լ����������
void image_filter(uint8(*mt9v03x_image_new)[image_w])//��̬ѧ�˲�������˵�������ͺ͸�ʴ��˼��
{
    uint16 i, j;
    uint32 num = 0;


    for (i = 1; i < image_h - 1; i++)
    {
        for (j = 1; j < (image_w - 1); j++)
        {
            //ͳ�ư˸����������ֵ
            num =
                    mt9v03x_image_new[i - 1][j - 1] + mt9v03x_image_new[i - 1][j] + mt9v03x_image_new[i - 1][j + 1]
                + mt9v03x_image_new[i][j - 1] + mt9v03x_image_new[i][j + 1]
                + mt9v03x_image_new[i + 1][j - 1] + mt9v03x_image_new[i + 1][j] + mt9v03x_image_new[i + 1][j + 1];


            if (num >= threshold_max && mt9v03x_image_new[i][j] == 0)
            {

                mt9v03x_image_new[i][j] = 255;//��  ���Ը�ɺ궨�壬�������

            }
            if (num <= threshold_min && mt9v03x_image_new[i][j] == 255)
            {

                mt9v03x_image_new[i][j] = 0;//��

            }

        }
    }

}

/*
�������ƣ�void image_draw_rectan(uint8(*ov7725_uart_image_dec)[image_w])
����˵������ͼ��һ���ڿ�
����˵����uint8(*image)[image_w] ͼ���׵�ַ
�������أ���
�޸�ʱ�䣺2022��9��8��
��    ע��
example�� image_draw_rectan(ov7725_uart_image_dec);
 */
void image_draw_rectan(uint8(*image)[image_w])
{

    uint8 i = 0;
    for (i = 0; i < image_h; i++)
    {
        image[i][0] = 0;
        image[i][1] = 0;
        image[i][image_w - 1] = 0;
        image[i][image_w - 2] = 0;

    }
    for (i = 0; i < image_w; i++)
    {
        image[0][i] = 0;
        image[1][i] = 0;
        //image[image_h-1][i] = 0;

    }
}

/*
�������ƣ�void findline(void)
����˵�������մ�����
����˵������
�������أ���
�޸�ʱ�䣺2022��9��8��
��    ע��
example�� image_process();
 */
void findlineb(void)
{
    uint16 i;
    uint8 hightest = 0;//����һ������У�tip����������ָ����yֵ����С
    /*�������ߵ����õ�*/
    //image_filter(&ov7725_uart_image_dec);//�˲�
    image_draw_rectan(&mt9v03x_image_new[160]);//Ԥ����
    //����
    data_stastics_l = 0;
    data_stastics_r = 0;
    if (get_start_point(image_h - 2))//�ҵ�����ˣ���ִ�а�����û�ҵ���һֱ��
    {
        //printf("���ڿ�ʼ������\n");
        search_l_r((uint16)USE_num, &mt9v03x_image_new[160], &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
        //printf("�������ѽ���\n");
        // ����ȡ�ı߽�������ȡ���� �� ��������������õı���
        get_left(data_stastics_l);
        get_right(data_stastics_r);
    }
    for (i = hightest; i < image_h-1; i++)
        {
            Findline.midline[i] = (Findline.leftline[i] + Findline.rightline[i])/2;//������
            //�������������󣬲����ǲ��߻�����״̬����ȫ�����ʹ��һ����ߣ����������������ܸ����������
        }

        EmergencyStop();
}

/**********************************����ͼ��Ѱ�����ߣ��������м�������************************************/
void findline(void)
{

    int16 i = 0, j = 0;
    //������ʼ��
    lose_line = 0 ;
    Findline.midline[119] = 80;
    Findline.leftline[119] = 0;
    Findline.rightline[119] = 159;

    Findline.leftstartpoint = 0;//��¼��ʼ�����ұ���ʱ�����ֵ
    Findline.rightstartpoint = 0;

    Findline.l_lose=0;
    Findline.r_lose=0;

    //��ͨѲ��
    {
        for(i = 118; i > 0; i--)
        {
            Findline.midline[i] = 0;
            Findline.leftline[i] = 0;
            Findline.rightline[i] = 159;
            Findline.leftlineflag[i] = 0;
            Findline.rightlineflag[i] = 0;
            Findline.endline = 0;

            //Ѱ���������
            if(Findline.midline[i + 1] > 80)j = Findline.midline[i + 1] > 155 ? 155 : Findline.midline[i + 1];
            else j = Findline.midline[i + 1] < 4 ? 4 : Findline.midline[i + 1];  //���ҵ��м�λ�ã����м�����߿�ʼ��

            for(; j > 3; j--)
            {
                if(mt9v03x_image_new[i][j] == 255 && mt9v03x_image_new[i][j - 1] == 255 && mt9v03x_image_new[i][j - 2] == 0 && mt9v03x_image_new[i][j - 3] == 0)
                {
                    if(Findline.leftstartpoint == 0)
                        Findline.leftstartpoint = i;
                    Findline.leftline[i] = j - 1;
                    Findline.leftlineflag[i] = 1;
                    break;
                }
            }

            //Ѱ���������
            if(Findline.midline[i + 1] > 80)j = Findline.midline[i + 1] > 155 ? 155 : Findline.midline[i + 1];
            else j = Findline.midline[i + 1] < 4 ? 4 : Findline.midline[i + 1];
            for(j = Findline.midline[i + 1]; j < 156; j++)
            {
                if(mt9v03x_image_new[i][j] == 255 && mt9v03x_image_new[i][j + 1] == 255 && mt9v03x_image_new[i][j + 2] == 0 && mt9v03x_image_new[i][j + 3] == 0)
                {
                    if(Findline.rightstartpoint == 0)
                        Findline.rightstartpoint = i;
                    Findline.rightline[i] = j + 1;
                    Findline.rightlineflag[i] = 1;
                    break;
                }
            }

            //�洢��������
            Findline.midline[i] = (Findline.leftline[i] + Findline.rightline[i]) / 2;

            //������������
            //load_width[i] = -Findline.leftline[i] + Findline.rightline[i];

            //���߶�û�ҵ�
            if((Findline.leftlineflag[i] == 0) && (Findline.rightlineflag[i] == 0))
            {
                lose_line++;
                Findline.midline[i] = Findline.midline[i + 1];
            }
            else if(lose_line > 0)
                lose_line--;

            if(Findline.leftlineflag[i] == 0)
                Findline.l_lose++;

            if(Findline.rightlineflag[i] == 0)
                Findline.r_lose++;

            //�Ҷ���(������ʧ��)
            if(Findline.midline[i + 1] > 80)j = Findline.midline[i + 1] > 155 ? 155 : Findline.midline[i + 1];
            else j = Findline.midline[i + 1] < 4 ? 4 : Findline.midline[i + 1];
            if( mt9v03x_image_new[i - 1][j] == 0 && (mt9v03x_image_new[i - 1][j - 2] == 0 && mt9v03x_image_new[i - 1][j + 2] == 0)  &&  mt9v03x_image_new[i - 2][j] == 0 && (mt9v03x_image_new[i - 2][j - 2] == 0 && mt9v03x_image_new[i - 2][j + 2] == 0))
            {
                Findline.endline = i;
                break;
            }
            Findline.endline=max(Findline.endline,30);

        }
    }

    EmergencyStop();    //����ͣ��

    widthlose=0;
    for(uint16 i=70;i>20;i--)
    {
        if(width[i]-(Findline.rightline[i]-Findline.leftline[i])>5)
        {
            widthlose++;
        }
    }
}

/*************************************����ƫ��***************************/
void caculate_err(void)
{
    int16 i,sum;
    lastError = Findline.err[0];

    sum = 0;
    for(i = 59; i >= 1; i--)
        Findline.err[i] = Findline.err[i - 1]; //ƫ���˲�
    Findline.err[0] = 0.0;
    if(qianzhan > Findline.endline + 4)//<58
    {
        for(i = qianzhan; i > Findline.endline && i > qianzhan-4; i--) // ����ͷǰհ�޸�λ�ã� ͨ���޸�i�ķ�Χ������ǰհ ԭ����8
        {
            sum++;
            Findline.err[0] += (float)( Findline.midline[i]-80);
        }
    Findline.err[0] = Findline.err[0] / sum;
    }
    else
        Findline.err[0] = (float)(Findline.midline[Findline.endline + 2] - 80);

    if (lastError * Findline.err[0] < 0 && fabs(lastError - Findline.err[0]) > 30)
    {
        Findline.err[0] = lastError;
    }

    if(start_flag==0)
    {
        Findline.err[0]=0;
    }

    if(right_lucounter==0&&youluzhang_flag)
    {
        Findline.err[0] = -chuerr;
    }
    if(left_lucounter==0&&zuoluzhang_flag)
    {
        Findline.err[0] = chuerr;
    }
    if(right_lucounter==1&&youluzhang_flag)
    {
        Findline.err[0] = huierr;
    }
    if(left_lucounter==1&&zuoluzhang_flag)
    {
        Findline.err[0] = -huierr;
    }

}

/***************************** ����ͣ��**************************************/
void EmergencyStop(void)
{
    //˼·������ʱ�����ļ��д󲿷ֶ��Ǻ�ɫ�ģ�ֻҪ�ⲿ�ֵĺ�ɫ�����ۼƴ���һ����ֵ����Ϊ����
    int16 i = 0, j = 0, count_black = 0 ;
    for(i = 118; i > 115; i--)
    {
        for(j = 130; j >= 30; j--)
        {
            if(mt9v03x_image_new[i][j] == 0)
            {
                count_black = count_black + 1;
            }
        }
    }
    if(count_black > 280&&!stop_flag)    //���ص����ĳһ��ֵ��������ͣ����־λ����
    {
        Findline.loseflag = 1;
        stop();
    }
}

void dynamic_qianzhan(void)
{
    float err_temp;
//    if(fabs(Findline.err[0]) >= 40 ){err_temp = 40;}
//    else {
//        err_temp = fabs(Findline.err[0]);
//    }
//
//    qianzhan = qianzhan_init + 0.5*err_temp;

    err_temp = fabs(Findline.err[0]);

    if(err_temp <= 5 ){qianzhan = qianzhan_init;}
    else if (err_temp <= 12) {
        qianzhan = qianzhan_init + 5;
    }

    else if (err_temp <= 20) {
        qianzhan = qianzhan_init + 10;
    }

    else {
        qianzhan = qianzhan_init + 16;
    }

}



