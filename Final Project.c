/*
	Tittle: Econo-Flight Airline Reservation Program
	Studet Name : Juan David Daza Velosa
	Student Number ID : 108722182
	PRG 255 Section: E
	Final Poject
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#define SIZE 12

struct Information
{
	char firtsNameOfThePerson[20];
	char lastNameOfThePerson[20];
	int seatNumber;
	int flag;
};

void allSeatsAssignmentAndEmpty(int plane, struct Information *pPersonalInformation);
void numberOfEmptySeats(int plane, struct Information *pPersonalInformation);
void assignCostumerSeat(int plane, struct Information *pPersonalInformation);
void deleteData(int plane, struct Information *pPersonalInformation);


int main()
{
	struct Information plane[SIZE], *pPersonalInformation;
	int numberOfSeats[SIZE];
	int seats[SIZE];
	int i, counter, emptySeats;
	char options, available[5] = "YES";

	emptySeats = 0;
	pPersonalInformation = plane;


	for (i = 0; i < SIZE; i++)
	{
		(pPersonalInformation + i)->flag = 0;
		(pPersonalInformation + i)->seatNumber = (i + 1);
	}

	printf("(a) Dsiplay All seat assignments (Including empty)\n");
	printf("(b) Display ONLY a list of empty seats and show total of empty seats\n");
	printf("(c) Assign the costumer to an empty seat\n");
	printf("(d) Delete All seat assignments\n");
	printf("(q) Quit the program\n\n");
	printf(" Please enter an option: ");
	scanf(" %c", &options);

	while (options != 'q' && options != 'Q')
	{

		switch (options)
		{

		case 'a':
		case 'A':

		{

			allSeatsAssignmentAndEmpty(SIZE, pPersonalInformation);

			break;

		}

		case 'b':
		case 'B':

		{

			numberOfEmptySeats(SIZE, pPersonalInformation);

			break;

		}

		case 'c':
		case 'C':

		{

			assignCostumerSeat(SIZE, pPersonalInformation);

		}

		break;

		case 'd':
		case 'D':

		{
			deleteData(SIZE, pPersonalInformation);
		}

		break;

		default: printf("\nInvalid value!\n");

		}

		printf("\n(a) Dsiplay All seat assignments (Including empty)\n");
		printf("(b) Display ONLY a list of empty seats and show total of empty seats\n");
		printf("(c) Assign the costumer to an empty seat\n");
		printf("(d) Delete All seat assignments\n");
		printf("(q) Quit the program\n\n");
		printf(" Please enter an option: ");
		scanf(" %c", &options);
	}

	_getch();
	return 0;

}


void allSeatsAssignmentAndEmpty(int plane, struct Information *pPersonalInformation)

{

	int i;
	char available[5] = "YES";

	printf("\nSeats \t First Name \t Last Name \t Assigned\n");
	for (i = 0; i < SIZE; i++)
	{
		if ((pPersonalInformation + i)->flag == 0)
		{

			printf(" %d \t EMPTY \t\t EMPTY \t\t NO\n", (pPersonalInformation + i)->seatNumber);

		}

		else
		{
			printf(" %d\t %s \t %s \t %s \n", (pPersonalInformation + i)->seatNumber, (pPersonalInformation + i)->firtsNameOfThePerson, (pPersonalInformation + i)->lastNameOfThePerson, available);
		}
	}
}


void numberOfEmptySeats(int plane, struct Information *pPersonalInformation)

{
	int i, emptySeats;
	char available[5] = "YES";

	emptySeats = 0;

	printf("\nSeats \t First Name \t Last Name \t Assigned\n");

	for (i = 0; i < SIZE; i++)
	{
		if ((pPersonalInformation + i)->flag == 0)
		{
			printf(" %d \t EMPTY \t\t EMPTY \t\t NO\n", (pPersonalInformation + i)->seatNumber);
			emptySeats = emptySeats + 1;
		}

		else if ((pPersonalInformation + i)->seatNumber != (pPersonalInformation + i)->seatNumber)

		{

			printf(" %d\t %s \t %s \t %s \n", (pPersonalInformation + i)->seatNumber, (pPersonalInformation + i)->firtsNameOfThePerson, (pPersonalInformation + i)->lastNameOfThePerson, available);

			emptySeats = emptySeats - SIZE;
		}

	}

	printf("\nThe number of empty seats are: %d\n", emptySeats);
}


void assignCostumerSeat(int plane, struct Information *pPersonalInformation)

{
	int i, counter;

	puts("\nPlease select the number of your seat:");
	scanf(" %d", &counter);

	for (i = 0; i < SIZE; i++)
	{
		if ((pPersonalInformation + i)->seatNumber == counter)
		{
			puts("\nPlease enter your first name: ");
			getchar();
			gets((pPersonalInformation + i)->firtsNameOfThePerson);
			puts("\nPlease enter the your last name: ");
			gets((pPersonalInformation + i)->lastNameOfThePerson);
			(pPersonalInformation + i)->flag = 1;
		}
	}
}


void deleteData(int plane, struct Information *pPersonalInformation)

{
	int i;

	for (i = 0; i < SIZE; i++)

	{
		strcpy((pPersonalInformation + i)->firtsNameOfThePerson, " ");
		strcpy((pPersonalInformation + i)->lastNameOfThePerson, " ");
		(pPersonalInformation + i)->flag = 0;
	}

}