// Writing this program I consider that city names such as "Big City" are written in form: "Big_City" - no spaces in name of city.
//
// !!! Every pointer must be set to NULL before use !!!
//
// global_tab:
// [0] - road file loaded?
// [1] - number of named Cities.
// [2] - number of roads.
//
//-----------------------------------------NOTEPAD END---------------------------------------------//

// Includes area:
#include <iostream>													// std. input/output												
#include <cstdlib>													// for exit command and free
#include <string>													// string functions
#include <fstream>													// useful for files
using namespace std;

// Constants:
#define INFINITY 1000000											// Definition of infinity for Dijkstra's algorythm

// Structs:
struct names
{
	int no;
	char city[50];
	names * next;
};

struct graph
{
	int ID;
	int begin;
	int end;
	int dist;
	graph * next;
};

struct dijkstra
{
	int counted;
	int prev;
	int lenght;
};

struct out
{
	int dist;
	char begin[50];
	int begin_ID;
	char end[50];
	int end_ID;
	out * next;
};

//--------------------------------------END OF DEFINITIONS-----------------------------------------//
// General use functions:
void cinflush()														// clears content of input stream.
{
	cin.clear();
	cin.sync();
}

void separator()													// Creates nice separating line.
{
	cout<<"   ----- * ----- * -----   "<<endl;
}

int* global_settings_tab_initialize()								// Initializes array with global settings.
{
	// lenght of tab:
	int global=3;
	// ------ //
	int * global_tab=(int*)malloc(global*sizeof(int));				// Allocates memory for main config array.
	int loop;
	for(loop=0;loop<global;loop++)
	{
		global_tab[loop]=0;											// Fills GT with 0 values (def. as off).
	}
	return global_tab;
}

// Debug mode functions:
int dbg_menu(int mode, names* base,graph*head)						// Additional menu for debug only actions.
{
	names* tmp = base;
	graph* temp = head;
	switch(mode)
	{
		case 0:														// Fast quit.
			system("CLS");
			return 1;
		break;
		case 1:														// Print names.
			system("CLS");
			while(tmp != NULL)
			{
				cout<<(tmp->no)<<' '<<(tmp->city)<<endl;
				tmp = tmp->next;
			}
			return 0;
		break;
		case 2:														// print graph.
			system("CLS");
			while(temp != NULL)
			{
				cout<<(temp->begin)<<' '<<(temp->end)<<' '<<(temp->dist)<<endl;
				temp = temp->next;
			}
			return 0;
		break;
		default:													// Return to main menu.
			system("CLS");
			return 0;
		break;
	}
}

int dbg_face( names* base,graph*head)								// Frontend of debugging menu.
{
	separator();
	int mode;
	cout<<"Debugging menu:"<<endl<<endl;
	cout<<"1) Print elements of names struct,"<<endl;
	cout<<"2) Print elements of graph struct,"<<endl;
	cout<<"0) QUIT app"<<endl;
	cout<<endl;
	cinflush();
	cin>>mode;
	return dbg_menu(mode,base,head);
}
// Structs functions:
void clear_names(names* head)										// Erase names list from memory.
{
	while(head!=NULL)
	{
		names*help=head;
		head=head->next;
		free(help);
	}
}

names* adder_names(names* head, names* elem)						// Adds element to names list.
{
	if(head == NULL)
		head=elem;
	else
	{
		elem->next=head;
		head=elem;
	}
	return head;
}

