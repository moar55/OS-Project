int stringEquals(char*,char*);
char* getCommParam(char*);
void copy(char*,char*);

int main(){
  char input[90];
  char* temp;
  //char* view = "\n\rview\n\0";
  //char* execute= "\n\rexecute\n\0";
  char file[13312];
  char file1[90];
  char dir[512];
  char line[90];
int i =0;
int j =0;
int k =0;
int found =0;
  while(1){
       for(i=0;i<90;i++)
      input[i]=0x0;

    interrupt(0x21,0,"SHELL>");
    interrupt(0x21,1,input,0,0);
    interrupt(0x21,0,"\r\n",0,0);
          if(input[2]=='v'&& input[3] == 'i'&& input[4]=='e'&& input[5] =='w' && input[6] ==' ')
    {

      interrupt(0x21,3,&input[7],file,0);
      interrupt(0x21,0,file,0,0);

    }

      //el 7eta eli bt3mil error
       else
	        if(input[2]=='e' && input[3] == 'x'&& input[4]=='e'&& input[5] =='c'&& input[6]=='u' && input[7]=='t' && input [8] == 'e'&& input[9] ==' ')
      {
        interrupt(0x21,4,&input[10],0x2000,0);

      }
      else{
          if(input[2]=='d' && input[3] == 'e'&& input[4]=='l'&& input[5] =='e'&& input[6]=='t' && input[7]=='e'&& input[8] ==' ')
          {
            i=0;
            k=9;
            interrupt(0x21,2,dir,2,0);
          	while(i<16){
          		j = 0;
          		while(j<6){
          			if(input[k] != dir[(i*32)+j]){
          				found=0;
          				break;
          			}
          			else if(j==5) found = i;
          			j++;
                k++;
          		}
          		if(found)break;
          		i++;
          	}
            if(found)
            {

              interrupt(0x21,0,"File deleted!\0",0,0);
              interrupt(0x21,7,&input[9],0,0);
            }
            else
            interrupt(0x21,0,"Couldn't find file!\0",0,0);

          }

          else
          if(input[2]=='c' && input[3] == 'o'&& input[4]=='p'&& input[5] =='y' && input[6] ==' ')
          {
            i=0;
            while(i<90)
              {
                file1[i] =0x0;
                i++;
              }
            i=0;
            while(input[i+7] != ' ')
            {
              file1[i] = input[i+7];
              i++;
            }
            j=i+8;

            copy(file1,&input[j]);
            interrupt(0x21,9,file1,&input[j],0);
          }
          else
          if(input[2]=='d' && input[3] == 'i'&& input[4]=='r' )
          {
            interrupt(0x21,10,0,0,0);
          }
          else
          if(input[2]=='c' && input[3] == 'r'&& input[4]=='e' && input[5]=='a' && input[6] == 't'&& input[7]=='e' && input[8] ==' ')
          {
            i=0;
            j=0;
            interrupt(0x21,2,dir,2,0);
            while(i<16)
            {
              if(dir[i*32]== 0x0)break;
              else i++;
            }
            if(i==16){
              interrupt(0x21,0,"no space\0",0,0);
            }
            else
            {
              while(j<16)
              {if(dir[j*32] ==input[9] && (dir[j*32 +1] ==input[10] || dir[j*32 +1] ==0x0) && (dir[j*32 +2] ==input[11] || dir[j*32 +2] ==0x0) && (dir[j*32 +3] ==input[12] || dir[j*32 +3] ==0x0) &&
                 (dir[j*32 +4] ==input[13] || dir[j*32 +1] ==0x0) && (dir[j*32 +5] ==input[14]|| dir[j*32 +5] ==0x0) )
                  break;
                  else
                  j++;

              }
              if(j<16)
              interrupt(0x21,0,"name taken\0",0,0);
              else
              {
                i=0;
                while(i<13312)
                {
                  file[i] =0x0;
                  i++;
                }
                i=0;
                j=0;

                while(1)
                {
                  interrupt(0x21,0,"enter a line to the file\n\r",0,0);
                    interrupt(0x21,1,line,0,0);
                    k=2;
                    if(line[2] == '\n')break;
                    else
                    {
                      interrupt(0x21,0,"\n\r",0,0);
                      j++;
                      while(line[k]!= 0x0 &&line[k]!= 0xa)

                        {

                          file[i] = line[k];
                          k++;
                          i++;
                        }
                        file[i] = '\r';
                        i++;
                        file[i] ='\n';
                        i++;

                    }

                }
                interrupt(0x21,8,&input[9],file,j);

              }
            }

          }
          else
        interrupt(0x21,0,"Bad Command\0",0,0);
      }
  interrupt(0x21,0,"\r\n",0,0);


    }
  }
  void copy(char* file1 , char* file2){

  	char dir[512];
  	int found=0;
    int j=6 ;
  	 char* buffer[512];
    int sectorNumber=0;
  //printString("hena\0");
  	interrupt(0x21,2,dir,2,0);
    buffer[0] = 0x0;
    interrupt(0x21,3,file1,buffer,0);
    //printString("hena3\0");
  	if(buffer[0] !=0x0){

       interrupt(0x21,0,"File copied\0",0,0);
   //interrupt(0x21,8,file2,buffer,sectorNumber);

  }
  else interrupt(0x21,0,"No such file\0",0,0);
  }
