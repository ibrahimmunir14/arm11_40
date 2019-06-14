#include<stdio.h>
#include<string.h>
#include<stdlib.h>

// function for creating and writing into csv file
void create_marks_csv(char **dates, double *prices, int num_data){
    FILE *fp;

    //create csv file
    fp = fopen("csvfile.csv","w+");

    for(int i = 0; i < num_data; i++) {
        fprintf(fp, "%s,%f\n", dates[i], prices[i]);
    }
    fclose(fp);
}