names* file_load_names(string& file, int& fuse, names* base,int& count)		// Cloects data about ID's and names of cities to save tem in names list.
{
	clear_names(base);												// Clear previous values.
	base=NULL;
	// variables:
	char data[256];													// Temporary container for line of file.
	int city;														// Number of city (readed from file).
	char temp[50];													// Name of city
	int i,j;														// Counters, 'i' is global inline position counter, 'j' counts lenght of part which will be separated.
	names*check;
	// code:
	cout<<"Looking for file...					";
	fstream f;														// Create stream for file.
	f.open(file, ios::in);											// Tie file in read only mode to stream.
	if(f.good() == true)											// Check if file is open properly.
	{
		cout<<"OK"<<endl<<"Creating list of cities...				";
		while(!f.eof())
		{
			names* temp1=(names*)malloc(sizeof(names));				// Create separated element of names list.
			temp1->next=NULL;										// Ensures that 'temp1' points to nothing.
			f.getline(data,256);									// Read line of file to 'data' array.
			i = 0; j=0;												// Rewrites file to structs.
			while(data[i]!=' ')										// ID of city.
			{
				temp[j]=data[i];
				i++;
				j++;
			}
			temp[j]='\0';											// End C type string containing ID.
			city = atoi(temp);										// Convert C string to integer.
			if(city == 0)
			{
				clear_names(base);
				cout<<"FAIL"<<endl<<endl<<"File is broken,"<<endl<<"content mismatches defined structure"<<endl;
				separator();
				return 0;
			}
			temp1->no = city;										// Rewrite ID from temporary variable to names element 'temp1'.
			// --|/|-- //
			check = base;											// Copy of 'names' list.
			while(check != NULL)									// Looking for useing one ID more than once.
			{
				if((temp1->no) == (check->no))
				{
					clear_names(base);
					cout<<"FAIL"<<endl<<endl<<"File is broken,"<<endl<<"multiple use of single ID is forbidden"<<endl;
					separator();
					return 0;
				}
				check = check->next;
			}
			// --|/|-- //
			i++; j=0;												// Set counters.
			while(data[i]!=' ')										// NAME of city.
			{
				temp1->city[j]=data[i];
				i++;
				j++;
				if(data[i] == '\0')
					break;
			}
			temp1->city[j]='\0';									// Ends C string containing name of city.
			base=adder_names(base,temp1);							// Tie element to list.
			count++;
		}
		f.close();													// Close file.
		fuse = 1;													// Set fuse (prevents from visibility of roads dependent menu options.
		return base;												// Return list.
	}
	else															// If file was not found / corrupted / non readable / non compatible, etc.
	{
		cout<<"FAIL"<<endl;
		separator();
		fuse = 0;													// Reset fuse.
		return 0;													// Return error.
	}
}

void clear_graph(graph* head)										// Erase graph list from memory
{
	while(head!=NULL)
	{
		graph*help=head;
		head=head->next;
		free(help);
	}
}

graph* adder_graph(graph* head, graph* elem)						// Adds elements to graph list.
{
	if(head == NULL)
		head=elem;
	else
	{
		elem->next=head;
		head=elem;
	}
	return head;
}

