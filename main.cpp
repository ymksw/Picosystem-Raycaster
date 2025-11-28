#include "picosystem.hpp"
//BASED ON 3DSAGE'S RAYCASTER V1
using namespace picosystem;

#include "happy.h"

#include <math.h>

//textures
buffer_t texture = {.w = 64, .h = 64, .data = (color_t *)_texture};



//-----------------------------MAP----------------------------------------------
#define mapX  8      //map width
#define mapY  8      //map height
#define mapS 64      //map cube size
int map[]=           //the map array. Edit to change level but keep the outer walls
{
 1,1,1,1,1,1,1,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,0,0,0,0,1,0,1,
 1,0,0,0,0,0,0,1,
 1,1,1,1,1,1,1,1,	
};



//------------------------PLAYER------------------------------------------------
float degToRad(int a) { return a*M_PI/180.0;}
int FixAng(int a){ if(a>359){ a-=360;} if(a<0){ a+=360;} return a;}

float px,py,pdx,pdy,pa; //player x,y,delta x,delta y,angle
float pdx2,pdy2; //sideways deltas


void Buttons()
{
 if(button(Y)){ pa+=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));     pdx2=cos(degToRad(FixAng(pa+90))); pdy2=-sin(degToRad(FixAng(pa+90)));}
 if(button(A)){ pa-=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));     pdx2=cos(degToRad(FixAng(pa+90))); pdy2=-sin(degToRad(FixAng(pa+90)));}
 //if(button(UP)){ px+=pdx*5; py+=pdy*5;}
 //if(button(DOWN)){ px-=pdx*5; py-=pdy*5;}
 
 int xo=0; if(pdx<0){ xo=-20;} else{ xo=20;}                                    //x offset to check map
 int yo=0; if(pdy<0){ yo=-20;} else{ yo=20;}                                    //y offset to check map
 int ipx=px/64.0, ipx_add_xo=(px+xo)/64.0, ipx_sub_xo=(px-xo)/64.0;             //x position and offset
 int ipy=py/64.0, ipy_add_yo=(py+yo)/64.0, ipy_sub_yo=(py-yo)/64.0;             //y position and offset
 
 if(button(UP))                                                                  //move forward
 {  
  if(map[ipy*mapX        + ipx_add_xo]==0){ px+=pdx*5;}
  if(map[ipy_add_yo*mapX + ipx       ]==0){ py+=pdy*5;}
 }
 if(button(DOWN))                                                                  //move backward
 { 
  if(map[ipy*mapX        + ipx_sub_xo]==0){ px-=pdx*5;}
  if(map[ipy_sub_yo*mapX + ipx       ]==0){ py-=pdy*5;}
 } 
 
 int xo2=0; if(pdx2<0){ xo2=-20;} else{ xo2=20;}                                    //x offset to check map
 int yo2=0; if(pdy2<0){ yo2=-20;} else{ yo2=20;}                                    //y offset to check map
 int ipx2=px/64.0, ipx_add_xo2=(px+xo2)/64.0, ipx_sub_xo2=(px-xo2)/64.0;             //x position and offset
 int ipy2=py/64.0, ipy_add_yo2=(py+yo2)/64.0, ipy_sub_yo2=(py-yo2)/64.0;             //y position and offset
 if(button(LEFT))                                                                  //move forward
 {  
  if(map[ipy2*mapX        + ipx_add_xo2]==0){ px+=pdx2*5;}
  if(map[ipy_add_yo2*mapX + ipx2       ]==0){ py+=pdy2*5;}
 }
 if(button(RIGHT))                                                                  //move backward
 { 
  if(map[ipy2*mapX        + ipx_sub_xo2]==0){ px-=pdx2*5;}
  if(map[ipy_sub_yo2*mapX + ipx2       ]==0){ py-=pdy2*5;}
 }

 //if(button(LEFT)){ px+=pdx2*5; py+=pdy2*5;}
 //if(button(RIGHT)){ px-=pdx2*5; py-=pdy2*5;} 
}//-----------------------------------------------------------------------------


