#include <iostream>
#include<string>
using namespace std;


void menu() {

	int choice;
	cout << "-------------Welcome To Electronic Dictionary-------------"<<endl<<endl;
	
	cout << "  ENTER YOUR CHOICE   " <<endl<<endl;
	cout << "1.Meaning of the Word" << endl;
	cout << "2.Synonym of the Word" << endl;
	cout << "3.Part of the Speech" << endl;
	cout << "4.Example Sentence" << endl;
	cout << "5.Word of the Day" << endl;
	cout << "6.Search history of the words" << endl;
	cout << "7. Exit" << endl;
	cin >> choice;

	switch (choice)
	{
	case 1:
			break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	default:
		cout << "Wrong Choice!";
		break;
	}

}
void main() {

	menu();

}
