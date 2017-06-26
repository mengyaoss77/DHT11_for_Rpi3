/*
* dht11.c for RPi3 B
* author: mengyaoss77
* 2017.06.25
*/

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <math.h>

#define PIN 7
//rest is x seconds
#define REST 60*10*1000

int main()
{
    wiringPiSetup();
    int data[40];
    int i;
    int count = 0;
    
    while(1) {

        for(i = 0; i < 40; i++)
            data[i] = 0;

        if(start(data, 40) == 0)
            if(check(data) == 0) {
                delay(REST);
                continue;
            }


        printf("重试\n");
        delay(REST);

    }
    return 0;
}

int check(int *data)
{
    int dec[5] = {0, 0, 0, 0, 0};
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            dec[i] += data[j + i*8] * pow(2, 7-j);
        }
    }
    int checksum = dec[0] + dec[1] + dec[2] + dec[3];
    if(dec[4] == checksum & 0xff) {
        printf("湿度: %d.%d%% |温度: %d.%d \n", dec[0], dec[1], dec[2], dec[3]);
        write2file(dec);
        return 0;
    }
    else {
        //printf("湿度: %d.%d%%\n温度: %d.%d 摄氏度\n", dec[0], dec[1], dec[2], dec[3]);
        printf("check failed\n");
        return -1;
    }
}

int start(int *data, int len)
{
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, HIGH); 
    delay(100);
    digitalWrite(PIN, LOW);
    delay(18);
    digitalWrite(PIN, HIGH);
    pinMode(PIN, INPUT);

    int ct = 0;
    // wait for dht response
    while(digitalRead(PIN) == HIGH) {
        ct ++;
        delayMicroseconds(1);
        if (ct == 255) {
            printf("no response\n");
            return -1; //failed
        }
    }

    while(digitalRead(PIN) == LOW) {
        delayMicroseconds(1);
    }
    
    for(ct = 0; ct < len; ct ++) {
        while(digitalRead(PIN) == HIGH) {
            delayMicroseconds(1);
        }
        while(digitalRead(PIN) == LOW){
            delayMicroseconds(1);
        }
        delayMicroseconds(27);
        if(digitalRead(PIN) == HIGH) {
            data[ct] = 1;
            //delayMicroseconds(40);
        }
            
    }
    return 0;
}

int write2file(int *dec)
{
    FILE *f;
    FILE *temp;
    char buf[56];
    f = fopen("data.txt", "r");
    temp = fopen("temp.txt", "w");
    int i = 0;
    int j;
    while(fgets(buf, 56, f) != 0){
        i++;
        //printf("%s", buf);
        fputs(buf, temp);
    }
    fclose(f);
    fclose(temp);

    f = fopen("data.txt", "w");
    temp = fopen("temp.txt", "r");
    if(i = 9) { //full
        fgets(buf, 56, temp);
        for(j = 1; j < 10; j++){
            fgets(buf, 56, temp);
            fputs(buf, f);
        }
    }else {
        for(j = 0; j <= i; j++){
                fgets(buf, 56, temp);
                fputs(buf, f);
            }
    }
    sprintf(buf, "%d.%d,%d.%d\n", dec[0], dec[1], dec[2], dec[3]);
    fputs(buf, f);
    fclose(f);
    fclose(temp);
    return 0;
}