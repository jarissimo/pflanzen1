#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//Number of sensors
#define NUM_SENSORS 5

//Border values for the sensors we should edit the values
#define PUMP_THRESHOLD_VERYHIGH  20
#define PUMP_THRESHOLD_HIGH  15
#define PUMP_THRESHOLD_LOW  10
#define PUMP_THRESHOLD_VERYLOW  5

//Struct that stores id of the sensors and humidity value
 struct DataStruct
  {
      int id;
      int data;
  } pump_data;

//Table to storage sensors data the size of the table depens on the number of sensors
int table [5][2];
bool pumpe_is_on = false;

//Each time we open/close the pump we should reset the table values
void reset_table( int table[][2])
{

	for (int i=0;i<NUM_SENSORS;i++){
		table[i][0]=0;
		table[i][1]=0;
	}
}

//Function that prints the table with the values just for visual information
void print_table( int table[][2])
{

	for(int i=0;i<NUM_SENSORS;i++){
		printf("id: %d value: %d \n",table[i][0],table[i][1]);
	}
}


bool pump_set_data(struct DataStruct pump_data, bool pumpe_is_on)
{

    int open_pumpe = 0;
    int close_pumpe = 0;
    int sum_hum = 0;
    int avg_hum = 0;
    if (pump_data.data  < PUMP_THRESHOLD_VERYLOW || pump_data.data > PUMP_THRESHOLD_VERYHIGH){

        if(pump_data.data < PUMP_THRESHOLD_VERYLOW && !pumpe_is_on){
		printf("OpenPumpe \n");
		reset_table(table);
		pumpe_is_on = true;
            //Here we should call the function that opens the pumpe
        }

        if(pump_data.data  > PUMP_THRESHOLD_VERYHIGH && pumpe_is_on){
	      printf("ClosePumpe");
	      reset_table(table);
             //Here we should call the function that closes the pumpe
        }


    }

    else{
        bool repeated_data = false;
	//If the sensor is already present in the table we update his value if not we add it to the table
        for(int i=0;i<NUM_SENSORS-1;i++){

            if(table[i][0]==pump_data.id){
                repeated_data = true;
		table[i][1] = pump_data.data;
		printf("TableUpdated \n");
		print_table(table);
            }
        }
        if(!repeated_data){
	    int aux=0;
	    //We look for the first free space in the table
	    while(table[aux][0] != 0){
		aux++;
	    }
            table[aux][0] = pump_data.id;
            table[aux][1] = pump_data.data;
	    printf("AddedToTable \n");
	    print_table(table);
        }

	//When we got the values of all the sensors we operate with the values
        if(table[NUM_SENSORS-1][0] != 0){
	    printf("ALL SENSORS SENDED THE DATA \n");
            //Calculate the AvgHum
            for(int i=0;i<NUM_SENSORS-1;i++){
                sum_hum = sum_hum + table[i][1];
            }
            avg_hum = sum_hum / NUM_SENSORS;

            if(avg_hum < PUMP_THRESHOLD_LOW && avg_hum > PUMP_THRESHOLD_VERYLOW){
                open_pumpe=1;
            }

            if(avg_hum < PUMP_THRESHOLD_VERYHIGH && avg_hum > PUMP_THRESHOLD_HIGH && pumpe_is_on){
                close_pumpe=1;
            }
            reset_table(table);

        }
        if(open_pumpe==1 && !pumpe_is_on){
		printf("OPENPUMPE \n");
		pumpe_is_on = true;
            //Here we should call the functions that opens the pumpe
            }

        if(close_pumpe==1 && pumpe_is_on){
	    	printf("CLOSEPUMPE \n");
		pumpe_is_on = false;
            //Here we should call the functions that closes the pumpe
            }
    }
return pumpe_is_on;
}

void shell_pump_set_data( int argc, char * argv[])
{
	//We should change this insted of keyboard parameters, a functions should send the values
	pump_data.id = strtol( argv[1],NULL,10);
	pump_data.data = strtol( argv[2],NULL,10);
	bool pumpe_state=false;
	pumpe_state = pump_set_data(pump_data,pumpe_state);
	int identifier=1;
	int data_sensor=0;

	while(pump_data.id != 0){
		printf("introduce the data: ");
		scanf("%d %d",&identifier,&data_sensor);
		pump_data.id=identifier;
		pump_data.data=data_sensor;
		printf("_____________________");
		pumpe_state = pump_set_data(pump_data,pumpe_state);

	}
}
