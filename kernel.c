void handleInterrupt21(int ax,int bx,int cx,int dx);
void readString(char*);
void printString(char*);
void readSector(char*,int);
void excecuteProgram(char*, int);
int readFile(char*);
void writeSector(char*,int);
void deleteFile(char*);
void terminate();
void writeFile(char*, char* , int);
void copy(char*,char*);
void showDir();
char get(int);
void putInArray(char*,char*);
void handleTimerInterrupt(int, int );
void getModified(int);
typedef struct {
	int active;
	int sp;
} process;

process  processTable[8] ;
int  currentProcess =0;
int timer = 0;

int main(){
	//char buffer[13312];
	char x [2];

	int i;
	for( i =0; i<8 ; i++)
	{
	   processTable[i].active =0;
	   processTable[i].sp =0xff00;
	}
	makeTimerInterrupt();
	makeInterrupt21();

	interrupt(0x21, 4, "hello2\0", 0, 0);
	interrupt(0x21, 4, "hello1\0", 0, 0);


	// interrupt(0x21, 4, "shell\0", 0x2000, 0);
	//showDir();
	while(1); /*hang up*/
}

void handleTimerInterrupt(int segment, int sp){
	int i;
	int j;
	char value [3];
	j = currentProcess+1;
	value[1]='\0';
	if(timer==100){
		printhex();

/* saving the sp doesn't seeem to work
		setKernelDataSegment();

		processTable[currentProcess].sp = sp;
		restoreDataSegment();
		*/

		timer =0;
		for( i=0;i<7;i++){
			if(processTable[j].active){

				segment = (j+2)*0x1000;
				setKernelDataSegment();
				sp = processTable[j].sp;
				restoreDataSegment();
				currentProcess = j;
				break;
			}
			else
				if(j==7){
					j=0;
				}
				else j++;
		}

	}
	else timer++;
	// restoreDataSegment();
	returnFromTimer(segment, sp);
}

void readSector(char* buffer,int sector){
  int relative_sector;
  int head;
  int track;
  int AH=2,AL=1,DL=0,AX,CX,DX;

  track = div(sector,36);
  relative_sector =  mod(sector,18) + 1;
  head = mod(div(sector,18),2);
  AX=AH*256+AL;
  CX=track*256+relative_sector;
  DX=head*256+DL;
  /*this is integer division, so the result should be rounded down*/
  interrupt(0x13,AX,buffer,CX,DX);
}

int readFile(char* name , char* file){
	char dir[512];
	int sec;
	int found=0;
	char* arr;
	int  i = 0 , j, k = 0;
	int x;
	file[0] = '\0';
	readSector(dir,2);
	while(i<16){
		j = 0;
		while(j<6){
			if(name[j] != dir[(i*32)+j]){
				found=0;

				break;
			}
			else if(j==5) found = i;
			j++;
		}
		if(found)break;
		i++;
	}
	if(found){
	 for(i=6;i<32;i++){
		 x =dir[found*32 + i];
		if(x==0)break;
		readSector(file,x);
		file+=512;k+=512;
	}
	file-=k;

	 //interrupt(0x21, 0,"\n\rA:\0",0, 0);
	return 1;
}
else{
	// printString("\n\rA:\n\0");
	return 0;
}

}



void printString(char* word){

    while(*word!='\0'){
      int AX=0xE*256+*word;
      interrupt(0x10,AX,0,0,0);
      word++;
    }
}

void readString(char* a){
    int i=2;
     int AX;
     char x;
     a[0]=0xa;
     a[1]=0xd;
    while(1){
        x = interrupt(0x16,0,0,0,0);
        if(x==0xd){ i--;break;}

        AX=0xE*256+x;
        interrupt(0x10,AX,0,0,0);
          if(x==0x8){
          i--;
          AX=0xE*256+' ';
          interrupt(0x10,AX,0,0,0);
           AX=0xE*256+x;
        interrupt(0x10,AX,0,0,0);
          }
          else{
        a[i]=x;
        i++;
          }
        }
        i++;
        a[i]=0xa;
    i++;
    a[i]=0x0;
}

void excecuteProgram(char* name){
	int i;
	int segment;
	char temp[13312];
	if(readFile(name,temp)){
	setKernelDataSegment();
	for(i =0; i<8 ;i++)
	{
	  if(processTable[i].active ==0)
	    {
		//printString(name);
		processTable[i].active =1;
		break;
	    }
	}
	restoreDataSegment();
	segment = (i+2)*0x1000;
	for(i = 0 ; i<13312 ; i++){

		putInMemory(segment, i, temp[i]);
	}
	//printString(name);
	// launchProgram(segment);
	initializeProgram(segment);
}

}

void terminate(){
	// printString("hello\0");
 // printhex();
	// interrupt(0x21, 4, "shell\0", 0x5000, 0);
	setKernelDataSegment();
	processTable[currentProcess].active =0;
	while(1);
}

int div(int x ,int y){
  int counter=0;
  while(x>1){
    x-=y;
    counter++;
  }
  if(x<0)counter--;
  return counter;
}

int mod(int x,int y){
  int value=div(x,y);
  return x-(y*value);
}

