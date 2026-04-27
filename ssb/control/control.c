//
// Created by Administrator on 2026/4/24.
//

#include "control.h"
#include "main.h"
/*引用主函数的参数*/
extern float kpup,kdup,kpv,kiv,kt,a,p0;
extern int16_t es,lowout_last,ee,lowout;
extern int16_t outv,outup,outt,pwmout,pwmoutz,pwmouty;
extern int16_t ez,ey,ax,ay,az,gx,gy,gz;
extern float p,r,y,pa,GY,GZ,err,error;
void Control_up(void){//直立环PD
    outup=kpup*(p-p0)+kdup*(GY-0);
}
void Control_v(void){//速度环PI
    ee=(ez+ey)-0;//计算速度偏差
    lowout=(1-a)*ee+a*lowout_last;//低通滤波
    lowout_last=lowout;
    es+=ee;//对速度偏差积分出位移
    if(es>=10000) es=10000;
    if(es<=-10000) es=-10000;
    outv=kpv*lowout+kiv*es;//速度环输出控制计算
}
void Control_turn(void){//转向环
    outt=kt*GZ+err;
}
void Control_Limit(void){//输出限幅
    if(pwmoutz>=7200)  pwmoutz=7200;
    if(pwmoutz<=-7200) pwmoutz=-7200;
    if(pwmouty>=7200)  pwmouty=7200;
    if(pwmouty<=-7200) pwmouty=-7200;
}