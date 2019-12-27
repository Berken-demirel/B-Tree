#include "pch.h"
#include <iostream>
#include <sstream>
#include <string>

#include <stdlib.h>

using namespace std;

#include "Student.h"
#include "BTreeNode.h"
#include "StudentRecord.h"

const int nMaxStudentsInDB = 100;
const int nTestStudentCount = 15;


typedef BTreeNode<StudentId, DbIndexForStudent> TreeNodeStudent; //Nodes 

typedef BalancedTree <StudentId, DbIndexForStudent> TreeStudents; //Tree which is composed of BTreeNode 


void Insert_to_StudentRecord_Database(StudentRecord &lfh, StudentRecord &rhs);
bool Check_Student_is_in_DB(StudentRecord &new_coming, StudentRecord *Database);
bool Add_Student_To_DB(StudentRecord &new_coming, StudentRecord *Database, int &nPlacedIndex);

/*
int searchStudent(int ID, BtreeNode<Student>* Tree, bool& valid);
void PrintTree (BtreeNode<Student>* Tree)
void PrintStudent(int ID, StudentRecord* Database, BtreeNode<Student>* Tree);
void UpdateStudentGrades(int ID, int * grades, StudentRecord*Database, BtreeNode<Student>* Tree);
void DeleteStudent(int ID, StudentRecord* Database,BtreeNode<Student>* Tree);
void List(BtreeNode<Student>* Tree);
int searchStudent(int ID, BtreeNode<Student>* Tree, bool& valid);
*/

// -------------------------------Global Functions --------------------------------
int searchStudent(int ID, TreeNodeStudent* Tree, bool& valid);
void List(TreeNodeStudent* Tree);
void PrintStudent(int ID, StudentRecord* Database, TreeNodeStudent* Tree);
void PrintTree(TreeNodeStudent* Tree);
void InsertStudent(int ID, char* Name, int* grades, StudentRecord* Database, TreeNodeStudent*& Tree);
void UpdateStudentGrades(int ID, int * grades, StudentRecord*Database, TreeNodeStudent* Tree);
// -------------------------------Global Functions --------------------------------


struct StudentDB
{
	StudentRecord*	m_Database;
	int			m_RecordCount;
	int			m_MaxRecords;

	StudentDB() :
		m_Database{nullptr}, m_RecordCount{0}, m_MaxRecords{0}
	{
	}

	StudentRecord* GetDB()
	{
		return m_Database;
	}

	void Initialize(int nMaxCount)
	{
		Cleanup();
		m_Database = new StudentRecord[nMaxCount];
		m_MaxRecords = nMaxCount;
		m_RecordCount = 0;
	}

	void Cleanup()
	{
		delete[] m_Database;
		m_Database = 0;
		m_MaxRecords = m_RecordCount = 0;
	}

	~StudentDB()
	{
		Cleanup();
	}
};



int main()
{
	TreeNodeStudent *Student_Info_Tree = 0;

	TreeStudents tree{ Student_Info_Tree, true };

	// Create an array of 100 studentRecord objects for Database
	StudentDB	studentDB;
	studentDB.Initialize(nMaxStudentsInDB);

	// Create a test database
	for (int n = 0; n < nTestStudentCount; ++n)
	{
		stringstream ss;
		ss << "Name" << n;

		string name = ss.str();

		const int nStudentID = rand() % 100 + 1; // Generate random students' ID

		int anGrades[3] = { n, n, n };
		StudentRecord* Student = new StudentRecord(true, nStudentID, (char*)name.c_str(), n, n, n);

		cout << (char*)name.c_str() << " id: " << nStudentID << endl;

		InsertStudent(nStudentID, (char*)name.c_str(), anGrades, studentDB.GetDB(), tree.GetRootRef());
	}
	 

	List(tree.GetRootRef());

	bool valid = false;

	int check;

	check = searchStudent(18, tree.GetRootRef(), valid);
	
    return 0;
}

//---------------------------------------------------------------------------------------
// -------- Global functions implementation---------------- START
//---------------------------------------------------------------------------------------
void PrintTree(TreeNodeStudent* Tree)
{
	TreeStudents tree{ Tree, false };
	tree.print();
};

