#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include "Main.h"

#define NBR	2

#define PI 3.14159265

Point posBlanche;

int maxCanneX = 640;
int minCanneX = 0;
int maxCanneY = 480;
int minCanneY = 0;

int ecartCanne = 100;

int coefDirCanne = 0;
int ordOrigCanne = 0;

int compteurDePoint = 0;
int compteurDeCouleur = 0;
int compteurDeLigne = 0;

Rect myROI, ROICanne;

vector<Vec4i> lines;

Mat matRecadrer, matRotatedFrame, blam, dst, cdst;

int thresholdCanny = 9;
int thresholdCenter = 16;

int minLineLengh = 5;
int maxLineGap = 3;

int minRed = 120;
int maxRed = 130;
int maxNoire = 120;
int maxBlanche = 40;
int minJaune = 90;
int maxJaune = 100;

//Paramêtres des cercles 
const int DIST_MIN = 5;
const int MIN_SIZE = 5;
const int MAX_SIZE = 10;

const string curseurWindowName = "curseurs";

Droite droite(Point p1, Point p2){


	Droite r;
	r.a = p1.y - p2.y;

	r.b = p2.x - p1.x;

	r.c = -p1.y*p2.x + p1.x*p2.y;

	return r;
}

Segment segment(Point p1, Point p2){


	Segment r;
	r.d = droite(p1, p2);
	r.p1 = p1;
	r.p2 = p2;

	return r;
}

Point croisement(Droite d1, Droite d2);

bool appartient(Segment s, Point p);

bool intersection(Point2i o1, Point2i p1, Point2i o2, Point2i p2, Point2i &r)
{
	Point2i x = o2 - o1;
	Point2i d1 = p1 - o1;
	Point2i d2 = p2 - o2;
	Point2i v;

	float cross = d1.x*d2.y - d1.y*d2.x;
	double t1 = (x.x * d2.y - x.y * d2.x) / cross;
	v = o1 + d1 * t1;
	if ((abs(cross) < /*EPS*/1e-8) || (v.x < min(o2.x, p2.x)) || (v.x > max(o2.x, p2.x)) || (v.y < min(o2.y, p2.y)) || (v.y > max(o2.y, p2.y)))
	{
		return false;
	}
	else
	{
		r = v;
		return true;
	}
}

int pscalaire(Point p1, Point p2, Point p3, Point p4);

Point sym(Segment s, Point a);

bool getPerpendiculaire(Point2i A, Point2i B, Point2i &r)
{
	Point v;
	v.x = B.x - A.x; v.y = B.y - A.y;
	float mag;
	mag = sqrt(float(v.x*v.x + v.y*v.y)); v.x = v.x / mag; v.y = v.y / mag;
	int temp;
	temp = v.x; v.x = -v.y; v.y = temp;
	int length = 10;
	Point C;
	C.x = B.x + v.x * length; C.y = B.y + v.y * length;
	if (mag == 0)
	{
		return false;
	}
	else
	{
		r = C;
		return true;
	}
}

void on_curseur(int, void*)
{

}

void createcurseurs(){
	//crée une fenêtre de curseurs


	namedWindow(curseurWindowName, 0);
	//crée de la mémoire pour stocker les noms des curseurs
	char curseurName[50];
	sprintf(curseurName, "UpperCanny", thresholdCanny);
	sprintf(curseurName, "CenterDetection", thresholdCenter);

	sprintf(curseurName, "Min Red", minRed);
	sprintf(curseurName, "Max Red", maxRed);
	sprintf(curseurName, "Max Noire", maxNoire);
	sprintf(curseurName, "Max Blanche", maxBlanche);
	sprintf(curseurName, "Min Jaune", minJaune);
	sprintf(curseurName, "Max Jaune", maxJaune);


	createTrackbar("UpperCanny", curseurWindowName, &thresholdCanny, 50, on_curseur);
	createTrackbar("CenterDetection", curseurWindowName, &thresholdCenter, 50, on_curseur);


	createTrackbar("Min Red", curseurWindowName, &minRed, 255, on_curseur);
	createTrackbar("Max Red", curseurWindowName, &maxRed, 255, on_curseur);
	createTrackbar("Max Noire", curseurWindowName, &maxNoire, 255, on_curseur);
	createTrackbar("Max Blanche", curseurWindowName, &maxBlanche, 255, on_curseur);
	createTrackbar("Min Jaune", curseurWindowName, &minJaune, 255, on_curseur);
	createTrackbar("Max Jaune", curseurWindowName, &maxJaune, 255, on_curseur);

}

