#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define NUM_SENSORS 5

#define PUMP_THRESHOLD_VERYHIGH  20
#define PUMP_THRESHOLD_HIGH  15
#define PUMP_THRESHOLD_LOW  10
#define PUMP_THRESHOLD_VERYLOW  5


 struct DataStruct
  {
      int id;
      int data;
  } pump_data;
int table [5][2];
bool pumpeisON = false;
void resetTable( int table[][2]){

	for (int i=0;i<NUM_SENSORS;i++){
		table[i][0]=0;
		table[i][1]=0;
	}
}

void printTable( int table[][2]){

	for(int i=0;i<NUM_SENSORS;i++){
		printf("id: %d value: %d \n",table[i][0],table[i][1]);
	}
}


bool mainPump(struct DataStruct pump_data, bool pumpeisON){

    int openPumpe = 0;
    int closePumpe = 0;
    int sumHum = 0;
    int avgHum = 0;
    if (pump_data.data  < PUMP_THRESHOLD_VERYLOW || pump_data.data > PUMP_THRESHOLD_VERYHIGH){

        if(pump_data.data < PUMP_THRESHOLD_VERYLOW && !pumpeisON){
		printf("OpenPumpe \n");
		resetTable(table);
		pumpeisON = true;
            //send(OpenPumpe);
        }

        if(pump_data.data  > PUMP_THRESHOLD_VERYHIGH && pumpeisON){
	      printf("ClosePumpe");
	      resetTable(table);
             //send(ClosePumpe);
        }


    }

    else{
        bool repeatedData = false;
        for(int i=0;i<NUM_SENSORS-1;i++){

            if(table[i][0]==pump_data.id){
                repeatedData = true;
		table[i][1] = pump_data.data;
		printf("TableUpdated \n");
		printTable(table);
            }
        }
        if(!repeatedData){
	    int aux=0;
	    while(table[aux][0] != 0){
		aux++;
	    }
            table[aux][0] = pump_data.id;
            table[aux][1] = pump_data.data;
	    printf("AddedToTable \n");
	    printTable(table);
        }


        if(table[NUM_SENSORS-1][0] != 0){
	    printf("ALL SENSORS SENDED THE DATA \n");
            //AvgHum
            for(int i=0;i<NUM_SENSORS-1;i++){
                sumHum = sumHum + table[i][1];
            }
            avgHum = sumHum / NUM_SENSORS;

            if(avgHum < PUMP_THRESHOLD_LOW && avgHum > PUMP_THRESHOLD_VERYLOW){
                openPumpe=1;
            }

            if(avgHum < PUMP_THRESHOLD_VERYHIGH && avgHum > PUMP_THRESHOLD_HIGH && pumpeisON){
                closePumpe=1;
            }
            resetTable(table);

        }
        if(openPumpe==1 && !pumpeisON){
		printf("OPENPUMPE \n");
		pumpeisON = true;
            //send(OpenPumpe);
            }

        if(closePumpe==1 && pumpeisON){
	    	printf("CLOSEPUMPE \n");
		pumpeisON = false;
            //send(ClosePumpe);
            }
    }
return pumpeisON;
}

void main( int argc, char * argv[]){

	pump_data.id = strtol( argv[1],NULL,10);
	pump_data.data = strtol( argv[2],NULL,10);
	bool pumpeState=false;
	pumpeState = mainPump(pump_data,pumpeState);
	int identifier=1;
	int datasensor=0;

	while(pump_data.id != 0){
		printf("introduce the data: ");
		scanf("%d %d",&identifier,&datasensor);
		pump_data.id=identifier;
		pump_data.data=datasensor;
		printf("_____________________");
		pumpeState = mainPump(pump_data,pumpeState);

	}
}