int searchStudent(int ID, TreeNodeStudent* Tree, bool& valid)
{
	TreeStudents tree{ Tree, false };
	int foundID;
	foundID = tree.search(ID, tree.GetRootRef());
	valid = (foundID == 0) ? false : true;
	if (valid == false)
		return -1;
	else
		return foundID;
};

void InsertStudent(int ID, char* Name, int* grades, StudentRecord* Database, TreeNodeStudent*& Tree)
{
	StudentRecord* Student_to_insert = new StudentRecord(true, ID, Name, grades[0], grades[1], grades[2]); // Create a StudentRecord for database

	//Check the whether student has already in the list or not
	if (Check_Student_is_in_DB(*Student_to_insert, Database))
	{
		cerr << "Student has already in the list" << endl;
		return;
	}
	
	int nPlacedIndex; //Index information
	bool fInsertOK = Add_Student_To_DB(*Student_to_insert, Database, nPlacedIndex);
	if (!fInsertOK)
	{
		//No more space in DB
		cerr << " There is no empty space in Database";
		return;
	}

	Student Student_1_tree{ ID, true, nPlacedIndex }; // Create a Student object for inserting Tree

	TreeStudents tree{ Tree, false };

	tree.insert(ID, nPlacedIndex); // Insert the Student to three with the ID and index values.

	tree.print(); // Prints the tree in breadthfirst order
}

void UpdateStudentGrades(int ID, int * grades, StudentRecord*Database, TreeNodeStudent* Tree)
{
	TreeStudents tree{ Tree, false };
	bool valid = false;
	int foundIndex;
	foundIndex = searchStudent(ID, tree.GetRootRef(), valid);
	
	if (!valid) // Student couldn't found in Tree
	{
		cerr << "No student in database" << endl; // Give error message
	}
	// Otherwise update student grades
	else
	{
		Database->studentGrades[0] = grades[0];
		Database->studentGrades[1] = grades[1];
		Database->studentGrades[2] = grades[2];
	}
}

void PrintStudent(int ID, StudentRecord* Database, TreeNodeStudent* Tree)
{
	TreeStudents tree{ Tree, false };
	bool valid = false,
	foundIndex = searchStudent(ID, tree.GetRootRef(), valid);

	if (!valid) // Student couldn't found in Tree
	{
		cerr << "No student in database" << endl; // Give error message
	}
	else
	{
		cout << "Student name: " << Database[foundIndex].studentName << endl;
		cout << "Grade 0: " << Database[foundIndex].studentGrades[0] << endl;
		cout << "Grade 1: " << Database[foundIndex].studentGrades[1] << endl;
		cout << "Grade 2: " << Database[foundIndex].studentGrades[2] << endl;
	}
}

void List(TreeNodeStudent* Tree)
{
	TreeStudents tree{ Tree, false };

	tree.List(tree.GetRootRef());
}
//---------------------------------------------------------------------------------------
// -------- Global functions implementation---------------- FINISH
//---------------------------------------------------------------------------------------


// Add a student to database, change the value of nPlacedIndex in order to assign index value for creating tree
bool Add_Student_To_DB(StudentRecord &new_coming, StudentRecord *Database, int &nPlacedIndex)
{
	for (int i = 0; i < nMaxStudentsInDB; i++)
	{
		if (!Database[i].valid)
		{
			nPlacedIndex = i;
			Database[nPlacedIndex] = new_coming;
			return true;
		}
	}
	return false;
}
// Function in order to check whether the student has already in the Database or not
bool Check_Student_is_in_DB(StudentRecord &new_coming, StudentRecord *Database)
{
	for (int i = 0;i < nMaxStudentsInDB; i++)
	{
		if ( (Database[i].valid) == true && Database[i].studentID == new_coming.studentID)
		{
			return true;
		}
	}
	return false;
}

// Insert students to Database arrays
void Insert_to_StudentRecord_Database(StudentRecord &lfh, StudentRecord &rhs)
{
	lfh.studentGrades[0] = rhs.studentGrades[0];
	lfh.studentGrades[1] = rhs.studentGrades[1];
	lfh.studentGrades[2] = rhs.studentGrades[2];

	lfh.valid = rhs.valid;
	strcpy_s(lfh.studentName, rhs.studentName);

	lfh.studentID = rhs.studentID;
}

