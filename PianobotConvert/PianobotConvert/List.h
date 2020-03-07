#ifndef   __List__
#define   __List__
#include <iostream>
#include <fstream>

struct stamp
{
	int time;
	int LHfingOn;
	int LHfingOff;
	int RHfingOn;
	int RHfingOff;
	stamp* next;
};

struct node
{
	int LHpos;
	int RHpos;
	stamp* head;
	stamp* tail;
	node* next;
};

using namespace std;
class List
{
private:
	node* head;
	node* tail;
	//Set up output file to input into Arduino
	fstream outFile;
public:
	List();
	~List();
	void createNode(int LHposition, int RHposition);
	void addStamp(int time, int hand, int fingOn, int fingOff);
	void printTimeline(void);
	void createArduino(void);
	int howFar(int handPos, int note);
	int nextNote(int current, int notesToMove);
};
#endif