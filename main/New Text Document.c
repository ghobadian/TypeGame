#include <stdio.h>
#include <windows.h>
#include "shashi.h"
#include "helper_windows.h"
#include <unistd.h>
#include <time.h>

char buffer[11][25];	//global
char entered_char;
int points=0;
int wave=0;
int changeWaveAllowed=0;	//global
//int move_up=0;


void my_callback_on_key_arrival(char c) {	//getting character from user
	entered_char = c;
}
void changing_buffer() {					//deleting first word by moving rest of the words one step back
	for(int i=0;buffer[i][0]!=0;i++)
		strcpy(buffer[i] , buffer[i+1]);
	if(buffer[0][0] == 0) {
		wave++;
        changeWaveAllowed=1;
	}
}
void loadNewWordsFromFile(FILE* words) {		//reading 10 new words from the file
	for(int i=0;i<10;i++)
		fscanf(words,"%s",buffer[i]);
}
int findWordType(char str[]) {			//function for categorizing words based on points
	int size = strlen(word);
	for(int i=0;i<size;i++)
		if(word[i] == '@' || word[i] == '$' || word[i] == '%' || word[i] == '^' || word[i] == '&' || word[i] == '!')
			return 3;
		else if(word[i] == '#')
			return 0;
			
	if(size>10)
		for(int i=0;i<size;i++)
			if('0' <= word[i] && word[i] <= '9')
				for(int j=0;j<size;j++)
					if(word[j] == '_')
						return 2;
	return 1;
}

int main() {
	setcolor(15);
	buffer[11][0] = 0;
	
	printf("Wellcome To Sonic Typing :0\n");	//difficulty menu
	printf("Choose Your Dificulty:\n");
	printf("1) Noob\n2) Amateur\n3) Expert\n");
	int dificulty;
	scanf("%d",&dificulty);
	while(dificulty!= 1 && dificulty!= 2 && dificulty!= 3) {	//check if dificulty is entered correctly
		printf("Wronge Number");
		sleep(1);
		printf("\r             \r");
		scanf("%d",&dificulty);
	}
	system("cls");
	float wave_time_span;
	float time_span_decreasing_percentage;
	switch(dificulty) {
		case 1 : {wave_time_span = 10; time_span_decreasing_percentage = .8; break;}
		case 2 : {wave_time_span = 8; time_span_decreasing_percentage = .7; break;}
		case 3 : {wave_time_span = 5; time_span_decreasing_percentage = .6; break;}
	}
	
	FILE *words = fopen("words.txt","r");	//getting words from file
    loadNewWordsFromFile(words);
	
	HANDLE thread_id = start_listening(my_callback_on_key_arrival);
	
	int hidden_word[]={1,3,6};
	int counter=0;
	int n;
	int freeze = 0;
	for(n=0;n<1000;n++) {		//new word coming from above
		if(n==20) {
			setcolor(4);
			printf("GAME OVER");
			setcolor(15);
			break;
		}
		clock_t start_t,end_t,start_t2,end_t2;
		start_t = clock();			//using clock() to scape from the bellow loop if time is up
		end_t = clock();
		
		while(end_t-start_t<wave_time_span*1000) {
			if(wave_time_span<1)
				break;
			if(changeWaveAllowed) {
                changeWaveAllowed = 0;
                loadNewWordsFromFile(words);
				n=0;
				wave_time_span *= time_span_decreasing_percentage;
				hidden_word[0] = 1;	hidden_word[1] = 3;	hidden_word[2] = 6;
			}
			for(int j=0;j<20;j++) {	//printing empty layout
				gotoxy(0,j);
				printf("#");
				gotoxy(21,j);
				printf("#");
			}
			
			gotoxy(0,20);			//printing points and ceiling
			for(int temp=0;temp<22;temp++)
				printf("#");
			printf("\n\npoints:%d",points);
			printf("\nwave:%d",wave+1);
			
			if(freeze == 1) {
				setcolor(1);
//				printf("ICE MODE ACCTIVATED");
				setcolor(15);
				wave_time_span*=3;
				freeze=2;
			}
			else if(freeze==2) {
				wave_time_span/=3;
				freeze=0;
			}
			
			for(int nth_word=n;nth_word>=0;nth_word--) {		//printing floating words
				int size = strlen(buffer[nth_word]);	//size of each word
				int starting_position = (20-size)/2;	
				gotoxy(starting_position,n-nth_word);		//used for center alignment
				
				for(int alphabet=0;alphabet<size;alphabet++) { //printing each word
					if(nth_word==0) {
						start_t2= clock();
						end_t2 = clock();
						while(counter!=size && end_t2-start_t2<wave_time_span*1000) {			//printing the front word that its letters should become red
							if(buffer[nth_word][counter] == entered_char)
								counter++;
							for(int j=0;j<counter;j++) {			//printing alphabets that have already been pressed in red
								setcolor(4);
								printf("%c",buffer[nth_word][j]);
								setcolor(15);
							}
							for(int last=counter;last<size;last++)	//printing alphabets that haven't been press yet in white
								printf("%c",buffer[nth_word][last]);
							gotoxy(starting_position,n);
							
	//						struct timespec remaining, request = {0,50000000};
	//						nanosleep(&request, &remaining);		//waiting for 0.05 second
							
							end_t2 = clock();
						}
						if(counter==size) {
							points+= findWordType(buffer[0]);	//giving points based on dificulty of the word that is typed
							if(findWordType(buffer[0]) == 0)
								freeze=1;
							if(hidden_word[0] == 0 || hidden_word[1] == 0 || hidden_word[2] == 0 )
								points++;
							changing_buffer();
							counter=0;		//reseting counter for the next word
							for(int i=0;i<3;i++)
								hidden_word[i]--;
							if(n!=0)
								n--;
						}
						break;
					}
					else
						if(nth_word==hidden_word[0] || nth_word==hidden_word[1] || nth_word==hidden_word[2]) {
							gotoxy(1,n-nth_word);
							for(int i=0;i<20;i++)
								printf("*");
						}
						else
							printf("%c",buffer[nth_word][alphabet]);
				}
			}
			system("cls");
			
			end_t = clock();
		}
		
	}
	fclose(words);
//	system("cls")
	printf("VICTORY IS YOURS")
	WaitForSingleObject(thread_id, INFINITE);
	return 0;
}