graph* file_load_graph(string& file, int& fuse, graph*head, int& cnt, names*base)	// Reads file and collets data into graph list.
{
	clear_graph(head);												// Clear previous values.
	head=NULL;
	// variables:
	int chk;														// Fuse.
	char data[256];													// Cache for line of file.
	int help;														// Keeps integer data.
	int from;														// Stores starting point of road.
	char temp[50];													// Keeps parts of file line before conversion to integer.
	int i,j;														// Counters,  'i' is global inline position counter, 'j' counts lenght of part which will be separated.
	// code:
	fstream f;														// Create filetype stream.
	f.open(file, ios::in);											// Open file in read only mode.
	if(f.good() == true)											// Check if file is properly open.
	{
		cout<<"OK"<<endl<<"Recreating graph from file...				";
		while(!f.eof())
		{	
			f.getline(data,256);									// Read line from file.
			i = 0; j=0;												// Rewriting file to structures.
			// Skips readed parts of line.
			while(data[i]!=' ')
			{
				temp[j]=data[i];
				i++;
				j++;
			}
			temp[j]='\0';
			from = atoi(temp);										// Creates starting point value.
			if(from == 0)
			{
				clear_names(base);
				cout<<"FAIL"<<endl<<endl<<"File is broken,"<<endl<<"content mismatches defined structure"<<endl;
				separator();
				return 0;
			}
			i++; j=0;
			while(data[i]!=' ')
			{
				i++;
				j++;
				if(data[i] == '\0')
					break;
			}
			i++; j=0;
			// Starts reading into structs and adding them to list
			while(data[i]!='\0')
			{
				graph* temp2=(graph*)malloc(sizeof(graph));			// Create separated element of graph list.
				temp2->next=NULL;									// Ensure that this element points to nothing.
				while(data[i]!=':')
				{
					temp[j]=data[i];
					i++;
					j++;
				}
				temp[j]='\0';
				help = atoi(temp);									// Convert destination ID to integer.
				if(help == 0)
				{
					clear_names(base);
					cout<<"FAIL"<<endl<<endl<<"File is broken,"<<endl<<"content mismatches defined structure"<<endl;
					separator();
					return 0;
				}
				temp2->end = help;									// Write destination to structure
				i++; j=0;
				while(data[i]!=' ')
				{
					temp[j]=data[i];
					i++;
					j++;
					if(data[i] == '\0')
						break;
				}
				temp[j]='\0';
				help = atoi(temp);									// Converts distance to integer.
				if(help == 0)
				{
					clear_names(base);
					cout<<"FAIL"<<endl<<endl<<"File is broken,"<<endl<<"content mismatches defined structure"<<endl;
					separator();
					return 0;
				}
				if(help<0)
				{
					cout<<"FAIL"<<endl<<endl<<"Lenght of road can't be less than 0!"<<endl;
					separator();
					fuse = 0;										// Reset fuse.
					return 0;										// Return error.
				}
				temp2->dist = help;									// Write destination to structure.
				temp2->begin = from;								// Write starting point to structure.
				temp2->ID = cnt;									// Useful for dijkstra alg.
				chk = 0;
				names*bak = base;
				while(bak!=NULL)									// Check if begin and end exists in 'names'.
				{
					if((bak->no)==temp2->begin)
					{
						chk++;
					}
					if((bak->no)==temp2->end)
					{
						chk++;
					}
					bak = bak->next;
				}
				if(chk != 2)
				{
					cout<<"FAIL"<<endl<<endl<<"Graph contains nonexisting cities"<<endl;
					separator();
					fuse = 0;										// Reset fuse.
					return 0;
				}
				head = adder_graph(head,temp2);						// Tie structure to list.
				cnt++;
				if(data[i]!='\0')									// If it's not end of line, then set counters.
					{i++; j=0;}										
			}
		}
		f.close();													// Close file.
		fuse = 1;													// Ensure that fuse is properly set.
		cout<<"OK"<<endl;
		separator();
		cout<<endl;
		return head;												// Return list.
	}
	else															// If file is broken, theoreticaly it should be right if this function was called, ->
	{																// but sometimes strange things happens.
		cout<<"FAIL"<<endl;
		separator();
		fuse = 0;													// Reset fuse.
		return 0;													// Return error.
	}
}

// Dijkstra functions:
void clear_out(out* head)										// Erase names list from memory.
{
	while(head!=NULL)
	{
		out*help=head;
		head=head->next;
		free(help);
	}
}

out* adder_out(out* head, out* elem)							// Adds element to names list.
{
	if(head == NULL)
		head=elem;
	else
	{
		elem->next=head;
		head=elem;
	}
	return head;
}

