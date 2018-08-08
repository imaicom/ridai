// cc -o remote remote.c -lm

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>

void main() {
	
   int v1,v2,ww,c1,c2,c3,c4;

    v1 = 0;    // 縦軸入力
    v2 = 0;    // 横軸入力
    ww = 10;   // 回転入力

    c1 = ( 8 * v1 +  8 * v2 +  6 * ww ) / 10;   // 左前
    c2 = ( 8 * v1 + -8 * v2 + -6 * ww ) / 10;   // 右前
    c3 = ( 8 * v1 + -8 * v2 +  6 * ww ) / 10;   // 左後
    c4 = ( 8 * v1 +  8 * v2 + -6 * ww ) / 10;   // 右後

    printf(" 左前=%4d ",c1);
    printf(" 右前=%4d ",c2);
    printf("\n");
    printf(" 左後=%4d ",c3);
    printf(" 右後=%4d ",c4);
    printf("\n");

    
}
