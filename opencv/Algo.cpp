


#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#include "Main.h"
#define NBR					4








Droite droite (const Point p1, const Point p2){


	Droite r;
	r.a =p1.y-p2.y;

	r.b =p2.x-p1.x;

	r.c =-p1.y*p2.x+p1.x*p2.y;

	return r; 
}

Droite droitev (const Point p, const Vecteur v){

	Droite r;
	r.a = v.y;
	r.b = -v.x;
	r.c = -r.a*p.x-r.b*p.y;
	return r;
}

Segment segment (const Point p1, const Point p2){


	Segment r;
	r.d = droite (p1,p2);
	r.p1 = p1;
	r.p2 = p2;

	return r; 
}

Point croisement (const Droite d1, const Droite d2){ 
	
	 Point r;
	 if(d2.a*d1.b-d1.a*d2.b != 0){
		r.y=(d2.c*d1.a-d2.a*d1.c)/(d2.a*d1.b-d1.a*d2.b);

		r.x=(d1.c*d2.b-d1.b*d2.c)/(d2.a*d1.b-d1.a*d2.b);
	 }else{
		 r.x = 0;
		 r.y = 0;
	 }

	 
	 return r;
}

bool appartient(const Segment s, const Point p){

	int i=0;

	if(s.p1.x < s.p2.x )
	{
		if( s.p1.x < p.x && p.x < s.p2.x )
			i=i++;
	}
	else
	{
		if(s.p2.x <= p.x && p.x <= s.p1.x )
			i=i++;
	}

	if(s.p1.y < s.p2.y )
	{
		if( s.p1.y < p.y && p.y < s.p2.y )
			i=i++;
	}
	else
	{
		if(s.p2.y <= p.y && p.y <= s.p1.y )
			i=i++;
	}

	if( -1 < s.d.a*p.x+s.d.b*p.y+s.d.c && s.d.a*p.x+s.d.b*p.y+s.d.c < 1 )
		i++;

	if(i == 3)
		return true;
	else 
		return false;

}

int pscalaire(const Point p1, const Point p2, const Point p3, const Point p4){

	int r;

	r = (p2.x-p1.x)*(p4.x-p3.x)+(p2.y-p1.y)*(p4.y-p3.y);

	return r;
}

Point sym(const Segment s, const Point a){
	
	Point r;
	r.x = (-s.d.a*s.d.a*a.x-2*s.d.a*(s.d.b*a.y+s.d.c)+s.d.b*s.d.b*a.x)/(s.d.a*s.d.a+s.d.b*s.d.b);
	r.y = (s.d.a*s.d.a*a.y-2*s.d.b*(s.d.a*a.x+s.d.c)-s.d.b*s.d.b*a.y)/(s.d.a*s.d.a+s.d.b*s.d.b);
	//r.y = 1/(1+s.d.a*s.d.a/s.d.b*s.d.b)*(2*s.p1.y-a.y-s.d.a/s.d.b*(2*a.x-s.d.a/s.d.b*a.y-2*s.p1.x));
	//r.x = a.x+s.d.a/s.d.b*(r.y-a.y);

	return r;
}

