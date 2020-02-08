#include "List.h"
List::List()
{
	head = nullptr;
	tail = nullptr;
}
List::~List() {}
void List::createNode(int LHposition, int RHposition)
{
	node* temp = new node;
	temp->LHpos = LHposition;
	temp->RHpos = RHposition;
	temp->head = nullptr;
	temp->tail = nullptr;
	if (head == nullptr)
	{
		head = temp;
		tail = temp;
		tail->next = nullptr;
		temp = nullptr;
	}
	else
	{
		tail->next = temp;
		tail = temp;
		tail->next = nullptr;
	}
}
void List::addStamp(int time, int hand, int fingOn, int fingOff)
{
	stamp* temp = new stamp;
	temp->time = time;
	//Set hand's on/off fingers
	if (hand == 0)
	{
		temp->LHfingOn = fingOn;
		temp->LHfingOff = fingOff;
		temp->RHfingOn = 0;
		temp->RHfingOff = 0;
	}
	else
	{
		temp->RHfingOn = fingOn;
		temp->RHfingOff = fingOff;
		temp->LHfingOn = 0;
		temp->LHfingOff = 0;
	}

	temp->next = nullptr;
	if (tail->head == nullptr)
	{
		tail->head = temp;
		tail->tail = temp;
		temp = nullptr;
	}
	else
	{
		tail->tail->next = temp;
		tail->tail = temp;
	}
}

void List::printTimeline(void)
{
	node* ptr = new node;
	ptr = head;
	stamp* temp = new stamp;
	while (ptr != nullptr)
	{
		temp = ptr->head;
		cout << "Hand position LH/RH: " << ptr->LHpos << " " << ptr->RHpos << endl;
		while (temp != nullptr)
		{
			cout << "Time (microseconds): " << temp->time << endl;
			cout << "LH on/off: " << temp->LHfingOn << " " << temp->LHfingOff << endl << "RH on/off: " << temp->RHfingOn << " " << temp->RHfingOff << endl;
			temp = temp->next;
		}
		ptr = ptr->next;
	}
}

void List::createArduino(void)
{
	int preLH = 36;
	int preRH = 60;
	int preTime = -1;
	int start = 1;
	node* ptr = new node;
	ptr = head;
	stamp* temp = new stamp;
	outFile.open("pianocontrol.txt", ios::out);
	if (!outFile)
	{
		cout << "Unable to open file";
		exit(1); // terminate with error
	}

	while (ptr != nullptr)
	{
		outFile << "H";
		if (ptr->LHpos - preLH > 0)
			outFile << "1_" << ptr->LHpos - preLH;
		else if (ptr->LHpos - preLH < 0)
			outFile << "0_" << preLH - ptr->LHpos;
		else
			outFile << "0_0";
		outFile << "h";
		if (ptr->RHpos - preRH > 0)
			outFile << "1_" << ptr->RHpos - preRH;
		else if (ptr->RHpos - preRH < 0)
			outFile << "0_" << preRH - ptr->RHpos;
		else
			outFile << "0_0";

		temp = ptr->head;
		while (temp != nullptr)
		{
			if (preTime != temp->time || start == 1)
			{
				preTime = temp->time;
				outFile << "t" << preTime;
				start = 0;
			}
			outFile << "f";
			if (temp->LHfingOn != 0)
				outFile << "0_" << "1" << temp->LHfingOn;
			else if (temp->LHfingOff != 0)
				outFile << "0_" << "0" << temp->LHfingOff;
			else if (temp->RHfingOn != 0)
				outFile << "1_" << "1" << temp->RHfingOn;
			else if (temp->RHfingOff != 0)
				outFile << "1_" << "0" << temp->RHfingOff;
			temp = temp->next;
		}
		start = 1;
		outFile << endl;
		preLH = ptr->LHpos;
		preRH = ptr->RHpos;
		ptr = ptr->next;
	}
}

int List::howFar(int handPos, int note)
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

int List::nextNote(int current, int notesToMove)
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
