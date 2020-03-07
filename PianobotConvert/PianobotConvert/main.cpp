#include <iostream>
#include <fstream>
#include <string>
#include "List.h"

using namespace std;
/* Several objectives and milestones:
 * 1. Converts to the correct format
 * 2. Adds in proper stepper controls
 * 3. Add optimization for hand paths
 */
const int RHstart = 60; //Start on C4 for right hand
const int LHstart = 36; //Start on C2 for left hand
const int octave = 12; //Set #semitones
const int limitL = 48; //Left limit
const int limitR = 108; //Right limit


//Test Github 

int nextNote(int current, int notesToMove);
bool inRange(int handPos, int note);
int findFing(int handPos, int note);
int howFar(int handPos, int note);

int main(void)
{
	//Set up midi file to get information
	fstream inFile;
	inFile.open("Twinkle.txt", ios::in);
	if (!inFile)
	{
		cout << "Unable to open file";
		exit(1); // terminate with error
	}

	//Buffer to store input values
	string buffer;

	//Initialize arrays to store information
	int* time = new int[2000];
	int* onOff = new int[2000];
	int* note = new int[2000];
	int row = 0;
	int rowMax = 0;

	//Flag to determine which variable it is during input
	int flag = 0;

	//Hand position variables for calculation
	int LHpos;
	int RHpos;

	//List to store all hand positions and time stamps
	List song;

	//Temporary: Find leftmost note
	int leftmost = limitR;

	//Final Arduino output string
	string output;

	//Temp variables for general use
	int temp;
	int i;

	//Loop through to save all the values
	while (!inFile.eof())
	{
		if (flag == 0)
		{
			getline(inFile, buffer, ',');
			if (!buffer.empty())
				time[row] = stoi(buffer);
			cout << time[row] << ", ";
			flag++;
		}
		else if (flag == 1)
		{
			getline(inFile, buffer, ',');
			if (!buffer.empty())
				onOff[row] = stoi(buffer);
			cout << onOff[row] << ", ";
			flag++;
		}
		else
		{
			getline(inFile, buffer, '\n');
			if (!buffer.empty())
				note[row] = stoi(buffer);
			cout << note[row] << "\n";
			row++;
			flag = 0;
		}
	}
	rowMax = row;

	//Temporary: Find leftmost position to determine split line - Doesn't WORK currently
	for (row = 0; row < 2000; row++)
	{
		if ((note[row] < leftmost) && (note[row] >= limitL))
			leftmost = note[row];
	}
	LHpos = 36;

	//Find starting position of right hand
	int min;
	int max;
	flag = 1;
	for (i = 0; i < 2000; i++)
	{
		if (note[i] > leftmost + nextNote(leftmost, 2)) //Check if note is outside range of LH limit + 2
		{
			if (flag == 1) //Start by setting both max/min to first note
			{
				min = note[i];
				max = note[i];
				flag = 0;
			}
			else
			{
				//Loop until a note is outeside the starting range of 7
				if (note[i] < min)
				{
					if (nextNote(note[i], 7) > max)
						break;
					else
						min = note[i];
				}
				if (note[i] > max)
				{
					if (nextNote(min, 7) < note[i])
						break;
					else
						max = note[i];
				}
			}
		}
	}
	RHpos = min;

	//Initialize with first hand position
	song.createNode(LHpos,RHpos);

	//Begin adding hand positions and time stamps - Preliminary
	for (row = 0; row < rowMax; row++)
	{
		if (inRange(LHpos, note[row]))
		{
			if (onOff[row] == 0)
				song.addStamp(time[row], 0, 0, findFing(LHpos, note[row]));
			else
				song.addStamp(time[row], 0, findFing(LHpos, note[row]), 0);
		}
		else if (inRange(RHpos, note[row]))
		{
			if (onOff[row] == 0)
				song.addStamp(time[row], 1, 0, findFing(RHpos, note[row]));
			else
				song.addStamp(time[row], 1, findFing(RHpos, note[row]), 0);
		}
		else
		{
			//Temporary solution of only the right hand moving
			if (note[row] > RHpos)
			{
				RHpos = note[row] - 12;
				song.createNode(LHpos, RHpos);
				//Only considers RH being out of range for now
				if (onOff[row] == 0)
					song.addStamp(time[row], 1, 0, 7);
				else
					song.addStamp(time[row], 1, 7, 0);
			}
			else
			{
				RHpos = note[row];
				song.createNode(LHpos, RHpos);
				//Only considers RH being out of range for now
				if (onOff[row] == 0)
					song.addStamp(time[row], 1, 0, findFing(RHpos, note[row]));
				else
					song.addStamp(time[row], 1, findFing(RHpos, note[row]), 0);
			}
		}
	}

	//Test of storing values into the file
	/*
	for (row = 0; row < rowMax; row++)
	{
		outFile << time[row] << ", ";
		outFile << onOff[row] << ", ";
		outFile << note[row] << "\n";
	}
	*/

	//song.printTimeline();
	song.createArduino();

	return 0;
}

//Checks how far do I need to move for a note in semitones
int nextNote(int current, int notesToMove)
{
	int pos = current % 12;
	int pos_note;
	int octaves = notesToMove / 12;
	int relative = notesToMove % 7;
	int num;
	int reference[7][7] = 
	{
		{ 0,2,4,5,7,9,11 },
		{ 0,2,3,5,7,9,10 },
		{ 0,1,3,5,7,8,10 },
		{ 0,2,4,6,7,9,11 },
		{ 0,2,4,5,7,9,10 },
		{ 0,2,3,5,7,8,10 },
		{ 0,1,3,5,6,8,10 }
	};
	switch (pos)
	{
	case 0:
		pos_note = 0;
		break;
	case 2:
		pos_note = 1;
		break;
	case 4:
		pos_note = 2;
		break;
	case 5:
		pos_note = 3;
		break;
	case 7:
		pos_note = 4;
		break;
	case 9:
		pos_note = 5;
		break;
	case 11:
		pos_note = 6;
		break;
	default:
		cout << "Included black keys\n";
		break;
	}
	num = reference[pos_note][notesToMove];
	num += octaves * 12;
	return num;
}

//Checks if a note is within range of a hand position
bool inRange(int handPos, int note)
{
	if (note < handPos || note > handPos + 12)
		return 0;
	else
		return 1;
}

//Finds which finger of the in-range note
int findFing(int handPos, int note)
{
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (handPos + nextNote(handPos, i) == note)
			return i + 1;
	}
	return 0;
}

//Calculates how far to move a hand, +ve or -ve
int howFar(int handPos, int note)
{
	int i;
	if (note < handPos)
	{
		for (i = 0; i++; i < 100)
		{
			if (note + nextNote(note, i) == handPos)
				return -i;
		}
	}
	else
	{
		for (i = 0; i++; i < 100)
		{
			if (handPos + 12 + nextNote(handPos + 12, i) == note)
				return i;
		}
	}
}