int distancepp(const Point p1, const Point p2){
	int r;
	r =  (int)sqrtf((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
	return r;
}

int distancepd(const Point p, const Droite d){
	int r;
	r = (int)(1/(sqrtf(d.a*d.a+d.b*d.b)*abs(d.a*p.x+d.b*p.y+d.c)));
	return r;
}

Point projection(const Droite d, const Point p){
	Point r;
	r.x = (d.b*d.b*p.x-d.a*(d.b*p.y+d.c))/(d.a*d.a+d.b*d.b);
	r.y = (d.a*d.a*p.y-d.b*(d.a*p.x+d.c))/(d.a*d.a+d.b*d.b);
	return r;
}


vector<Point> rebond (const Point coin[12],const Point boules[15], const Point blanche,const Point cane[2],const int rayon){


		// tableau dynamique des points a et b
	Point p;
	vector<Point> apt;
	for (int i =0 ; i < 100 ;i++){apt.push_back(p);}
	vector<Point> bpt;
	for (int i =0 ; i < 100 ;i++){bpt.push_back(p);
		bpt[i].x = -10;
		bpt[i].y = -10;
	}
	Point cb;

	

		// coordonnées des points de départ

	cb.x=blanche.x;cb.y=blanche.y; // boule blanche

	if(distancepp(cane[0],blanche)<distancepp(cane[1],blanche)){
		apt[0] = cane[0]; // pointe canne puis déplacement boule blanche
		bpt[0] = cane[1]; // arrière canne puis déplacement symétrique
	}else{
		apt[0] = cane[1]; // pointe canne puis déplacement boule blanche
		bpt[0] = cane[0]; // arrière canne puis déplacement symétrique
	}
	// equation des bords, des trous, et de la canne (vérifiées : OK )

	Segment bord[6];

	for(int i = 0; i < 12; i += 2){
			bord[i/2]=segment(coin[i],coin[i+1]);
	}

	Segment trou[6];
	for(int i=1;i < 12; i += 2){
		int j = (i+1)%12;
		trou[(i-1)/2] = segment(coin[i],coin[j]);
	}

	vector<Segment> canne; 

	Segment s;
	for (int i =0 ; i < 6 ;i++){
		canne.push_back(s);
	}
	 canne[0].p1 = apt[0];
	 canne[0].p2 = bpt[0];
	 canne[0].d = droite(canne[0].p1,canne[0].p2);

	 


	// equation des points d'intersection

	Segment bs[6];

	int Lmax = 5;
	int j = 0;
	int r = rayon;
	int t = 0;



	while(j < Lmax){
		
		//recherche du point d'intersection :

		for(int i = 0; i < 6; i++){
			if(appartient(trou[i],croisement(canne[j].d,trou[i].d))){
				if(pscalaire(bpt[j],apt[j],apt[j],croisement(canne[j].d,bord[i].d)) > 0){
						apt[j] = croisement(canne[j].d,trou[i].d);
						t = 1;
					}
			}
			else{
				if(appartient(bord[i],croisement(canne[j].d,bord[i].d))){
					if(pscalaire(bpt[j],apt[j],apt[j],croisement(canne[j].d,bord[i].d)) > 0){
						apt[j+1] = croisement(canne[j].d,bord[i].d);
						bs[j+1] = bord[i];
						bpt[j+1] = sym(bs[j+1], apt[j]);
						canne[j+1] = segment(apt[j+1],bpt[j+1]);
						
					}
				}
			}
		}

		
		
		/*

		for(int i = 0; i < 15;i++){
			Droite d = droite(apt[j],apt[j+1]);
			if(distancepd(boules[i],d)<r){
				Point p = boules[i];
				//apt[j+1].x = (-(int)sqrtf(-d.b*d.b*(-4*d.a*d.a*r*r+d.a*d.a*boules[i].x*boules[i].x+2*d.a*d.b*boules[i].x*boules[i].y+2*d.a*d.c*boules[i].x-4*d.b*d.b*r*r+d.b*d.b*boules[i].y*boules[i].y+2*d.b*d.c*boules[i].y+d.c*d.c))-d.a*d.b*boules[i].y-d.a*d.c+d.b*d.b*boules[i].x)/(d.a*d.a+d.b*d.b);
				//apt[j+1].y =(d.a*(int)sqrtf(-d.b*d.b*(-4*d.a*d.a*r*r+d.a*d.a*boules[i].x*boules[i].x+2*d.a*d.b*boules[i].x*boules[i].y+2*d.a*d.c*boules[i].x-4*d.b*d.b*r*r+d.b*d.b*boules[i].y*boules[i].y+2*d.b*d.c*boules[i].y+d.c*d.c))+d.a*d.a*d.b*boules[i].y-d.a*d.b*d.b*boules[i].x-d.b*d.b*d.c)/(d.b*(d.a*d.a+d.b*d.b));
				apt[j+1].x = (-(int)sqrtf(-d.b*d.b*(-4*d.a*d.a*r*r+d.a*d.a*p.x*p.x+2*d.a*d.b*p.x*p.y+2*d.a*d.c*p.x-4*d.b*d.b*r*r+d.b*d.b*p.y*p.y+2*d.b*d.c*p.y+d.c*d.c))-d.a*d.b*p.y-d.a*d.c+d.b*d.b*p.x)/(d.a*d.a+d.b*d.b);
				apt[j+1].y = (d.a*(int)sqrtf(-d.b*d.b*(-4*d.a*d.a*r^2+d.a*d.a*p.x^2+2*d.a*d.b*p.x*p.y+2*d.a*d.c*p.x-4*d.b*d.b*r^2+d.b*d.b*p.y^2+2*d.b*d.c*p.y+d.c*d.c))+d.a*d.a*d.b*p.y-d.a*d.b*d.b*p.x-d.b*d.b*d.c)/(d.b*(d.a*d.a+d.b*d.b));
				
					
				Droite d1 = droite(apt[j+1],boules[i]);
				Vecteur v; v.x = d1.a; v.y = d1.b;
				Droite d2 = droitev(apt[j+1],v);
				bpt[j+1] = projection(d2,apt[j]);
				canne[j+1] = segment(apt[j+1],bpt[j+1]);
				t = 0;
			}
		}
		*/

		j = j+1;
		if(t == 1 ){ break; }
	}


	return apt;





}	