out * flister(out*head, int start, int stop, dijkstra* data, names * base,graph * list,int last, char to[50])
{
	names*bak;
	graph*test = list;
	while((test->ID) != last)
	{
		test = test->next;
	}
	char tempstr[50];
	int counter;
	counter = last;
	strcpy_s(tempstr,to);
	while(true)
	{
		// reset variables:
		bak = base;
		// code:
		out * temp = (out*)malloc(sizeof(out));
		strcpy_s((temp->end),tempstr);
		(temp->end_ID)=(test->end);
		(temp->dist)=(test->dist);
		test = list;
		while((test->ID) != (data[counter].prev))
		{
			if((test->next) != NULL)
			{
				test = test->next;
			}
			else
			{
				while(bak!=NULL)												// Check if begin and end has translation from int to string.
				{
					if((bak->no)==start)
					{
						strcpy_s((temp->begin),(bak->city));
						(temp->begin_ID)=start;
						temp->next = NULL;
						head = adder_out(head,temp);
						return head;
					}
				bak = bak->next;
				}
			}
		}
		while(bak!=NULL)												// Check if begin and end has translation from int to string.
		{
			if((bak->no)==(test->end))
			{
				strcpy_s(tempstr,(bak->city));
				counter = data[counter].prev;
			}
			bak = bak->next;
		}
		strcpy_s((temp->begin),tempstr);
		(temp->begin_ID)=(test->end);
		temp->next = NULL;
		head = adder_out(head,temp);
	}
}

void list(names* base, int count);									// Separated header of interface function, placed here due to use in 'fdijkstra'.

void out_text(int mode,names*base,graph*head,dijkstra*data,int last,int max,char from[50],char to[50],int begin,int end)
{
	names*worker=base;												// Copy of list for safety.
	graph*temp=head;												// Copy of list for safety.
	if(mode==0)
	{
		// Route from one city to itself:
		cout<<"Route from "<<from<<" ("<<begin<<") to "<<to<<" ("<<end<<") has no lenght."<<endl;;
		cout<<"Both ends of your route are one city."<<endl;
	}
	else
	{
		if(mode==1)
		{
			// Route can't be calculated:
			cout<<"Route from "<<from<<" ("<<begin<<") to "<<to<<" ("<<end<<") can't be calculated."<<endl;
		}
		else
		{
			cout<<"Route from "<<from<<" ("<<begin<<") to "<<to<<" ("<<end<<") was calculated."<<endl;
			cout<<"Lenght of this road is equal to: "<<data[last].lenght<<" km"<<endl<<endl;
			cout<<"Your shortest road:"<<endl;
			out * outlist = NULL;
			outlist = flister(outlist,begin,end,data,base,head,last,to);
			while(outlist!=NULL)
			{
				cout<<(outlist->begin)<<" ("<<(outlist->begin_ID)<<") ---> "<<(outlist->end)<<" ("<<(outlist->end_ID)<<") = "<<(outlist->dist)<<" km"<<endl;
				outlist = outlist->next;
			}
			clear_out(outlist);
		}
	}
}

