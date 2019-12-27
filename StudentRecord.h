#ifndef STUDENTRECORD_CLASS
#define STUDENTRECORD_CLASS

using namespace std;

class StudentRecord
{
public:

	StudentRecord(const StudentRecord &p2) { valid = p2.valid; studentID = p2.studentID; *studentName = *p2.studentName; *studentGrades = *p2.studentGrades; }

	bool valid;
	int studentID;
	char studentName[100];
	int studentGrades[3];
	StudentRecord(bool v = false, int ID = 0, const char* name = "", int grade1 = 0, int grade2 = 0, int grade3 = 0);
};

StudentRecord::StudentRecord(bool v, int ID, const char* name, int grade1, int grade2, int grade3)
{
	valid = v;
	studentID = ID;
	strcpy_s(studentName, name);

	studentGrades[0] = grade1;
	studentGrades[1] = grade2;
	studentGrades[2] = grade3;
}



#endif // !STUDENTRECORD_CLASS

