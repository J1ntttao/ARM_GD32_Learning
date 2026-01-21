#include "App.h"
#include "oled.h"

u8 DeviceState = DeviceDefault;
char LevelSwitch = Level1;

u8 Lv1Lvsw = 1; 
u8 Lv2Lvsw = 1; 
u8 Lv3Lvsw = 1; 

void App_OLED_refresh(){	// 128 * 64
		if(DeviceState == DeviceDefault){
                OLED_ShowString(0,0,"GUESS COLOR v1.0",16);
                OLED_ShowString(0,2,"Please Press",16);
				OLED_ShowString(0,4,"Rightmost Button",16);
				OLED_ShowString(0,6,"to Start",16);
		}
		else if(DeviceState == DeviceLevelSW){
             switch(LevelSwitch){
                 case Level1:
                        OLED_ShowString(0,0,"Difficulty SW:",16);
                        OLED_ShowString(0,2,"> 1. Normal",16);
                        OLED_ShowString(0,4,"  2. Hard",16);
                        OLED_ShowString(0,6,"  3. Experts",16);
                        break;
                 case Level2:
                        OLED_ShowString(0,0,"Difficulty SW:",16);
                        OLED_ShowString(0,2,"  1. Normal",16);
                        OLED_ShowString(0,4,"> 2. Hard",16);
                        OLED_ShowString(0,6,"  3. Experts",16);
                        break;
                 case Level3:
                        OLED_ShowString(0,0,"Difficulty SW:",16);
                        OLED_ShowString(0,2,"  1. Normal",16);
                        OLED_ShowString(0,4,"  2. Hard",16);
                        OLED_ShowString(0,6,"> 3. Experts",16);	
                        break;
                 default:
                        OLED_Clear();
                        break;
             }
		}
		else if(DeviceState == DeviceLvSwitch){
             if(LevelSwitch == Level1)
             {
                 switch(Lv1Lvsw){
                     case 1:
                            OLED_ShowString(0,0,"> Normal Lv1 ",16);
                            OLED_ShowString(0,2,"  Normal Lv2 ",16);
                            OLED_ShowString(0,4,"  Normal Lv3 ",16);
                            OLED_ShowString(0,6,"  Normal Lv4 ",16);
                            break;                        
                     case 2:                              
                            OLED_ShowString(0,0,"  Normal Lv1 ",16);
                            OLED_ShowString(0,2,"> Normal Lv2 ",16);
                            OLED_ShowString(0,4,"  Normal Lv3 ",16);
                            OLED_ShowString(0,6,"  Normal Lv4 ",16);
                            break;                        
                     case 3:                              
                            OLED_ShowString(0,0,"  Normal Lv1 ",16);
                            OLED_ShowString(0,2,"  Normal Lv2 ",16);
                            OLED_ShowString(0,4,"> Normal Lv3 ",16);
                            OLED_ShowString(0,6,"  Normal Lv4 ",16);
                            break;                        
                     case 4:                              
                            OLED_ShowString(0,0,"  Normal Lv1 ",16);
                            OLED_ShowString(0,2,"  Normal Lv2 ",16);
                            OLED_ShowString(0,4,"  Normal Lv3 ",16);
                            OLED_ShowString(0,6,"> Normal Lv4 ",16);
                            break;                 
                     default:
                            OLED_Clear();
                            break;
                 }
             }
             else if(LevelSwitch == Level2)
             {
                 switch(Lv2Lvsw){
                     case 1:
                            OLED_ShowString(0,0,"> Hard Lv1 ",16);
                            OLED_ShowString(0,2,"  Hard Lv2 ",16);
                            OLED_ShowString(0,4,"  Hard Lv3 ",16);
                            OLED_ShowString(0,6,"  Hard Lv4 ",16);
                            break;                      
                     case 2:                            
                            OLED_ShowString(0,0,"  Hard Lv1 ",16);
                            OLED_ShowString(0,2,"> Hard Lv2 ",16);
                            OLED_ShowString(0,4,"  Hard Lv3 ",16);
                            OLED_ShowString(0,6,"  Hard Lv4 ",16);
                            break;                      
                     case 3:                            
                            OLED_ShowString(0,0,"  Hard Lv1 ",16);
                            OLED_ShowString(0,2,"  Hard Lv2 ",16);
                            OLED_ShowString(0,4,"> Hard Lv3 ",16);
                            OLED_ShowString(0,6,"  Hard Lv4 ",16);
                            break;                      
                     case 4:                            
                            OLED_ShowString(0,0,"  Hard Lv1 ",16);
                            OLED_ShowString(0,2,"  Hard Lv2 ",16);
                            OLED_ShowString(0,4,"  Hard Lv3 ",16);
                            OLED_ShowString(0,6,"> Hard Lv4 ",16);
                            break;                 
                     default:
                            OLED_Clear();
                            break;
                 }
             }    
             else if(LevelSwitch == Level3)
             {   
                 switch(Lv3Lvsw){
                     case 1:
                            OLED_ShowString(0,0,"> Experts Lv1 ",16);
                            OLED_ShowString(0,2,"  Experts Lv2 ",16);
                            OLED_ShowString(0,4,"  Experts Lv3 ",16);
                            OLED_ShowString(0,6,"  Experts Lv4 ",16);
                            break;                         
                     case 2:                               
                            OLED_ShowString(0,0,"  Experts Lv1 ",16);
                            OLED_ShowString(0,2,"> Experts Lv2 ",16);
                            OLED_ShowString(0,4,"  Experts Lv3 ",16);
                            OLED_ShowString(0,6,"  Experts Lv4 ",16);
                            break;                         
                     case 3:                               
                            OLED_ShowString(0,0,"  Experts Lv1 ",16);
                            OLED_ShowString(0,2,"  Experts Lv2 ",16);
                            OLED_ShowString(0,4,"> Experts Lv3 ",16);
                            OLED_ShowString(0,6,"  Experts Lv4 ",16);
                            break;                         
                     case 4:                               
                            OLED_ShowString(0,0,"  Experts Lv1 ",16);
                            OLED_ShowString(0,2,"  Experts Lv2 ",16);
                            OLED_ShowString(0,4,"  Experts Lv3 ",16);
                            OLED_ShowString(0,6,"> Experts Lv4 ",16);
                            break;                 
                     default:
                            OLED_Clear();
                            break;
                 }
             }             
		}        
        
        /*
             Lv1: TIP显示本关出现的颜色	 
                  POS会显示情况指示灯（位置相关）
                  ACK不显示
                                                 
             Lv2: TIP显示本关出现的颜色
                  POS不显示
                  ACK会显示情况指示灯（位置无关，仅表示个数）
                                                 
             Lv3: TIP不显示
                  POS不显示
                  ACK会显示情况指示灯（位置无关，仅表示个数）
        */        
        
		else if(DeviceState == DeviceStart){
             char buff[20];
             OLED_Clear();
             switch(LevelSwitch){
                 case Level1:
                        sprintf(buff,"Nm Level %d",(int)Lv1Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"TIP: Color used",16);
                        OLED_ShowString(0,4,"POS: Green Exact",16);
                        OLED_ShowString(0,6,"ACK: Not Display",16);
                        break;
                 case Level2:
                        sprintf(buff,"Hd Level %d",(int)Lv2Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"TIP: Color used",16);
                        OLED_ShowString(0,4,"POS: Not Display",16);
                        OLED_ShowString(0,6,"ACK: Correct Cnt",16);
                        break;
                 case Level3:
                        sprintf(buff,"Ex Level %d",(int)Lv3Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"TIP: Not Display",16);
                        OLED_ShowString(0,4,"POS: Not Display",16);
                        OLED_ShowString(0,6,"ACK: Correct Cnt",16);
                        break;
                 default:
                        OLED_Clear();
                        break;     
             }
		}
		else if(DeviceState == DeviceDefeat){
             char buff[20];           
             OLED_Clear();
             switch(LevelSwitch){
                 case Level1:
                        sprintf(buff,"Nm Lv %d Failed",(int)Lv1Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"You Fail...",16);
                        OLED_ShowString(0,4,"Don`t Worry",16);
                        OLED_ShowString(0,6,"Let`s try again",16);
                        break;
                 case Level2:
                        sprintf(buff,"Hd Lv %d Failed",(int)Lv2Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"You Fail...",16);
                        OLED_ShowString(0,4,"You can do it",16);
                        OLED_ShowString(0,6,"Believe yourself",16);
                        break;
                 case Level3:
                        sprintf(buff,"Ex Lv %d Failed",(int)Lv3Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"You Fail...",16);
                        OLED_ShowString(0,4,"Is it still",16);      
                        OLED_ShowString(0,6,"too hard for you",16);
                        break;
                 default:
                        OLED_Clear();
                        break;
             }
		}
		else if(DeviceState == DeviceVictory){
             char buff[20]; 
             OLED_Clear();
             switch(LevelSwitch){
                 case Level1:
                        sprintf(buff,"Nm Lv %d Cleared",(int)Lv1Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"Perfect!",16);
                        OLED_ShowString(0,4,"You took the",16);
                        OLED_ShowString(0,6,"first step!",16);  
                        break;
                 case Level2:
                        sprintf(buff,"Hd Lv %d Cleared",(int)Lv2Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"Excellent!",16);
                        OLED_ShowString(0,4,"Well played!",16);
                        OLED_ShowString(0,6,"Keep it up!",16);
                        break;
                 case Level3:
                        sprintf(buff,"Ex Lv %d Cleared",(int)Lv3Lvsw);
                        OLED_ShowString(0,0,buff,16);
                        OLED_ShowString(0,2,"Congratulations!",16);
                        OLED_ShowString(0,4,"Unbelievable!",16);      
                        OLED_ShowString(0,6,"You made it!",16);
                        break;
                 default:
                        OLED_Clear();
                        break;
             }                
		} 		
}