vector<Point> algo(const Point coin[12], const Point boules[15], const Point blanche, const Point cane[2]);

//create a table of 8 points
Point rectangleBillard[12]; // Ensemble de points définissant les bandes du billard
int cmptClics = 12; //Ledit compteur

//tableau des 4 points moyens
Point rectMoy[4];

//La canne
Point cannePoint[2];

// Fonction pour récupérer les actions de la souris
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN) // Lorsqu'on clique on crée des points
	{
		if (cmptClics>0) //Test du nombre de clics effectués
		{
			//cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
			rectangleBillard[compteurDePoint].x = x;
			rectangleBillard[compteurDePoint].y = y;
			cout << "Le point numéro " << compteurDePoint << " a pour position (" << rectangleBillard[compteurDePoint].x << ", " << rectangleBillard[compteurDePoint].y << ")." << endl;
			compteurDePoint++;
			cmptClics--;
		}
	}
}

int main(int argc, char* argv[])
{
	posBlanche.x = 0;
	posBlanche.y = 0;
	//Initialisation de rectangleBillard (les bors du billards sont initialisés à des valeurs plausibles
	rectangleBillard[0].x = 85;
	rectangleBillard[0].y = 309;
	rectangleBillard[1].x = 58;
	rectangleBillard[1].y = 102;
	rectangleBillard[2].x = 80;
	rectangleBillard[2].y = 74;
	rectangleBillard[3].x = 287;
	rectangleBillard[3].y = 69;
	rectangleBillard[4].x = 349;
	rectangleBillard[4].y = 68;
	rectangleBillard[5].x = 540;
	rectangleBillard[5].y = 61;
	rectangleBillard[6].x = 575;
	rectangleBillard[6].y = 84;
	rectangleBillard[7].x = 571;
	rectangleBillard[7].y = 257;
	rectangleBillard[8].x = 560;
	rectangleBillard[8].y = 305;
	rectangleBillard[9].x = 358;
	rectangleBillard[9].y = 319;
	rectangleBillard[10].x = 294;
	rectangleBillard[10].y = 316;
	rectangleBillard[11].x = 103;
	rectangleBillard[11].y = 329;
	myROI = cvRect(rectangleBillard[0].x, rectangleBillard[2].y, rectangleBillard[7].x - rectangleBillard[0].x, rectangleBillard[8].y - rectangleBillard[2].y);

	// Chargement de la vidéo pour lecture
	//todo insérer en paramètre en début de programme
	VideoCapture cap("Capture_20150204_1.mp4");

	// En cas d'écherc, fin du programme
	if (!cap.isOpened())
	{
		cout << "Impossible d'ouvrir le fichier vidéo" << endl;
		return -1;
	}

	const char* pzOriginalWindowName = "Original Video, Définissez les bandes ici";
	namedWindow(pzOriginalWindowName, CV_WINDOW_AUTOSIZE);

	const char* pzRotatingWindowName = "Rotated Video";
	//namedWindow( pzRotatingWindowName, CV_WINDOW_AUTOSIZE );

	const char* pzCirclesWindowName = "Circles Video";
	//namedWindow( pzCirclesWindowName, CV_WINDOW_AUTOSIZE );

	//Create the curseurs window
	createcurseurs();

	double iAngle = 180;
	double iAngle1 = 180;
	double iAngle2 = 180;
	double iAngle3 = 180;
	double iAngle4 = 180;

	while (true)
	{
		Mat matOriginalFrame;

		// Lecture d'une nouvelle image de la vidéo
		bool bSuccess = cap.read(matOriginalFrame);

		// En cas d'échec, fin du programme
		if (!bSuccess)
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		imshow(pzOriginalWindowName, matOriginalFrame);

		//set the callback function for any mouse event
		setMouseCallback(pzOriginalWindowName, CallBackFunc, NULL); //On choisi la fenêtre sur laquelle on travail

		//On affiche la fenêtre recadrée que lorsqu'on a fini de cliquer les 12 points
		if (compteurDePoint > 11)
		{
			/*
			// évite que le calcul des points soit fait pour chaque image après la saisie du 8eme point
			if(compteurDePoint == 12)
			{
			// calcul des 4 points moyens à partir des 8 cliqués

			//pour les x
			rectMoy[0].x = (rectangleBillard[0].x + rectangleBillard[1].x) / 2;
			rectMoy[1].x = (rectangleBillard[2].x + rectangleBillard[3].x + rectangleBillard[4].x + rectangleBillard[5].x) / 4;
			rectMoy[2].x = (rectangleBillard[6].x + rectangleBillard[7].x) / 2;
			rectMoy[3].x = (rectangleBillard[8].x + rectangleBillard[9].x + rectangleBillard[10].x + rectangleBillard[11].x) / 4;

			//pour les y
			rectMoy[0].y = (rectangleBillard[0].y + rectangleBillard[1].y) / 2;
			rectMoy[1].y = (rectangleBillard[2].y + rectangleBillard[3].y + rectangleBillard[4].y + rectangleBillard[5].y) / 4;
			rectMoy[2].y = (rectangleBillard[6].y + rectangleBillard[7].y) / 2;
			rectMoy[3].y = (rectangleBillard[8].y + rectangleBillard[9].y + rectangleBillard[10].y + rectangleBillard[11].y) / 4;


			// calcul de l'angle
			iAngle1 = (atan2(rectangleBillard[0].x - rectangleBillard[1].x , rectangleBillard[0].y - rectangleBillard[1].y)) * (180/PI);
			iAngle2 = (atan2(rectangleBillard[2].y - rectangleBillard[5].y , rectangleBillard[5].x - rectangleBillard[2].x)) * (180/PI);
			iAngle3 = (atan2(rectangleBillard[7].x - rectangleBillard[6].x , rectangleBillard[7].y - rectangleBillard[6].y)) * (180/PI);
			iAngle4 = (atan2(rectangleBillard[11].y - rectangleBillard[8].y , rectangleBillard[8].x - rectangleBillard[11].x)) * (180/PI);

			iAngle = (iAngle1 + iAngle2 + iAngle3 + iAngle4) / 4;

			//get the affine transformation matrix
			Mat matRotation = getRotationMatrix2D( Point(matOriginalFrame.cols / 2, matOriginalFrame.rows / 2), - iAngle , 1 ); //iAngle = 182

			warpAffine( matOriginalFrame, matRotatedFrame, matRotation, matOriginalFrame.size() );

			//ROI: Region Of Interest : on ne va traiter que cette partie de la matrice
			// Recadrer l'image
			myROI = cvRect( rectMoy[0].x, rectMoy[1].y, rectMoy[2].x - rectMoy[0].x, rectMoy[3].y - rectMoy[1].y);

			compteurDePoint++; //évite que le calcul se fasse plein de fois
			}
			else
			{
			matRecadrer = matRotatedFrame(myROI);
			}*/
			matRecadrer = matOriginalFrame; // à enlever
		}
		else //On prend l'image d'origine tant qu'on a pas recadré l'image
		{
			matRecadrer = matOriginalFrame;
		}

		/////////////////////////////////////////////// Detection de Lignes //////////////////////////////////////////////////////////////////////////////////////////////

		Mat matLine;

		if (posBlanche.x == 0)//Si pas de canne
		{
			//Alors on cherche à l'intérieur de tout le billard
			ROICanne = myROI;
			cout << "lines est empty" << endl;
		}
		else//Canne
		{
			//Alors on cherche à proximité de la position précédente de la canne
			ROICanne = cvRect(posBlanche.x - ecartCanne, posBlanche.y - ecartCanne, 2 * ecartCanne, 2 * ecartCanne);
			cout << "On a trouvé une canne" << endl;
		}

		matLine = matOriginalFrame(ROICanne);

		Canny(matLine, dst, 50, 200, 3);

		HoughLinesP(dst, lines, 1, CV_PI / 360, 50, minLineLengh, maxLineGap); //Plus le 3eme membre est petit plus la fonction est précise mais lente

		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];

			line(matLine, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
			//On essaye d'identifier les lignes correspondant à la canne	
		}

		//Calcul de la moyenne des coeficients directeurs
		for (size_t j = 0; j < lines.size(); j++)
		{
			if (lines[j][2] - lines[j][0] != 0) //On évite la division par zéro
			{
				int coefDir;
				//int ordOrig;
				int maxX = max(lines[j][0], lines[j][2]);
				int minX = min(lines[j][0], lines[j][2]);
				if (lines[j][0] == maxX)
				{
					int temp = lines[j][3];
					lines[j][3] = lines[j][1];
					lines[j][1] = temp;
				}
				coefDir = (lines[j][3] - lines[j][1]) / (maxX - minX);
				//ordOrig = lines[j][1] - coefDir * lines[j][0];
				//On fait la moyenne des coef directeurs et des ordonnées à l'origine
				coefDirCanne = (coefDirCanne + coefDir) / (j + 1);
				//ordOrigCanne = (ordOrigCanne + ordOrig) / (j + 1);
			}
		}
		//Calcul des max en min
		for (size_t k = 0; k < lines.size(); k++)
		{
			maxCanneX = max(lines[k][0], lines[k][2]);
			maxCanneY = max(lines[k][1], lines[k][3]);
			minCanneX = min(lines[k][0], lines[k][2]);
			minCanneY = min(lines[k][1], lines[k][3]);
		}

		if (coefDirCanne <= 0)
		{
			cannePoint[0].x = minCanneX;
			cannePoint[0].y = minCanneY;
			cannePoint[1].x = maxCanneX;
			cannePoint[1].y = maxCanneY;
		}
		if (coefDirCanne > 0)
		{
			cannePoint[0].x = minCanneX;
			cannePoint[0].y = maxCanneY;
			cannePoint[1].x = minCanneX;
			cannePoint[1].y = maxCanneY;
		}
		imshow(pzRotatingWindowName, matLine);
		////////////////////////////////////// Detection de cercles //////////////////////////////////////////////////////////////////////


		//On crée deux matrices
		//src : image d'origine
		//gray : image convertie en noir et blanc
		Mat src, gray;
		//src = imread( "building.jpg", 1 );resize(src,src,Size(640,480));
		//cap.read(src);
		src = matRecadrer;
		//Conversion de l'image source en N&B, stockage dans la matrice 'gray'
		cvtColor(src, gray, CV_BGR2GRAY);

		// On réduit le bruit par un flou gaussien pour affiner la détection des cercles
		// N&B -> N&B
		GaussianBlur(gray, gray, Size(13, 13), 1, 1);

		vector<Vec3f> circles;

		//circles.resize(16); // Redéfini le nombre de cercles recherchés -> il faudrait trouver comment utiliser gpu car dasn ce cas le nombre de cercles est un paramêtre de HoughCircle -> c'est mieux !!!

		// Application la transformée de Hough pour trouver les cercles
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, DIST_MIN, thresholdCanny, thresholdCenter, MIN_SIZE, MAX_SIZE);


		// Dessin des cercles détectés
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			//circle( src, center, 2, Scalar(0,255,0), -1, 8, 0 );// circle center     
			circle(src, center, radius, Scalar(255, 255, 0), 1, 8, 0);// circle outline
			//cout << "center : " << center << "\nradius : " << radius << endl;
		}


		////////////////////////////////////// Détection de couleurs à l'intérieur des cercles ///////////////////////////////////////////////////
		Mat img_rgb, img_hsv;
		img_rgb = src;
		//On converti img_rgb en img_hsv
		cvtColor(img_rgb, img_hsv, CV_RGB2HSV);
		//On crée un tableau des billes
		struct Bille TableauBilles[16];
		//On cherche le couleur à l'intérieur de chaque cercle
		for (size_t i = 0; i < circles.size(); i++) //pour éviter les erreures d'assertion
		{

			//for(int j = 0; j < 10; j++)//On parcours quelques points de la bille pour faire une moyenne
			//{
			int bla = compteurDeCouleur % 1; //On fait l'opération une fois tous les 2 tours
			if (bla == 0)
			{
				//Dans la fonction qui suit y est en premier et x en second

				//Le "point" la ligne qui suit est d'une importance primordiale !!!!
				Vec3b intensity = img_hsv.at<Vec3b>(Point(cvRound(circles[i][0]), cvRound(circles[i][1])));//On s'intéresse ici au point d'une des billes
				/* In case of BGR coded image
				int blue = intensity.val[0];
				int green = intensity.val[1];
				int red = intensity.val[2];
				*/


				//cout << "Bille n° " << i << " is ( " << blue << " ," << green << " ," << red << " )" << endl;

				// In case of HVS coded image
				int intens0 = (int)intensity.val[0];
				int intens1 = (int)intensity.val[1];
				int intens2 = (int)intensity.val[2];

				//Si la couleur est comprise dans un intervalle correct alors on considère qu'on a une bille sinon non


				if (intens1 < maxBlanche)
				{
					if (compteurDeLigne == 0)
					{
						TableauBilles[i].color = "Blanche";
						//cout << "Color " << TableauBilles[i].color <<endl;
						//cout << "( " << intens0 << ", " << intens1 << ", " << intens2 << " )" << endl;
						posBlanche.x = circles[i][0];
						posBlanche.y = circles[i][1];
						cout << posBlanche.x << endl;
						putText(src, TableauBilles[i].color, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), 1, 1, Scalar(255, 255, 255));
					}
					else if ((circles[i][0] < posBlanche.x + 50) && (circles[i][0] > posBlanche.x - 50))
					{
						TableauBilles[i].color = "Blanche";
						//cout << "Color " << TableauBilles[i].color <<endl;
						//cout << "( " << intens0 << ", " << intens1 << ", " << intens2 << " )" << endl;
						posBlanche.x = circles[i][0];
						posBlanche.y = circles[i][1];
						cout << posBlanche.x << endl;
						putText(src, TableauBilles[i].color, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), 1, 1, Scalar(255, 255, 255));
					}
					else
					{

					}
				}
				else if (intens2 < maxNoire)
				{
					TableauBilles[i].color = "Noire";
					//cout << "Color " << TableauBilles[i].color <<endl;
					//cout << "( " << intens0 << ", " << intens1 << ", " << intens2 << " )" << endl;

					putText(src, TableauBilles[i].color, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), 1, 1, Scalar(0, 0, 0));
				}
				else if (intens0 < maxRed && intens0 > minRed)
				{
					TableauBilles[i].color = "Rouge";
					//cout << "Color " << TableauBilles[i].color <<endl;
					//cout << "( " << intens0 << ", " << intens1 << ", " << intens2 << " )" << endl;

					putText(src, TableauBilles[i].color, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), 1, 1, Scalar(0, 0, 255));
				}
				else if (intens0 > minJaune && intens0 < maxJaune)
				{
					TableauBilles[i].color = "Jaune";
					//cout << "Color " << TableauBilles[i].color <<endl;
					//cout << "( " << intens0 << ", " << intens1 << ", " << intens2 << " )" << endl;

					putText(src, TableauBilles[i].color, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), 1, 1, Scalar(0, 255, 255));
				}
				else
				{
					TableauBilles[i].color = "Unknown"; // for unknonw : ce n'est pas un bille
					//cout << "Color " << TableauBilles[i].color <<endl;
					//cout << "( " << intens0 << ", " << intens1 << ", " << intens2 << " )" << endl;

					putText(src, TableauBilles[i].color, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), 1, 1, Scalar(255, 0, 0));
				}
				//cout << "( " << intens0 << ", " << intens1 << ", " << intens2 << " )" << endl;
				//cout << "Color " << TableauBilles[i].color <<endl;
				//cout << "Couleur ( " << intensity[0] << ", " << intensity[1] << ", " << intensity[2] << " )" << endl;

			}
			else
			{

			}
		}
		//////////////////////////////////////////// Rebonds ///////////////////////////////////////////////////////////////////////////////////////////////////////		
		
		Point positionBilles[16] ;
		for (size_t i = 0; i < 15; i++) //initialisation du tableau avec des billes à l'origine, pour éviter les pointeurs null
		{
			positionBilles[i] = Point(0, 0);
		}

		for (size_t i = 0; i < 16; i++) //Extraction de la position des billes non-blanches
		{
			if (TableauBilles[i].color.compare("Blanche") != 0)
			{
				positionBilles[i] = Point(TableauBilles[i].x, TableauBilles[i].y);
			}
		}

		vector<Point> Rebonds = algo(rectangleBillard, positionBilles,posBlanche,cannePoint);
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		compteurDeCouleur++;
		compteurDeLigne++;
		cout << compteurDeLigne << endl;
		///////////////////////////////////////////// Affichage ///////////////////////////////////////////////////////////////////////////////

		line(matLine, Point(cannePoint[0].x, cannePoint[0].y), Point(cannePoint[1].x, cannePoint[1].y), Scalar(255, 255, 0), 5, CV_AA); //Affichage de la portion de canne détectée
		for (int i = 0; i < 12; i += 2) //Affichage des bandes
		{
			line(matOriginalFrame, Point(rectangleBillard[i].x, rectangleBillard[i].y), Point(rectangleBillard[i + 1].x, rectangleBillard[i + 1].y), Scalar(0, 255, 255), 2, CV_AA);
		}

		// Affiche le resultat

		imshow(pzCirclesWindowName, src);

		//Attends pendant 30 ms une pression sur la touche ESC; sort de la boucle si c'est le cas
		if (waitKey(30) == 27)
		{
			cout << "L'utilisateur a appuyé sur la touche ESC" << endl;
			break;
		}
	}
	return 0;
}