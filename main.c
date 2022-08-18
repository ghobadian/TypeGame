#include <stdio.h>
#include "shashi.h"
#include "helper_unix.h"
#include <unistd.h>
#include <time.h>
#include <string.h>

char buffer[11][25];	//global variables
char entered_char;
int wave=0;
int changeWaveAllowed=0;
int word_in_wave = 10;	//global variables

int wordHasNumber(char *word);

int wordHasUnderScore(char word[]);

void removeSpecialModeEffect(int *is_special_mode, int *change_size, float *wave_time_span);

void changeWave(float time_span_decreasing_percentage, int *hidden_word, float *wave_time_span, int *n);

void closeFiles(FILE *users, FILE *words);

void printUserWonMessage(int points, int highest_score, FILE *users, int dificulty);

void printUserLostMessage(int points, int highest_score, FILE *users);

void checkUsernameExistsInFile(const char *username, const char *username_from_file, int *score_from_file, FILE *users,
                               int *highest_score, int *position_of_highscore, int *usernameExistsInFile);

void my_callback_on_key_arrival(char c);

void changing_buffer() {						//deleting first word by moving rest of the words one step back
	for(int i=0;buffer[i][0]!=0;i++)
		strcpy(buffer[i] , buffer[i+1]);
	if(buffer[0][0] == 0) {
		wave++;
        changeWaveAllowed=1;
	}
}

void loadNewWordsFromFile(FILE* words) {
	for(int i=0;i<word_in_wave;i++)
		fscanf(words,"%s",buffer[i]);
}



int findWordType(char word[]) {
	int size = strlen(word);
	for(int i=0;i<size;i++)
		if(word[i] == '@' || word[i] == '$' || word[i] == '%' || word[i] == '^' || word[i] == '&' || word[i] == '!')
			return 3;
		else if(word[i] == '#')
			return 0;
    if(size > 10 && wordHasNumber(word) == 1 && wordHasUnderScore(word) == 1)
        return 2;
	return 1;
}

int wordHasNumber(char *word) {
    int size = strlen(word);
    for(int i=0;i<size;i++)
        if('0' <= word[i] && word[i] <= '9')
            return 1;
    return 0;
}

int wordHasUnderScore(char word[]) {
    int size = strlen(word);
    for(int j=0;j<size;j++)
        if(word[j] == '_')
            return 1;
    return 0;
}


