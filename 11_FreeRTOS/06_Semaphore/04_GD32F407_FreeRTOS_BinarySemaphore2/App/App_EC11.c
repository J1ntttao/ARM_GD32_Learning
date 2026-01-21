#include "App.h"
#include "EC11.h"
//	 0      1       2      3     4      5     6       7				
//  red  orange  yellow	 green  cyan  blue  purple   无色														
// 0    5      10 		 15     20    25    30      35														
#if EC11
void App_EC11_Btn(){				
		EC11_Btn_Scan();
}		

void App_EC11_Knob(){

}
#endif