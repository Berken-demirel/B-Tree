#ifndef Student_CLASS
#define Student_CLASS

typedef int StudentId;
typedef int DbIndexForStudent;

class Student
{
public:
	StudentId studentID;
	bool record_valid;
	DbIndexForStudent index;
	Student(int ID = 0, bool valid = false, int i = -1);//constructor

};

Student::Student(int ID , bool valid , int i )
{
	studentID = ID;
	record_valid = valid;
	index = i;
}

inline bool operator <(const Student &lhs, const Student& rhs)
{
	return lhs.index < rhs.index;
}
inline bool operator <=(const Student &lhs, const Student& rhs)
{
	return lhs.index <= rhs.index;
}

inline bool operator >(const Student &lhs, const Student& rhs)
{
	return lhs.index > rhs.index;
}
inline bool operator >=(const Student &lhs, const Student& rhs)
{
	return lhs.index >= rhs.index;
}

inline bool operator ==(const Student &lhs, const Student& rhs)
{
	return lhs.index == rhs.index;
}

#endif 