void short_road(int begin, int end, graph*head, names* base, dijkstra*data, int length)
{
	// Precalculations check-in:
	system("CLS");
	char from[50];
	char to[50];
	names*bak = base;
	int test = 0;
	while(bak!=NULL)												// Check if begin and end has translation from int to string.
	{
		if((bak->no)==begin)
		{
			strcpy_s(from,(bak->city));
			test++;
		}
		if((bak->no)==end)
		{
			strcpy_s(to,(bak->city));
			test++;
		}
		bak = bak->next;
	}
	if(test != 2)
	{
		cout<<"Route don't have specified begin or end in file!"<<endl;
		return;
	}
	// Variables:
	int i;															// counter
	int count=0;													// Counts number of order change
	int fuse;														// Checks for order change.
	int dist=INFINITY;												// For searching shortest path
	int order=length+1;												// Points to previous table value
	int start;														// Back-up copy of oryginal begin point.
	graph*worker=head;												// Copy of list for safety.
	// Populating data table:
	for(i=0;i<=length;i++)
	{
		data[i].lenght=INFINITY;									// Distance from start (INFINTITY = not counted / not exist)
		data[i].prev=-1;											// Previously used road
		data[i].counted=0;											// Route counted indicator
	}
	data[order].lenght=0;											// Route from begin to begin, treated as starting point for Dijkstra algorythm
	data[order].prev=order;
	data[order].counted=1;
	// Code:
	start = begin;
	if(begin == end)												// For counting route from CityA to itself
	{
		// Route from A to A;
		out_text(0,base,head,data,0,(dist+1),from,to,begin,end);
	}
	else
	{
 		while(true)													// Infinite loop
		{
			while(worker!=NULL)										// Loop for calculating route between start (currently pointed city) and it's neighbours
			{
				if((worker->begin)==start)							// looking for neighbors
				{
					if(data[worker->ID].lenght>data[order].lenght+worker->dist)
					{
						data[worker->ID].lenght=data[order].lenght+worker->dist;
						data[worker->ID].prev=order;
					}
				}
			worker=worker->next;
			}
			worker=head;											// Reset of list order
			fuse=0;													// Reset of fuse
			while(worker!=NULL)										// Check if all posible routes are counted
			{
				if(data[worker->ID].counted==0)						// move "start" to another city (end of counted route)
				{
					if(data[worker->ID].lenght<dist)
					{
						dist=data[worker->ID].lenght;
						order=worker->ID;
						start=(worker->end);
						fuse++;										// Shut down fuse
					}
				}
				worker=worker->next; 
			}
			count++;												// Counter of counted routes
			data[order].counted=1;									// Set route (in array) as counted;
			worker=head;											// Reset of list order
			dist=INFINITY;											// Reset dist to INFINITY, to make possible calculation of shortest road;
			if(count==(length+1))									// If all possible routes are counted, then do:
			{
				fuse = 0;											// Reset of fuse
				while(worker!=NULL)									// "Find shortest way" loop
				{
					if((worker->end)==end)
					{
						if(data[worker->ID].lenght<dist)
						{
							dist=data[worker->ID].lenght;
							order=worker->ID;
							fuse++;									// Shut down fuse
						}
					}
				worker=worker->next;
				}
				if(fuse==0)											// Route don't exist
				{
					// There is no such route.
					out_text(1,base,head,data,0,(dist+1),from,to,begin,end); 
					break;
				}
				worker=head;
				//Route exists and was calculted, SHOW IT.
				out_text(2,base,head,data,order,(dist+1),from,to,begin,end);
				break;
			}
		}
	}
}

void fdijkstra(graph*head, names* base, int count, int cnt)
{
	// Variables:
	int begin;
	int end;
	// Code:
	// Get data:
	system("CLS");
	list(base,count);												// Show list of cities from file.
	separator();
	cout<<"Give ID of start of your route:"<<endl;
	cinflush();
	cin>>begin;
	cout<<"Give ID of end of your route:"<<endl;
	cinflush();
	cin>>end;
	// Allocation of struct data array:
	dijkstra*data=(dijkstra*)malloc((cnt+2)*sizeof(dijkstra));		// array for storage of dijkstra temporary outputs.
	// Call route counting function.
	short_road(begin,end,head,base,data,cnt);
	free(data);
}

// Interface functions:
void list(names* base, int count)
{
	names* list = NULL;
	names* temp = base;
	while(temp!=NULL)
	{
		names* temp1=(names*)malloc(sizeof(names));					// Create separated element of names list.
		temp1->no = temp->no;
		strcpy_s((temp1->city),(temp->city));
		temp1->next=NULL;
		list = adder_names(list,temp1);
		temp=temp->next;
	}
	cout<<"ID:	Name:"<<endl;
	while(list!=NULL)
	{
		cout<<(list->no)<<"	"<<(list->city)<<endl;
		list = list->next;
	}
	clear_names(list);
}