int main() {
	setcolor(15);
	buffer[11][0] = 0;
	int points=0;
	int is_special_mode=0;
	int change_size = 0;
	
	char username[20];
	char username_from_file[20];
	int highest_score=0;
	int score_from_file;
	int positionOfHighscore;
	int usernameExistsInFile=1;
	printf("Wellcome To Sonic Typing :0\n");	
	printf("Enter Your Name: ");
	scanf("%s",username);
	FILE* users = fopen("users.txt","r+");
    checkUsernameExistsInFile(username, username_from_file, &score_from_file, users, &highest_score,
                              &positionOfHighscore, &usernameExistsInFile);
    if(usernameExistsInFile == 0) {
		fprintf(users,"\n%s %03d",username,0);					//user is not found in file
		positionOfHighscore = ftell(users) - 3;
	}
	fseek(users, positionOfHighscore, SEEK_SET);				//used for changing highest score in file
	
	printf("Choose Your Difficulty:\n");											//difficulty menu
	printf("1) Noob\n2) Amateur\n3) Expert\n4) Customize Difficulty\n");
	int difficulty;
	scanf("%d",&difficulty);
	while(difficulty != 1 && difficulty != 2 && difficulty != 3 && difficulty != 4) {	//check if difficulty is entered correctly
		printf("Wrong Number");
		sleep(1);
		printf("\r             \r");
		scanf("%d",&difficulty);
	}
	float wave_time_span;
	float time_span_decreasing_percentage;
	switch(difficulty) {
		case 1 : {wave_time_span = 10; time_span_decreasing_percentage = .8; break;}
		case 2 : {wave_time_span = 8; time_span_decreasing_percentage = .7; break;}
		case 3 : {wave_time_span = 5; time_span_decreasing_percentage = .6; break;}
		case 4 : {
			printf("Enter Wave Time Span >1 : ");					//customizing wave time span & time span decreasing percentage
			scanf("%f",&wave_time_span);
			printf("\nEnter Time Span Decreasing Percentage <1: ");
			scanf("%f",&time_span_decreasing_percentage);
			break;
		}
	}
	
	int settings;
	printf("Do You Want To Change Settings : ");				//let user choose setting menu
	printf("\n(for YES type 1 | for NO type 0)\n");
	scanf("%d",&settings);
	
	int start_game = 0;
	int width=21;
	int height=20;
	if(settings == 1) {
		while(start_game == 0) {
			printf("Which Item Do You wish To Change:\n");
			printf("1) Width\n2) Height \n3) How Many Words For Each Wave\n4) start game\n");
			scanf("%d",&settings);
			switch(settings) {
				case 1 :{										//customizing width
					printf("\nEnter Width:");
					scanf("%d",&width);
					printf("Widgth Changed To %d\n",width);
					sleep(2);
					break;
				}
				case 2 :{										//customizing height
					printf("\nEnter Height:");
					scanf("%d",&height);
					printf("Height Changed To %d\n",height);
					sleep(2);
					break;
				}
				case 3 :{										//decreasing or increasing the number words in each wave
					printf("\nEnter A Number:");
					scanf("%d",&word_in_wave);
					printf("Each Wave Will Have %d Words\n",word_in_wave);
					sleep(2);
					break;
				}
				case 4 :{ start_game = 1; break;}				//starting the game
			}
		}
	}
	system("cls");
	
	FILE *words = fopen("words.txt","r");						//getting words from file
    loadNewWordsFromFile(words);
	
	pthread_t thread_id = start_listening(my_callback_on_key_arrival);
	
	int user_lives=3;
	int hidden_word[]={1,3,6};
	for(int i=0;i<3;i++)							//solution for mobham word and word in wave bug
		if(hidden_word[i]>=word_in_wave)
			hidden_word[i]=-1;
	int counter=0;
	int n;
	int freeze = 0;
	for(n=0;n<1000;n++) {							//new word coming from above
		if(n==height) {
			user_lives--;
			n=0;
		}
		if(user_lives==0) {
            printUserLostMessage(points, highest_score, users);
            closeFiles(users, words);
			sleep(5);
			return 0;
		} else if(wave_time_span<1) {
            printUserWonMessage(points, highest_score, users, difficulty);
            closeFiles(users, words);
            sleep(5);
			return 0;
		}
		if(changeWaveAllowed) {
            loadNewWordsFromFile(words);
            changeWave(time_span_decreasing_percentage, hidden_word, &wave_time_span, &n);
        }
		
		if(is_special_mode == 2) {
            removeSpecialModeEffect(&is_special_mode, &change_size, &wave_time_span);
        }
		if(freeze==2) {							//removing the "kalameh yakhi" effect
			wave_time_span/=3;
			freeze=0;
		}
		
		clock_t start_t;
        clock_t end_t;
		start_t = clock();										//using clock() to let other words move forward if time is up
		end_t = clock();
		while(end_t-start_t<wave_time_span*1000 && buffer[0][0]!=0) {
			for(int j=0;j<height;j++) {							//printing empty layout
				gotoxy(0,j);
				printf("#");
				gotoxy(width,j);
				printf("#");
			}
			gotoxy(0,height);									//printing points, wave number, user's lives,
			for(int temp=0;temp<width+1;temp++)					//user's highest score and ceiling
				printf("#");
			printf("\n\npoints:%d",points);
			printf("\nwave:%d",wave+1);
			printf("\nYour Lives: ");
			for(int i=0;i<user_lives;i++)
				printf("%c ",3);
			printf("\nHighest Score: %d",highest_score);
			
			if(freeze == 1) {				//freeze mode is on
				setcolor(1);
				setcolor(15);
				wave_time_span*=3;
				freeze=2;
			}
			
			if(is_special_mode == 1) {		//special mode is on
				wave_time_span*=2;
				is_special_mode = 2;
				change_size=1;
			}
			
			for(int nth_word=n;nth_word>=0;nth_word--) {		//printing floating words
				int size;
				if(change_size)		//related to special mode
					size = 3;
				else
					size = strlen(buffer[nth_word]);	//size of each word
				int starting_position = (width-1-size)/2;	
				gotoxy(starting_position,n-nth_word);		//used for center alignment
				
				if(nth_word==0) {
					while(counter!=size && end_t-start_t<wave_time_span*1000 && buffer[0][0]!= 0) {			//printing the front word that its letters
						if(buffer[nth_word][counter] == entered_char)										//should become red
							counter++;
						if(buffer[nth_word][counter] == buffer[nth_word][counter-1] || buffer[nth_word][counter-1]== buffer[nth_word+1][0])	//double-skip 
							entered_char = '+';																								//problem
						for(int j=0;j<counter;j++) {			//printing alphabets that have already been pressed in red
							setcolor(4);
							printf("%c",buffer[nth_word][j]);
							setcolor(15);
						}
						for(int last=counter;last<size;last++)	//printing alphabets that haven't been press yet in white
							printf("%c",buffer[nth_word][last]);
						gotoxy(starting_position,n);
						
						struct timespec remaining, request = {0,50000000};
						nanosleep(&request, &remaining);		//waiting for 0.05 second
						
						end_t = clock();
					}
					if(counter==size) {
						points+= findWordType(buffer[0]);	//giving points based on difficulty of the word that is typed
						if(findWordType(buffer[0]) == 0)		//checking for freeze mode
							freeze=1;					
						if(hidden_word[0] == 0 || hidden_word[1] == 0 || hidden_word[2] == 0 )
							points++;					//giving extra point for "kalamateh mobham"
						if(n>height-4 && is_special_mode==0 && findWordType(buffer[0]) == 3)
							is_special_mode = 1;		//checking for special mode
						changing_buffer();
						counter=0;						//reseting counter for the next word
						for(int i=0;i<3;i++)
							hidden_word[i]--;
						if(n!=0)
							n--;
					}
					break;
				}
				else{
					if(nth_word==hidden_word[0] || nth_word==hidden_word[1] || nth_word==hidden_word[2]) {
						gotoxy(1,n-nth_word);			//not showing the "mobham" words
						for(int i=0;i<width-1;i++)
							printf("*");
					}
					else if(strlen(buffer[nth_word])>=3)
						for(int alphabet=0;alphabet<size;alphabet++) 		//printing other words in white
							printf("%c",buffer[nth_word][alphabet]);
				}
			}
			system("cls");
			end_t = clock();
		}
	}
	pthread_join(thread_id, NULL);
	return 0;
}

