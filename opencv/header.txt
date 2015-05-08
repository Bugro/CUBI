using namespace cv;
using namespace std;

struct Bille
{
	int x;
	int y;
	string color; //N (noir), B(blanc), J(jaune) ou R(rouge)
};

struct Droite{
	int a;
	int b;
	int c;
};

struct Segment{
	Droite d;
	Point p1;
	Point p2;
};

struct Vecteur {
	int x;
	int y;
};