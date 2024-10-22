#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#define NBR	4
#include "Main.h"

Droite droite(const Point p1, const Point p2);

Droite droitev(const Point p, const Vecteur v){

	Droite r;
	r.a = v.y;
	r.b = -v.x;
	r.c = -r.a*p.x - r.b*p.y;
	return r;
}

Segment segment(const Point p1, const Point p2);

Point croisement(const Droite d1, const Droite d2){

	Point r;
	if (d2.a*d1.b - d1.a*d2.b != 0){
		r.y = (d2.c*d1.a - d2.a*d1.c) / (d2.a*d1.b - d1.a*d2.b);

		r.x = (d1.c*d2.b - d1.b*d2.c) / (d2.a*d1.b - d1.a*d2.b);
	}
	else{
		r.x = 0;
		r.y = 0;
	}


	return r;
}

bool appartient(const Segment s, const Point p){

	int i = 0;

	if (s.p1.x < s.p2.x)
	{
		if (s.p1.x < p.x && p.x < s.p2.x)
			i = i++;
	}
	else
	{
		if (s.p2.x <= p.x && p.x <= s.p1.x)
			i = i++;
	}

	if (s.p1.y < s.p2.y)
	{
		if (s.p1.y < p.y && p.y < s.p2.y)
			i = i++;
	}
	else
	{
		if (s.p2.y <= p.y && p.y <= s.p1.y)
			i = i++;
	}

	if (-1 < s.d.a*p.x + s.d.b*p.y + s.d.c && s.d.a*p.x + s.d.b*p.y + s.d.c < 1)
		i++;

	if (i == 3)
		return true;
	else
		return false;

}

int pscalaire(const Point p1, const Point p2, const Point p3, const Point p4){

	int r;

	r = (p2.x - p1.x)*(p4.x - p3.x) + (p2.y - p1.y)*(p4.y - p3.y);

	return r;
}

Point sym(const Segment s, const Point a){

	Point r;
	r.x = (-s.d.a*s.d.a*a.x - 2 * s.d.a*(s.d.b*a.y + s.d.c) + s.d.b*s.d.b*a.x) / (s.d.a*s.d.a + s.d.b*s.d.b);
	r.y = (s.d.a*s.d.a*a.y - 2 * s.d.b*(s.d.a*a.x + s.d.c) - s.d.b*s.d.b*a.y) / (s.d.a*s.d.a + s.d.b*s.d.b);
	//r.y = 1/(1+s.d.a*s.d.a/s.d.b*s.d.b)*(2*s.p1.y-a.y-s.d.a/s.d.b*(2*a.x-s.d.a/s.d.b*a.y-2*s.p1.x));
	//r.x = a.x+s.d.a/s.d.b*(r.y-a.y);

	return r;
}

int distancepp(const Point p1, const Point p2){
	int r;
	r = (int)sqrtf((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
	return r;
}

int distancepd(const Point p, const Droite d){
	int r;
	r = (int)(1 / (sqrtf(d.a*d.a + d.b*d.b)*abs(d.a*p.x + d.b*p.y + d.c)));
	return r;
}

vector<Point> algo(const Point coin[12], const Point boules[15], const Point blanche, const Point cane[2]){


	// tableau dynamique des points a et b
	Point p;
	vector<Point> apt;
	for (int i = 0; i < 100; i++){ apt.push_back(p); }
	vector<Point> bpt;
	for (int i = 0; i < 100; i++){
		bpt.push_back(p);
		bpt[i].x = -10;
		bpt[i].y = -10;
	}
	Point cb;


	// coordonn�es des points de d�part

	cb.x = blanche.x; cb.y = blanche.y; // boule blanche

	if (distancepp(cane[0], blanche)<distancepp(cane[1], blanche)){
		apt[0] = cane[0]; // pointe canne puis d�placement boule blanche
		bpt[0] = cane[1]; // arri�re canne puis d�placement sym�trique
	}
	else{
		apt[0] = cane[1]; // pointe canne puis d�placement boule blanche
		bpt[0] = cane[0]; // arri�re canne puis d�placement sym�trique
	}
	// equation des bords, des trous, et de la canne (v�rifi�es : OK )

	Segment bord[6];

	for (int i = 0; i < 12; i += 2){
		bord[i / 2] = segment(coin[i], coin[i + 1]);
	}

	Segment trou[6];
	for (int i = 1; i < 12; i += 2){
		int j = (i + 1) % 12;
		trou[(i - 1) / 2] = segment(coin[i], coin[j]);
	}

	vector<Segment> canne;

	Segment s;
	for (int i = 0; i < 6; i++){
		canne.push_back(s);
	}
	canne[0].p1 = apt[0];
	canne[0].p2 = bpt[0];
	canne[0].d = droite(canne[0].p1, canne[0].p2);




	// equation des points d'intersection

	Segment bs[6];

	int Lmax = 4;
	int j = 0;
	int r = 3;
	int t = 0;



	while (j < Lmax){

		//recherche du point d'intersection :
		for (int i = 0; i < 6; i++){
			if (appartient(trou[i], croisement(canne[j].d, trou[i].d))){
				if (pscalaire(bpt[j - 1], apt[j - 1], apt[j - 1], croisement(canne[j].d, bord[i].d)) > 0){
					apt[j] = croisement(canne[j].d, trou[i].d);
					t = 1;
				}
			}
			else{
				if (appartient(bord[i], croisement(canne[j].d, bord[i].d))){
					if (pscalaire(bpt[j], apt[j], apt[j], croisement(canne[j].d, bord[i].d)) > 0){
						apt[j + 1] = croisement(canne[j].d, bord[i].d);
						bs[j + 1] = bord[i];
						bpt[j + 1] = sym(bs[j + 1], apt[j]);
						canne[j + 1] = segment(apt[j + 1], bpt[j + 1]);

					}
				}
			}
		}

		if (t == 1){ j = Lmax; }
		/*
		for(int i = 0; i < 15;i++){
		Droite d = droite(apt[j],apt[j+1]);
		if(distancepd(boules[i],d)<r){

		apt[j+1].x = (-sqrt(-d.b*d.b*(-4*d.a*d.a*r*r+d.a*d.a*boules[i].x*boules[i].x+2*d.a*d.b*boules[i].x*boules[i].y+2*d.a*d.c*boules[i].x-4*d.b*d.b*r*r+d.b*d.b*boules[i].y*boules[i].y+2*d.b*d.c*boules[i].y+d.c*d.c))-d.a*d.b*boules[i].y-d.a*d.c+d.b*d.b*boules[i].x)/(d.a*d.a+d.b*d.b);
		apt[j+1].y =(d.a*sqrt(-d.b*d.b*(-4*d.a*d.a*r*r+d.a*d.a*boules[i].x*boules[i].x+2*d.a*d.b*boules[i].x*boules[i].y+2*d.a*d.c*boules[i].x-4*d.b*d.b*r*r+d.b*d.b*boules[i].y*boules[i].y+2*d.b*d.c*boules[i].y+d.c*d.c))+d.a*d.a*d.b*boules[i].y-d.a*d.b*d.b*boules[i].x-d.b*d.b*d.c)/(d.b*(d.a*d.a+d.b*d.b));


		}
		*/


		j = j++;

	}


	return apt;





}