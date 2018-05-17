#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//Read Data and convert into Struct
const int NumSensors = 5;

const int A =20;
const int B =15;
const int C =10;
const int D =5;


 struct DataStruct
  {
      int id;
      int data;
  } Datos;
int table [5][2];
bool PumpeisON = false;
void ResetTable( int table[][2]){

	for (int i=0;i<NumSensors;i++){
		table[i][0]=0;
		table[i][1]=0;
	}
}

void printTable( int table[][2]){

	for(int i=0;i<NumSensors;i++){
		printf("id: %d value: %d \n",table[i][0],table[i][1]);
	}
}


bool mainPump(struct DataStruct Datos, bool PumpeisON){

    int OpenPumpe = 0;
    int ClosePumpe = 0;
    int SumTemp = 0;
    int AvgTemp = 0;
    if (Datos.data  < D || Datos.data > A){

        if(Datos.data < D && !PumpeisON){
		printf("OpenPumpe \n");
		ResetTable(table);
		PumpeisON = true;
            //send(OpenPumpe);
        }

        if(Datos.data  > A && PumpeisON){
	      printf("ClosePumpe");
	      ResetTable(table);
             //send(ClosePumpe);
        }


    }

    else{
        bool RepitedData = false;
        for(int i=0;i<NumSensors-1;i++){

            if(table[i][0]==Datos.id){
                RepitedData = true;
            }
        }
        if(!RepitedData){
	    int aux=0;
	    while(table[aux][0] != 0){
		aux++;
	    }
            table[aux][0] = Datos.id;
            table[aux][1] = Datos.data;
	    printf("AddedToTable \n");
	    printTable(table);
        }


        if(table[NumSensors-1][0] != 0){
	    printf("ALL SENSORS SENDED THE DATA \n");
            //AvgData
            for(int i=0;i<NumSensors-1;i++){
                SumTemp = SumTemp + table[i][1];
            }
            AvgTemp = SumTemp / NumSensors;

            if(AvgTemp < C && AvgTemp > D){
                OpenPumpe=1;
            }

            if(AvgTemp < A && AvgTemp > B && PumpeisON){
                ClosePumpe=1;
            }
            ResetTable(table);

        }
        if(OpenPumpe==1 && !PumpeisON){
		printf("OPENPUMPE \n");
		PumpeisON = true;
            //send(OpenPumpe);
            }

        if(ClosePumpe==1 && PumpeisON){
	    	printf("CLOSEPUMPE \n");
		PumpeisON = false;
            //send(ClosePumpe);
            }
    }
return PumpeisON;
}

void main( int argc, char * argv[]){

	Datos.id = strtol( argv[1],NULL,10);
	Datos.data = strtol( argv[2],NULL,10);
	bool PumpeState=false;
	PumpeState = mainPump(Datos,PumpeState);
	int identifier=1;
	int datasensor=0;

	while(Datos.id != 0){
		printf("introduce the data: ");
		scanf("%d %d",&identifier,&datasensor);
		Datos.id=identifier;
		Datos.data=datasensor;
		printf("_____________________");
		PumpeState = mainPump(Datos,PumpeState);

	}
}