void checkUsernameExistsInFile(const char *username, const char *username_from_file, int *score_from_file, FILE *users,
                               int *highest_score, int *position_of_highscore, int *usernameExistsInFile) {
    while(fscanf(users, "%s %d", username_from_file, score_from_file) != EOF)	//checking users file
        if(strcmp(username_from_file, username) == 0) {
            (*highest_score) = (*score_from_file);
            (*position_of_highscore) = ftell(users)-3;
            (*usernameExistsInFile) =0;
        }
}

void printUserLostMessage(int points, int highest_score, FILE *users) {
    if(highest_score < points)
        fprintf(users,"%03d",points);
    gotoxy(6,6);
    setcolor(4);
    printf("GAME OVER");
    setcolor(15);
}

void printUserWonMessage(int points, int highest_score, FILE *users, int dificulty) {
    if(highest_score < points)
        fprintf(users,"%03d",points);
    gotoxy(5,3);
    setcolor(2);
    printf("VICTORY IS YOURS\n\n\n");
    setcolor(15);
    printf("Wave:%d\n",wave);
    printf("Points:%03d vs Highest_score:%03d\n",points,highest_score);
    printf("Choosen Dificulty:%d",dificulty);
}

void closeFiles(FILE *users, FILE *words) {
    fclose(words);
    fclose(users);
}

void changeWave(float time_span_decreasing_percentage, int *hidden_word, float *wave_time_span, int *n) {
    changeWaveAllowed = 0;
    (*n) =0;
    (*wave_time_span) *= time_span_decreasing_percentage;
    hidden_word[0] = 1;
    hidden_word[1] = 3;
    hidden_word[2] = 6;
}

void removeSpecialModeEffect(int *is_special_mode, int *change_size, float *wave_time_span) {
    (*is_special_mode) = 69;
    (*wave_time_span) /=2;
    (*change_size) =0;
}

void my_callback_on_key_arrival(char c) {	//getting character from user
    entered_char = c;
}