//---------------------------Draw Rays and Walls--------------------------------
//float distance(ax,ay,bx,by,ang){ return cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay);}

void drawRays2D()
{

 int r,mx,my,mp,dof,side; float vx,vy,rx,ry,ra,xo,yo,disV,disH; 
 
 ra=FixAng(pa+30);                                                              //ray set back 30 degrees

 for(r=0;r<60;r++)
 {
  //---Vertical--- 
  dof=0; side=0; disV=100000;
  float Tan=tan(degToRad(ra));
       if(cos(degToRad(ra))> 0.001){ rx=(((int)px>>6)<<6)+64;      ry=(px-rx)*Tan+py; xo= 64; yo=-xo*Tan;}//looking left
  else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6) -0.0001; ry=(px-rx)*Tan+py; xo=-64; yo=-xo*Tan;}//looking right
  else { rx=px; ry=py; dof=8;}                                                  //looking up or down. no hit  

  while(dof<8) 
  { 
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                     
   if(mp>0 && mp<mapX*mapY && map[mp]>0){ dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  } 
  vx=rx; vy=ry;

  //---Horizontal---
  dof=0; disH=100000;
  Tan=1.0/Tan; 
       if(sin(degToRad(ra))> 0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;}//looking up 
  else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;      rx=(py-ry)*Tan+px; yo= 64; xo=-yo*Tan;}//looking down
  else{ rx=px; ry=py; dof=8;}                                                   //looking straight left or right
 
  while(dof<8) 
  { 
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                          
   if(mp>0 && mp<mapX*mapY && map[mp]>0){ dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  } 
  
  //pen(0,12,0);
  //if(disV<disH){ rx=vx; ry=vy; disH=disV; pen(0,9,0);}                  //horizontal hit first

  pen(0,0,0,0);
  if(disV<disH){ rx=vx; ry=vy; disH=disV; pen(0,0,0,3);}                  //horizontal hit first 

  /*
  int ca=FixAng(pa-ra); disH=disH*cos(degToRad(ca));                            //fix fisheye 
  int lineH = (mapS*320)/(disH); if(lineH>320){ lineH=320;}                     //line height and limit
  int lineOff = 160 - (lineH>>1);                                               //line offset
  
  rect(r * 8 + 530 - 4, lineOff, 8, lineH); // 8px thick line centered
  ra=FixAng(ra-1);*/                                                              //go to next ray
  int ca = FixAng(pa - ra); 
  disH = disH * cos(degToRad(ca)); // fix fisheye
  int lineH = (mapS * 120) / (disH); // Adjusted for 240 height
  if (lineH > 240) { 
    lineH = 240; 
  } // line height and limit
  int lineOff = 120 - (lineH >> 1); // line offset for 240 height

  ra=FixAng(ra-1);                                                              //go to next ray

  // Use rect to draw a filled rectangle as a thick line
  //frect(r * 8 - 4, lineOff, 8, lineH); // Centered in 240x240 window
  //vline(r,0,lineH);
  int textureOff = (((int)rx)%64) + (((int)ry)%64);
  blit(&texture,textureOff,0,1,64,r*8-4,lineOff,8,lineH);
  frect(r*8 -4,lineOff,8,lineH);//darken horizontal sides
 }
}//-----------------------------------------------------------------------------


void init()
{
 px=150; py=400; pa=90;
 pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));
 pdx2=cos(degToRad(FixAng(pa+90))); pdy2=-sin(degToRad(FixAng(pa+90)));
}

void update(uint32_t tick)
{
  Buttons();
}

void draw(uint32_t tick)
{   
 //pen(0,0,0);
 //clear();
 pen(0,0,15);
 clear();
 pen(0,15,15);
 frect(0,0,240,120);
 drawRays2D();
 //blit(&texture,0,0,64,64,0,0,240,240);
}
