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


//int main(){
//    char **dates = calloc(3, sizeof(char *));
//    char *date1 = "30-40-50";
//    char *date2 = "50-50-60";
//    char *date3 = "60-50-49";
//
//    double *prices = calloc(3, sizeof(double));
//    double p1 = 30.0;
//    double p2 = 56.3;
//    double p3 = 6.6;
//
//    dates[0] = date1;
//    dates[1] = date2;
//    dates[2] = date3;
//
//    prices[0] = p1;
//    prices[1] = p2;
//    prices[2] = p3;
//
//    create_marks_csv(dates, prices, 3);
//
//    return 0;
//}