int stringEquals(char* a , char* b){
  int found = 0;
  while(*a!='\0'){
    if(*a != *b && found == 1 ){
      return 0;
    }
    else if(*a == *b){
       found = 1;
  }
    a++;b++;
  }
  return found;
}

void writeSector(char* buffer,int sector){
  int relative_sector;
  int head;
  int track;
  int AH=3,AL=1,DL=0,AX,CX,DX;

  track = div(sector,36);
  relative_sector =  mod(sector,18) + 1;
  head = mod(div(sector,18),2);
  AX=AH*256+AL;
  CX=track*256+relative_sector;
  DX=head*256+DL;
  interrupt(0x13,AX,buffer,CX,DX);
}

void deleteFile(char* name){
	char dir[512];
	int sec;
	int found=0;
	char* arr;
	char directory[512];
	char map[512];
	int  i = 0 , j, k = 0;
	int x;
	readSector(dir,2);
	while(i<16){
		j = 0;
		while(j<6){
			if(name[j] != dir[(i*32)+j]){
				found=0;
				break;
			}
			else if(j==5) found = i;
			j++;
		}
		if(found)break;
		i++;
	}
	if(found){
		readSector(map,1);
		readSector(directory,2);
		i=6;
		directory[found*32]=0x00;
		while(i<26){
			if(directory[found*32+i]!=0x00){
				map[directory[found*32+i]]=0x00;
			}else break;
			i++;
		}
		writeSector(map,1);
		writeSector(directory,2);
	}
	else return;
}

void writeFile(char* name, char* buffer, int secNum){
	char directory[512];
	char map[512];
	int j=0;
	int k=0;
	int i=0;
	readSector(map,1);
	readSector(directory,2);
	while(i<16){
		if(directory[i*32]==0x00){
			while(j<6){
				if(name[j]!='\0')directory[i*32+j]=name[j];
				else directory[i*32+j]=0x00;
				j++;
			}
			while(j<26){
				if(secNum>0){
					while(map[k]!=0x00){
						k++;
					}
					directory[i*32+j]=k;
					map[k]=0xff;
					writeSector(buffer,k);
					buffer+=512;
				secNum--;}
				else directory[i*32+j]=0x00;
				j++;
			}
	               writeSector(map,1);
	               writeSector(directory,2);
			return;
		}
		i++;
	}
		printString("No Free Space");
}

void copy(char* file1 , char* file2){

	char dir[512];
	int found=0;
  int j=6 ;
	 char* buffer[512];
  int sectorNumber=0;
//printString("hena\0");
	readSector(dir,2);
  //printString("hena2\0");
  found=readFile(file1,buffer);
  //printString("hena3\0");
	if(found){
		while(j<32 && dir[found*32 + j]!= 0x00){
				j++;
			 sectorNumber++;
     }
     interrupt(0x21,0,"File copied\0",0,0);
 writeFile(file2,buffer,sectorNumber);

}
else interrupt(0x21,0,"No such file\0",0,0);
}


void showDir(){
  int i=0;
  int j=0;
  int z=0;
  int counter = 0;
  char name[15];
  char dir[512];
	char no[5];

  interrupt(0x21,2,dir,2,0);

  while(i<16 ){

    while(j<6 && dir[i*32+j]!=0x0){
      name[j]=dir[i*32+j];
      j++;
    }
    name[j] = ' ';
    name[j+1] = '\0';


		j=6;
    while(j<32 && dir[i*32+j]!=0x0){
      j++;
      counter++;
    }

		no[0] = get(div(counter,10));
    no[1] = get(mod(counter,10));
    no[2] = '\n';
    no[3]='\r';
    no[4]='\0';

if(counter !=0 && name[0] != ' ')
  {  printString(name);

    printString(no);
  }
	  j=0;
    counter = 0;
    i++;
  }
}


void getModified(int x,char* output){
	int i =0;
	while(x>0){
			output[i] = get(mod(x,10));
			x= div(x,10);
			i++;
	}
	output[i] = '\n';
}

char get(int x){

	if(x ==0) return '0';
	if(x ==1) return '1';
	if(x ==2) return '2';
	if(x ==3) return '3';
	if(x ==4) return '4';
	if(x ==5) return '5';
	if(x ==6) return '6';
	if(x ==7) return '7';
	if(x ==8) return '8';
	if(x ==9) return '9';

}

void putInArray(char * a , char * b){
  int i = 0;
  while(*a != '\0'){
    b[i] = a;
    i++;
  }
  b[i] = '\0';
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
 	if(ax==0)printString(bx);
	else if(ax==1)readString(bx);
	else if(ax==2)readSector(bx,cx);
	else if(ax==3)readFile(bx,cx);
	else if(ax==4)excecuteProgram(bx);
	else if(ax==5)terminate();
	else if(ax==6)writeSector(bx,cx);
	else if(ax==7)deleteFile(bx);
	else if(ax==8)writeFile(bx,cx,dx);
	else if(ax==9)copy(bx,cx);
	else if(ax==10)showDir();
	else printString("ERROR:Ma3leeeesh");
}
