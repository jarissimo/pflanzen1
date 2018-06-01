#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//Number of sensors
#define NUM_SENSORS 5

//ID of the water level sensor
#define ID_WATER_LEVEL_SENSOR 6

//Lowest Humidicity level accepted for the water level sensor
#define HUMIDICITY_LEVEL_ACCEPTED 5

//Border values for the sensors we should edit the values
#define PUMP_THRESHOLD_VERYHIGH  20
#define PUMP_THRESHOLD_HIGH  15
#define PUMP_THRESHOLD_LOW  10
#define PUMP_THRESHOLD_VERYLOW  5

//Struct that stores id of the sensors and humidity value
struct PumpDataStruct
{
    int id;
    int data;
} pump_data;

//Table to storage sensors data the size of the table depens on the number of sensors
int table [NUM_SENSORS][2];
bool pump_is_on = false;

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


//TODO: I would say pump_is_on should be global, not a parameter to this. --marian
bool pump_set_data(struct PumpDataStruct pump_data, bool pump_is_on)
{
    int open_pump = 0;
    int close_pump = 0;
    int sum_hum = 0;
    int avg_hum = 0;

    if(pump_data.id == ID_WATER_LEVEL_SENSOR && pump_data.data < HUMIDICITY_LEVEL_ACCEPTED)
    {
        if(pump_is_on){
            printf("ClosePumpe \n");
            printf("Need to be filled");
        }

        else{
            printf("Need to be filled");
        }

    }

    if (pump_data.data  < PUMP_THRESHOLD_VERYLOW || pump_data.data > PUMP_THRESHOLD_VERYHIGH){

        if(pump_data.data < PUMP_THRESHOLD_VERYLOW && !pump_is_on){
            printf("OpenPumpe \n");
            reset_table(table);
            pump_is_on = true;
            //Here we should call the function that opens the pump
        }

        if(pump_data.data  > PUMP_THRESHOLD_VERYHIGH && pump_is_on) {
            printf("ClosePumpe");
            reset_table(table);
            //Here we should call the function that closes the pump
        }


    }

    else {
        bool repeated_data = false;
	//If the sensor is already present in the table we update his value if not we add it to the table
        for(int i=0;i<NUM_SENSORS-1;i++) {

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
                open_pump=1;
            }

            if(avg_hum < PUMP_THRESHOLD_VERYHIGH && avg_hum > PUMP_THRESHOLD_HIGH && pump_is_on){
                close_pump=1;
            }
            reset_table(table);

        }
        if(open_pump==1 && !pumpe_is_on){
            printf("OPENPUMPE \n");
            pump_is_on = true;
            //Here we should call the functions that opens the pump
        }

        if(close_pump==1 && pumpe_is_on){
            printf("CLOSEPUMPE \n");
            pump_is_on = false;
            //Here we should call the functions that closes the pump
        }
    }
    return pump_is_on;
}

int shell_pump_set_data( int argc, char * argv[])
{
    //TODO We should change this insted of keyboard parameters, a functions should send the values
    if ( argc < 2 ) {
        printf("Usage: %s pump_id data_value\n", argv[0]);
        return 1;
    }

    pump_data.id = strtol( argv[1],NULL,10);
    pump_data.data = strtol( argv[2],NULL,10);
    static bool pump_state=false;
    pump_state = pump_set_data(pump_data,pumpe_state);
    return 0;
}
