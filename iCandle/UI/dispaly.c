

/** Paint display */
void UpdateDisplay(){

  LCDClean();
  LCD_DataWrite(displayTitle[0]);
  LCD_DataWrite(displayTitle[1]);
  LCD_DataWrite(displayTitle[2]);
  LCD_DataWrite(displayTitle[3]);
  LCD_DataWrite(displayTitle[4]);
  LCD_DataWrite(displayTitle[5]);
  LCD_DataWrite(displayTitle[6]);
  LCD_DataWrite(displayTitle[7]);
  LCD_DataWrite(displayTitle[8]);
  LCD_DataWrite(displayTitle[9]);
  LCD_DataWrite(displayTitle[10]);
  LCD_DataWrite(displayTitle[11]);
  LCD_DataWrite(displayTitle[12]);
  LCD_DataWrite(displayTitle[13]);
  LCD_DataWrite(displayTitle[14]);
  LCD_DataWrite(displayTitle[15]);
  LcdChangeLine();
  LCD_DataWrite(displayTitle[16]);
  LCD_DataWrite(displayTitle[17]);
  LCD_DataWrite(displayTitle[18]);
  LCD_DataWrite(displayTitle[19]);
  LCD_DataWrite(displayTitle[20]);
  LCD_DataWrite(displayTitle[21]);
  LCD_DataWrite(displayTitle[22]);
  LCD_DataWrite(displayTitle[23]);
  LCD_DataWrite(displayTitle[24]);
  LCD_DataWrite(displayTitle[25]);
  LCD_DataWrite(displayTitle[26]);
  LCD_DataWrite(displayTitle[27]);
  LCD_DataWrite(displayTitle[28]);
  LCD_DataWrite(displayTitle[29]);
  LCD_DataWrite(displayTitle[30]);
  LCD_DataWrite(displayTitle[31]);

}




/** Sets playing time and mode icon to left display title */
void ScreenPlayTime(){
  unsigned int playTime;
  unsigned char minutes, seconds;

  playTime = VS_ReadReg(SCI_DECODE_TIME);
  minutes = playTime/60;
  seconds = playTime%60;

  displayTitle[0]=('P');//+('a'-'z')*(second/2)
  displayTitle[1]=('l');
  displayTitle[2]=('a');
  displayTitle[3]=('y');//32 40 
  displayTitle[4]=('i');
  displayTitle[5]=('n');
  displayTitle[6]=('g');
  displayTitle[7]=('.'*(second/2));
  displayTitle[8]=('.'*((second+1)/2));
  displayTitle[9]=(' ');
  displayTitle[10]=('0'+minutes/10);
  displayTitle[11]=('0'+minutes%10);
  displayTitle[12]=(':');
  displayTitle[13]=('0'+seconds/10);
  displayTitle[14]=('0'+seconds%10);
  displayTitle[15]=(' ');

}