void help()
{
		system("CLS");
		cout<<"If you are lost, it should help you."<<endl;
		cout<<"Welcome in >> Route Finder << manual."<<endl<<endl;
		cout<<"To calculate routes you have to provide file containing roads, in form:"<<endl;
		cout<<"<city_ID> <City Name> <ID>:<Distance> ..., for instance such as:"<<endl<<endl;
		cout<<"1 London 2:40 3:25 7:950"<<endl<<endl;
		cout<<"Remember that:"<<endl<<"1) each line of file can only contain definition of one city,"<<endl;
		cout<<"2) there is no special character at the end of line,"<<endl;
		cout<<"3) File should be saved plain text - best tool is notepad."<<endl<<endl;
		cout<<"When you set path to your file in 1st option in main menu,"<<endl<<"rest of programm options will activate."<<endl;
		cout<<"If file is in same folder as exe file, you can use only name of file,"<<endl<<"if not you have to use full path."<<endl;
}

void intro()														// Shows welcome message when programm was run.
{
	cout<<"Welcome in route finder."<<endl<<endl;
	cout<<"Copyright by Michal Urbaniak 2014"<<endl<<endl<<"----------"<<endl<<endl;
}

int menu(int mode, int& fuse, string& file, names* base,graph*head,int count,int cnt)	// Main menu switch
{
	string adress;
	switch(mode)
	{
		case 0:														// Quit
			return 1;
		break;
		case 1:														// Set file and load graph to memory.
			system("CLS");
			cout<<"Where is road file?"<<endl<<endl;
			cinflush();
			cin>>adress;
			file = adress;											// Associate captured file path to variable.
			return 10;
		break;
		case 2:														// Show list.
			system("CLS");
			list(base,count);										// Call reversing and printing function.
			separator();
			return 0;
		break;
		case 3:														// Calculate route.
			system("CLS");
			fdijkstra(head,base,count,cnt);
			separator();
			return 0;
		break;
		case 10:													// Help.
			help();
			cout<<endl;
			return 0;
		break;
		case 2603:													// Debug menu.
			system("CLS");
			return dbg_face(base,head);
		break;
		default:													// In case of improper mode, return to menu.
			system("CLS");
			cout<<"You've choosed improper option, try again:"<<endl<<endl;
			separator();
			return 0;
		break;
	}
}

int face(int &fuse,string& file, names* base,graph*head,int count, int cnt)
{
	int mode;
	cout<<"Main menu:"<<endl<<endl<<"Please choose what you want to do:"<<endl<<endl;
	cout<<"1) Choose input file,"<<endl;
	if(fuse == 1)
	{
		cout<<"2) Print cities list,"<<endl;
		cout<<"3) Calculate route,"<<endl;
	}
	cout<<"10) Show manual,"<<endl;
	cout<<"0) EXIT."<<endl<<endl;
	cinflush();														// Clear stream.
	cin>>mode;														// Obtain mode switch from user.
	return menu(mode,fuse,file,base,head,count,cnt);				// Call menu and return it's state.
}

// main function area:
int main()
{
	// Variables
	names* base=NULL;												// Create place for 'names' list.
	graph* head=NULL;												// Create place for 'graph' list.
	int execution_loop=0;											// State holder for main loop.
	int * global_tab=NULL;											// Create place for 'global_tab'.
	string file;													// Create place for path to file.
	
	//Code
	global_tab=global_settings_tab_initialize();					// Initialize 'global_tab'.
	intro();														// Show intro message
	// ----- //
	while(execution_loop == 0)										// Loop for main menu and called functions:
	{
		execution_loop=face(global_tab[0],file,base,head,global_tab[1],global_tab[2]);	// Call menu, and use it returned value for control of main loop.
		if(execution_loop==10)										// For file reading case, must be called outside 'menu' function due to returning of structs.
		{
				base=file_load_names(file,global_tab[0],base,global_tab[1]);	// Create list of city names.
				if(base!=0)											// If list of cities is not empty.
				head=file_load_graph(file,global_tab[0],head,global_tab[2],base);	// Create list of avalible roads.
				execution_loop=0;									// Restore normal state.
		}
	}
	clear_names(base);												// Free memory allocated by 'names' list.
	clear_graph(head);												// Free memory allocated by 'graph' list.
	free(global_tab);												// Free memory allocated by 'global_tab'
	return 0;														// Proper end of programm.
}