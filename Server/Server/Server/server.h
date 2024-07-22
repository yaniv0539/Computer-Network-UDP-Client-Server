#pragma once

void GetTime(char* o_SendBuff);
void GetTimeWithoutDate(char* o_SendBuff);
void GetTimeSinceEpoch(char* o_SendBuff);
void GetClientToServerDelayEstimation(char* o_SendBuff);
void MeasureRTT(char* o_SendBuff);
void GetTimeWithoutDateOrSeconds(char* o_SendBuff);
void GetYear(char* o_SendBuff);
void GetMonthAndDay(char* o_SendBuff);
void GetYear(char* o_SendBuff);
void GetMonthAndDay(char* o_SendBuff);
void GetSecondsSinceBeginningOfMonth(char* o_SendBuff);
void GetWeekOfYear(char* o_SendBuff);
void GetDaylightSavings(char* o_SendBuff);
void GetTimeWithoutDateInCity(int clientChoice, char* o_SendBuff);
void MeasureTimeLap(char* o_SendBuff, time_t& o_PrevTime, bool firstRequest);