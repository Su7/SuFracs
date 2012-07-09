/*
  Projet Algorithmique S1 A2011, Quentin Barrand)

  Ce programme et son code source sont placés sous licence Creative Commons BY 
  http://creativecommons.org/licenses/by/3.0/

  La bibliothèque graphique DrawingWindow utilisée dans ce programme est 
  (c) 2007-2010, Arnaud Giersch  <arnaud.giersch@iut-bm.univ-fcomte.fr>, 
  Tous droits réservés.
*/

#include <iostream>
#include <QApplication>
#include <DrawingWindow.h>
#include <sstream>

using namespace std;

//fonction de dessin
void remplissage(DrawingWindow &w)
{
  w.setBgColor("black");
  w.clearGraph();
  w.setColor("grey");
  w.drawText(w.width/2, w.height/3, 
  		"Bienvenue dans mon programme de fractales !", 
	    Qt::AlignHCenter);
  w.drawText(w.width/2, 2*(w.height/3), 
	    "Reportez-vous à la console pour paramétrer et afficher votre fractale.", 
	    Qt::AlignHCenter);

  /*
    zoom[] est un tableau qui contient les extremums des axes d'abscisse et d'ordonnée. 
    Par défaut il est initialisé à la valeur donnée dans le sujet du projet, mais 
    on modifiera son contenu à l'aide de la méthode waitMousePress.
    Les données sont organisées comme ceci :
    zoom[0] = minimum de l'axe 0x
    zoom[1] = maximum de l'axe 0x
    zoom[2] = minimum de l'axe 0y
    zoom[3] = maximum de l'axe Oy
    On utilise un tableau de longueur 200 dans un souci de préservation de la RAM
    pour stocker les coordonnées zoom après zoom. 
    Une longueur de 200 permet de stocker 200/4 = 50 niveaux de zoom. Dans la pratique,
    on est limitésbien avant d'atteindre ce nombre par la capacité de la variable 
    de type double utilisée pour les coordonnées des points.
  */
  double cx, cy, zoom[200] = {-1.5, 1.5, -1.5, 1.5};
  
  //Niveau de zoom
  int nz = 0, formule, rendu;
  string rendutable[6] = {"Classique (sujet)", "Dégradé rouge", "Dégradé vert", 
			  "Dégradé bleu", "Dégradé blanc", "Experimental"};
  string formuletable[5] = {"0.382 + 0.147i (sujet)", "-0.8 + 0i (Mandelbrot)", 
			    "0 + 1i (Eclair)", "-0.8 + 0.2i", "Saisie manuelle"};    

  cout<<"Bienvenue dans mon programme de fractales !"<<endl<<endl<<endl;

  //Choix de la formule de la fractale via un menu
  cout<<"Choisissez la formule de votre fractale :"<<endl;

  for(int i=0; (float)i<(sizeof(formuletable)/sizeof(string)); i++)
    cout<<i+1<<": "<<formuletable[i]<<endl;

  cout<<endl<<"Formule choisie : ";
  cin>>formule;

  //Message d'erreur si la valeur saisie n'est pas conforme à ce qui est attendu
  while(formule<1 || (float)formule>(sizeof(formuletable)/sizeof(string)))
    {
      cout<<endl<<"Le caractère saisi n'est pas correct, réessayez !"<<endl;

      for(int i=0; (float)i<(sizeof(formuletable)/sizeof(string)); i++)
	cout<<i+1<<": "<<formuletable[i]<<endl;

      cout<<endl<<"Formule choisie : ";
      cin>>formule;
    }
  
switch(formule)
  {
  case 1:
    cx = 0.382;
    cy = 0.147;
    break;

  case 2:
    cx = -0.8;
    cy = 0;
    break;

  case 3:
    cx = 0;
    cy = 1;
    break;

  case 4:
    cx = -0.8;
    cy = 0.2;
    break;

  case 5:
    cout<<endl<<"Entrez la partie réelle du paramètre Re(c) = ";
    cin>>cx;
    cout<<"Entrez la partie imaginaire du paramètre Im(c) = ";
    cin>>cy;

    /*
      Stockage de la formule saisie dans formuletable[4] pour lui permettre 
      d'être affichée dans le gadget en haut à gauche de la fenêtre de dessin
    */    
    ostringstream oss;
    oss<<cx<<" + "<<cy<<"i";
    formuletable[3] = oss.str();

    break;
  }

  cout<<"Le paramètre choisi est c = "<<cx<<" + "<<cy<<"i."<<endl<<endl;;

  //Choix du rendu (jeu de couleurs)
  cout<<"Choisissez un rendu (vous pouvez le changer en cours";
  cout<<"d'exécution en pressant le bouton central de votre souris) :"<<endl;

  for(int i=0; (float)i<(sizeof(rendutable)/sizeof(string)); i++)
    cout<<i+1<<": "<<rendutable[i]<<endl;

  cout<<endl<<"Rendu choisi : ";
  cin>>rendu;

  //Message d'erreur si la valeur saisie n'est pas conforme à ce qui est attendu
  while(rendu<1 || (float)rendu>(sizeof(rendutable)/sizeof(string)))
    {
      cout<<endl<<"Le caractère saisi n'est pas correct, réessayez !"<<endl;

      for(int i=0; (float)i<(sizeof(rendutable)/sizeof(string)); i++)
	cout<<i+1<<": "<<rendutable[i]<<endl;

      cout<<endl<<"Rendu choisi : ";
      cin>>rendu;
    }

  //nombre d'itérations N dans la suite
  const int N = 100;

  while(true)
    {
      w.setBgColor("black");
      w.clearGraph();
      w.setColor("grey");
      w.drawText(w.width/2, w.height/2, "Chargement, veuillez patienter...", 
		 Qt::AlignHCenter);
      w.drawText(w.width/2, 2*(w.height/3), 
		 "Deux clics gauches pour zoomer, clic droit pour dézoomer,", 
		 Qt::AlignHCenter);
      w.drawText(w.width/2, 2*(w.height/3)+15, "clic central pour changer de rendu.", 
		 Qt::AlignHCenter);
 
      //Balayage de la fenêtre
      for(int y=0; y<w.height; y++)
	{
	  for(int x=0; x<w.width; x++)
	    {
	      /*
		On va maintenant transformer les coordonnées du pixel (x,y) 
		en coordonnées du repère de zoom[].
		Pour ce faire, j'ai choisi d'implémenter une version modifiée 
		des méthodes utilisées dans la correction du TP6 qui ne nécessitent
		plus l'emploi de variables globales.
	      */

	      //Calcul de la partie réelle de z, affixe de p dans le repère défini par zoom[]
	      double rzn = (zoom[4*nz+1] - zoom[4*nz]) * x / (w.width - 1) + zoom[4*nz];

	      //Calcul de la partie imaginaire de z, affixe de p dans le repère défini par zoom[]
	      double izn = (zoom[4*nz+3] - zoom[4*nz+2]) * y / (w.height - 1) + zoom[4*nz+2];

	      /*
		Maintenant qu'on a les parties réelles et imaginaires de z, 
		calculons zn à l'aide de la suite.
		* rzn est la partie réelle de zn et izn sa partie imaginaire.
		* przn est la partie réelle de zn-1 et pizn sa partie imaginaire.
	      */
	      double przn = rzn, pizn = izn;
	      int n = 0;

	      while(n<N && ((rzn*rzn + izn*izn)<4))
		{
		  //Calcul de zn
		  rzn = (przn*przn - pizn*pizn) + cx; 
		  izn = (2*przn*pizn) + cy;
		  
		  //zn-1 reçoit zn pour le prochain tour de boucle
		  przn = rzn;
		  pizn = izn;

		  /*
		    Incrémentation du n qui sert à éviter la boucle while() infinie 
		    mais qui est aussi utilisé pour déterminer si p appartient à 
		    l'ensemble de Julia (voir la suite).
		  */
		  n++;
		}	      
	      
	      /*
		On sélectionne la couleur en fonction du nombre d'itérations : si n == N, cela signifie que malgré 
		les N itérations de la boucle, le module de zn au carré est strictement inférieur à 4; il appartient
		donc à l'ensemble de Julia et on sélectionne la couleur noire. Dans tous les autres cas on peut définir 
		une couleur à l'aide de n (sa valeur étant progressive on l'utilise pour faire un dégradé).
	      */
	      if(n == N)
		w.setColor("black");
	      else
		{
		  float couleur = (float)n/100;

		  switch(rendu)
		    {
		    case 1:
		      w.setColor(couleur*couleur, couleur-(couleur*couleur*couleur*couleur*
							   couleur), 1 - couleur);
		      break;
		    case 2:
		      w.setColor(couleur, 0, 0);
		      break;
		    case 3:
		      w.setColor(0, couleur, 0);
		      break;
		    case 4:
		      w.setColor(0, 0, couleur);
		      break;
		    case 5:
		      w.setColor(couleur, couleur, couleur);
		      break;
		    case 7:
		      w.setColor(couleur, couleur*couleur, 0);
		      break;
		    }
		}

	      //Enfin, une fois la couleur sélectionnée, on dessine le pixel
	      w.drawPoint(x, y);
	    }
	}
      //Affichage d'un gadget en haut à gauche qui résume les principales caractéristiques du dessin en cours
      ostringstream oss;
     
      oss<<"Niveau de zoom : "<<nz<<" "<<endl<<"Zone de dessin : ["<<zoom[4*nz]<<" "<<zoom[4*nz+2]<<
	"i; "<<zoom[4*nz+1]<<" + "<<zoom[4*nz+3]<<"i]. "<<endl<<"Formule : "<<formuletable[formule-1]<<
	", rendu : "<<rendutable[rendu-1]<<"  "; 

      string chaine = oss.str();

      w.setColor("grey");
      w.drawTextBg(0, 0, chaine);

      /*
	Cette partie 'zoom' récupère les coordonnées de deux clics de souris
	à l'aide de la méthode waitMousePress() et stocke ces coordonnées 
	à la suite du tableau zoom.
	On est obligé d'utiliser un tableau tempzoom[] pour stocker 
	temporairement les coordonnées récupérées par la souris; 
	leur mise à l'échelle fait appel aux données de zoom[].
      */
      double tempzoom[4];
      int enz = 0;
     
      for(int i=0; i<2; i++)
	{
	  int zoomx = 0, zoomy = 0, button = 0;

	  w.waitMousePress(zoomx, zoomy, button);

	  //Pour revenir au niveau de zoom précédent
	  if(i == 0 && button == 2 && nz != 0)
	    {
	      for(int j = 0; j<4; j++)
		tempzoom[j] = zoom[4*nz - 4 + j];
	      enz = -1;
	      break;
	    }

	  //Pour annuler le clic gauche
	  if(i == 1 && button == 2)
	    {
	      for(int j=0; j<4; j++)
		tempzoom[j] = zoom[4*nz + j];
	      break;
	    }

	  //Pour changer de rendu
	  if(button == 3)
	    {
	      for(int j=0; j<4; j++)
		tempzoom[j] = zoom[4*nz + j];

	      if((float)rendu == sizeof(rendutable)/sizeof(string))
		rendu = 1;
	      else
		rendu++;

	      break;
	    }
	  
	  /*
	    Dans le cas ou on presse une deuxième fois le bouton gauche de la souris
	    (on souhaite réellement zoomer)
	  */
	  if(button != 2)
	    {
	      w.setColor("grey");
	      w.setPenWidth(2);
	      w.drawLine(0, zoomy, w.width, zoomy);
	      w.drawLine(zoomx, 0, zoomx, w.height);
	      
	      tempzoom[i] = (zoom[4*nz + 1] - zoom[4*nz]) * zoomx / (w.width - 1) + zoom[4*nz];
	      tempzoom[i+2] = (zoom[4*nz + 3] - zoom[4*nz + 2]) * zoomy / (w.height - 1) + zoom[4*nz+2];
	  
	      enz++;
	    }
	}
      //Incrémentation du niveau de zoom en fonction de enz
      switch(enz)
	{
	case -1:
	  nz--;
	  break;
	case 2:
	  nz++;
	  break;
	}

      /*
	Pour respecter l'organisation de zoom[,] on est obligé d'utiliser
	les fonctions min et max comme ceci :
      */
      zoom[4*nz    ] = min(tempzoom[0], tempzoom[1]);
      zoom[4*nz + 1] = max(tempzoom[0], tempzoom[1]);
      zoom[4*nz + 2] = min(tempzoom[2], tempzoom[3]);
      zoom[4*nz + 3] = max(tempzoom[2], tempzoom[3]); 
    }
}


int main(int argc, char *argv[])
{
  /*
    40 : hauteur standard d'un terminal (Linux, DOS). On fait ici l'équivalent de la
    commande 'clear' sous Linux.
  */  
  for(int i=0; i<40; i++)
    cout<<endl;

  QApplication application(argc, argv);
    
  /*
    On peut spécifier ici la taille en pixels de la fenêtre, 
    (on la choisit carrée pour ne pas étirer la fractale)
  */
  DrawingWindow julia(remplissage, 500, 500);

  julia.setWindowTitle("Projet Algorithmique de S1 par Quentin Barrand - A2011");
  julia.show();

  return application.exec();
}