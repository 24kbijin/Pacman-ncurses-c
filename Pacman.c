#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>
#include <semaphore.h>

char **InitMatriz(char **matriz, char *pfile);
struct thread_data_ghost* findPos(struct thread_data_ghost *g1, char **matriz, int ancho, int alto, char id);
struct thread_data_pacman* findPosPacman(struct thread_data_pacman *pac, char **matriz, int ancho, int alto);

// Semaforos generales
sem_t ghos1;
sem_t ghos2;
sem_t ghos3;
sem_t ghos4;

int total;

struct thread_data_ghost {
	char id_ghost;
	int pos_x;
	int pos_y;
	char movant;
};

struct thread_data_pacman {
	char id_pac;
	int pos_x;
	int pos_y;
	int puntaje;
};

struct thread_data_ghost *data_ghost1;
struct thread_data_ghost *data_ghost2;
struct thread_data_ghost *data_ghost3;
struct thread_data_ghost *data_ghost4;

struct thread_data_pacman *data_pacman;

typedef enum { derecha, arriba, izquierda, abajo} direccion;

 char **matriz;
 int alto;
 int ancho;
 int gameover = 0;

				  
/*
	Movimientos :
	mvaddch(x,y++,char) ->derecha
	mvaddch(x,y--,char) ->izq
	mvaddch(x++,y,char) ->abajo
	mvaddch(x--,y,char) ->arriba 
*/	

int genMov(int num){
	srand (time(NULL));
  /* generate secret number between 1 and num: */
     return rand() % num;
     
}
				  
 void initTablero(){

 int i,j;
			
 for(i=0;i<alto;i++){
	for(j=0;j<ancho;j++){
		
		mvaddch(i,j,matriz[i][j]);
		refresh();
	}
		mvaddch(i,j,'\n');
		refresh();
 }
 mvaddstr(0,ancho+2,"Puntaje:");
 refresh();
}


 void setTablero(int y, int x, char str){
	 
	matriz[y][x]=str;
	mvaddch(y,x,str);
	refresh();
	
 }

 int puedeMoverseIzq( int x, int y, char id1, char id2, char id3){

 	if( y > 0 && matriz[x][y-1]!='*' && matriz[x][y-1]!= id1 && matriz[x][y-1]!= id2 && matriz[x][y-1]!= id3  ){
 		return 1;
 	}
 	return 0;
 }

 int puedeMoverseDer( int x, int y, char id1, char id2, char id3){

 	if( y < (ancho-1) && matriz[x][y+1]!='*'  && matriz[x][y+1]!= id1 && matriz[x][y+1]!= id2 && matriz[x][y+1]!= id3  ){
 		return 1;
 	}
 	return 0;
 }

int puedeMoverseAba( int x, int y, char id1, char id2, char id3){

 	if( x < (alto-1) && matriz[x+1][y]!='*'  && matriz[x+1][y]!= id1 && matriz[x+1][y]!= id2 && matriz[x+1][y]!= id3  ){
 		return 1;
 	}
 	return 0;
 }

int puedeMoverseArr( int x, int y, char id1, char id2, char id3){

 	if(x > 0 && matriz[x-1][y]!='*' && matriz[x-1][y]!= id1 && matriz[x-1][y]!= id2 && matriz[x-1][y]!= id3){
 		return 1;
 	}
 	return 0;
 
}

	void losser(){
		
			gameover=1;
			mvaddstr(ancho,alto+2,"¡You Lost! GameOver");
			refresh();
			sleep(5);
			clear();
			erase();
			endwin();
			pthread_exit(NULL);
	}
				  
	void *initGame1(void *threadarg){
		
	
	struct thread_data_ghost *my_dataG1;

	my_dataG1 = (struct thread_data_ghost *) threadarg;
	
	int numMov=0,posMov=0;
	
	my_dataG1->movant='.'; 
	 
		while(gameover == 0){
		sem_wait(&ghos1);
		
		if(my_dataG1->movant=='P'){
			losser();
		}
	    
	    if(puedeMoverseArr(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme arriba
         numMov++; 
        }

       if(puedeMoverseAba(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme abajo
      	numMov++;
       }

       if(puedeMoverseDer(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme derecha
        numMov++;
       }

       if(puedeMoverseIzq(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme izquierda
       	numMov++;
       }


 	
 		int arrMov[numMov];

 		if(puedeMoverseArr(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme arriba
         arrMov[posMov] = 0;
         posMov++; 
        }

       if(puedeMoverseAba(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme abajo
      	arrMov[posMov] = 1;
         posMov++;
       }

       if(puedeMoverseDer(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme derecha
        arrMov[posMov] = 2;
         posMov++;
       }

       if(puedeMoverseIzq(my_dataG1->pos_x, my_dataG1->pos_y,'2','3','4')==1){ //Puedo moverme izquierda
       	arrMov[posMov] = 3;
         posMov++;
       }
    

  	int movFinal = genMov(numMov);

  	switch(arrMov[movFinal]){

  		case 0:
  		
  	
  		
  		setTablero(my_dataG1->pos_x, my_dataG1->pos_y,my_dataG1->movant );
        my_dataG1->pos_x--;
        my_dataG1->movant = matriz[my_dataG1->pos_x][my_dataG1->pos_y];
        usleep(90000);
		setTablero(my_dataG1->pos_x,my_dataG1->pos_y, my_dataG1->id_ghost);

  		break;

  		case 1:

  		setTablero(my_dataG1->pos_x, my_dataG1->pos_y,my_dataG1->movant );
        my_dataG1->pos_x++;
        my_dataG1->movant = matriz[my_dataG1->pos_x][my_dataG1->pos_y];
        usleep(90000);
		setTablero(my_dataG1->pos_x,my_dataG1->pos_y, my_dataG1->id_ghost);

  		break;

  		case 2:

  		setTablero(my_dataG1->pos_x, my_dataG1->pos_y,my_dataG1->movant );
        my_dataG1->pos_y++;
        my_dataG1->movant = matriz[my_dataG1->pos_x][my_dataG1->pos_y];
        usleep(90000);
		setTablero(my_dataG1->pos_x,my_dataG1->pos_y, my_dataG1->id_ghost);

  		break;

  		case 3:

  		setTablero(my_dataG1->pos_x, my_dataG1->pos_y,my_dataG1->movant );
        my_dataG1->pos_y--;
        my_dataG1->movant = matriz[my_dataG1->pos_x][my_dataG1->pos_y];
        usleep(90000);
		setTablero(my_dataG1->pos_x,my_dataG1->pos_y, my_dataG1->id_ghost);

  		break;

  	}
		
		sem_post(&ghos2);
		numMov=0;
		posMov=0;
    }

pthread_exit(NULL);   
}
	
	void *initGame2(void *threadarg){

	 
	 struct thread_data_ghost *my_dataG2;

	my_dataG2 = (struct thread_data_ghost *) threadarg;

	int numMov=0,posMov=0;
	my_dataG2->movant='.';
	 
		while(gameover == 0){
		sem_wait(&ghos2);
		
		if(my_dataG2->movant=='P'){
			losser();
		}
	    
	    if(puedeMoverseArr(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme arriba
         numMov++; 
        }

       if(puedeMoverseAba(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme abajo
      	numMov++;
       }

       if(puedeMoverseDer(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme derecha
        numMov++;
       }

       if(puedeMoverseIzq(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme izquierda
       	numMov++;
       }


 	
 		int arrMov[numMov];

 		if(puedeMoverseArr(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme arriba
         arrMov[posMov] = 0;
         posMov++; 
        }

       if(puedeMoverseAba(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme abajo
      	arrMov[posMov] = 1;
         posMov++;
       }

       if(puedeMoverseDer(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme derecha
        arrMov[posMov] = 2;
         posMov++;
       }

       if(puedeMoverseIzq(my_dataG2->pos_x, my_dataG2->pos_y,'1','3','4')==1){ //Puedo moverme izquierda
       	arrMov[posMov] = 3;
         posMov++;
       }
    

  	int movFinal = genMov(numMov);

  	switch(arrMov[movFinal]){

  		case 0:
  		
  	
  		
  		setTablero(my_dataG2->pos_x, my_dataG2->pos_y,my_dataG2->movant );
        my_dataG2->pos_x--;
        my_dataG2->movant = matriz[my_dataG2->pos_x][my_dataG2->pos_y];
        usleep(90000);
		setTablero(my_dataG2->pos_x,my_dataG2->pos_y, my_dataG2->id_ghost);

  		break;

  		case 1:

  		setTablero(my_dataG2->pos_x, my_dataG2->pos_y,my_dataG2->movant );
        my_dataG2->pos_x++;
        my_dataG2->movant = matriz[my_dataG2->pos_x][my_dataG2->pos_y];
        usleep(90000);
		setTablero(my_dataG2->pos_x,my_dataG2->pos_y, my_dataG2->id_ghost);

  		break;

  		case 2:

  		setTablero(my_dataG2->pos_x, my_dataG2->pos_y,my_dataG2->movant );
        my_dataG2->pos_y++;
        my_dataG2->movant = matriz[my_dataG2->pos_x][my_dataG2->pos_y];
        usleep(90000);
		setTablero(my_dataG2->pos_x,my_dataG2->pos_y, my_dataG2->id_ghost);

  		break;

  		case 3:

  		setTablero(my_dataG2->pos_x, my_dataG2->pos_y,my_dataG2->movant );
        my_dataG2->pos_y--;
        my_dataG2->movant = matriz[my_dataG2->pos_x][my_dataG2->pos_y];
        usleep(90000);
		setTablero(my_dataG2->pos_x,my_dataG2->pos_y, my_dataG2->id_ghost);

  		break;

  	}
		
		sem_post(&ghos3);
		numMov=0;
		posMov=0;
    }
    pthread_exit(NULL);

	}
	
	void *initGame3(void *threadarg){
		
	 
	 struct thread_data_ghost *my_dataG3;

	my_dataG3 = (struct thread_data_ghost *) threadarg;
	 
	int numMov=0,posMov=0;
	my_dataG3->movant='.';
	 
		while(gameover == 0){
		sem_wait(&ghos3);
		
		if(my_dataG3->movant=='P'){
			losser();
		}
	    
	    if(puedeMoverseArr(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme arriba
         numMov++; 
        }

       if(puedeMoverseAba(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme abajo
      	numMov++;
       }

       if(puedeMoverseDer(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme derecha
        numMov++;
       }

       if(puedeMoverseIzq(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme izquierda
       	numMov++;
       }
 	
 		int arrMov[numMov];

 		if(puedeMoverseArr(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme arriba
         arrMov[posMov] = 0;
         posMov++; 
        }

       if(puedeMoverseAba(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme abajo
      	arrMov[posMov] = 1;
         posMov++;
       }

       if(puedeMoverseDer(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme derecha
        arrMov[posMov] = 2;
         posMov++;
       }

       if(puedeMoverseIzq(my_dataG3->pos_x, my_dataG3->pos_y,'1','2','4')==1){ //Puedo moverme izquierda
       	arrMov[posMov] = 3;
         posMov++;
       }
    

  	int movFinal = genMov(numMov);

  	switch(arrMov[movFinal]){

  		case 0:
  		
  	
  		
  		setTablero(my_dataG3->pos_x, my_dataG3->pos_y,my_dataG3->movant );
        my_dataG3->pos_x--;
        my_dataG3->movant = matriz[my_dataG3->pos_x][my_dataG3->pos_y];
        usleep(90000);
		setTablero(my_dataG3->pos_x,my_dataG3->pos_y, my_dataG3->id_ghost);

  		break;

  		case 1:

  		setTablero(my_dataG3->pos_x, my_dataG3->pos_y,my_dataG3->movant );
        my_dataG3->pos_x++;
        my_dataG3->movant = matriz[my_dataG3->pos_x][my_dataG3->pos_y];
        usleep(90000);
		setTablero(my_dataG3->pos_x,my_dataG3->pos_y, my_dataG3->id_ghost);

  		break;

  		case 2:

  		setTablero(my_dataG3->pos_x, my_dataG3->pos_y,my_dataG3->movant );
        my_dataG3->pos_y++;
        my_dataG3->movant = matriz[my_dataG3->pos_x][my_dataG3->pos_y];
        usleep(90000);
		setTablero(my_dataG3->pos_x,my_dataG3->pos_y, my_dataG3->id_ghost);

  		break;

  		case 3:

  		setTablero(my_dataG3->pos_x, my_dataG3->pos_y,my_dataG3->movant );
        my_dataG3->pos_y--;
        my_dataG3->movant = matriz[my_dataG3->pos_x][my_dataG3->pos_y];
        usleep(90000);
		setTablero(my_dataG3->pos_x,my_dataG3->pos_y, my_dataG3->id_ghost);

  		break;

  	}
		
		sem_post(&ghos4);
		numMov=0;
		posMov=0;
    }
   pthread_exit(NULL);
}
	
	void *initGame4(void *threadarg){
		
	struct thread_data_ghost *my_dataG4;
	my_dataG4 = (struct thread_data_ghost *) threadarg;
	 
	  
		int numMov=0,posMov=0;
		
		my_dataG4->movant='.';
	 
		while(gameover == 0){
		sem_wait(&ghos4);
		
		if(my_dataG4->movant=='P'){
			losser();
		}
	    
	    if(puedeMoverseArr(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme arriba
         numMov++; 
        }

       if(puedeMoverseAba(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme abajo
      	numMov++;
       }

       if(puedeMoverseDer(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme derecha
        numMov++;
       }

       if(puedeMoverseIzq(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme izquierda
       	numMov++;
       }
 	
 		int arrMov[numMov];

 		if(puedeMoverseArr(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme arriba
         arrMov[posMov] = 0;
         posMov++; 
        }

       if(puedeMoverseAba(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme abajo
      	arrMov[posMov] = 1;
         posMov++;
       }

       if(puedeMoverseDer(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme derecha
        arrMov[posMov] = 2;
         posMov++;
       }

       if(puedeMoverseIzq(my_dataG4->pos_x, my_dataG4->pos_y,'1','2','3')==1){ //Puedo moverme izquierda
       	arrMov[posMov] = 3;
         posMov++;
       }
    

  	int movFinal = genMov(numMov);

  	switch(arrMov[movFinal]){

  		case 0:
  		
  		setTablero(my_dataG4->pos_x, my_dataG4->pos_y,my_dataG4->movant );
        my_dataG4->pos_x--;
        my_dataG4->movant = matriz[my_dataG4->pos_x][my_dataG4->pos_y];
        usleep(90000);
		setTablero(my_dataG4->pos_x,my_dataG4->pos_y, my_dataG4->id_ghost);

  		break;

  		case 1:

  		setTablero(my_dataG4->pos_x, my_dataG4->pos_y,my_dataG4->movant );
        my_dataG4->pos_x++;
        my_dataG4->movant = matriz[my_dataG4->pos_x][my_dataG4->pos_y];
        usleep(90000);
		setTablero(my_dataG4->pos_x,my_dataG4->pos_y, my_dataG4->id_ghost);

  		break;

  		case 2:

  		setTablero(my_dataG4->pos_x, my_dataG4->pos_y,my_dataG4->movant );
        my_dataG4->pos_y++;
        my_dataG4->movant = matriz[my_dataG4->pos_x][my_dataG4->pos_y];
        usleep(90000);
		setTablero(my_dataG4->pos_x,my_dataG4->pos_y, my_dataG4->id_ghost);

  		break;

  		case 3:

  		setTablero(my_dataG4->pos_x, my_dataG4->pos_y,my_dataG4->movant );
        my_dataG4->pos_y--;
        my_dataG4->movant = matriz[my_dataG4->pos_x][my_dataG4->pos_y];
        usleep(90000);
		setTablero(my_dataG4->pos_x,my_dataG4->pos_y, my_dataG4->id_ghost);

  		break;

  	}
		
		sem_post(&ghos1);
		numMov=0;
		posMov=0;
    }
    pthread_exit(NULL);

	}
	
	int choqueGhost(int x, int y){
		
		if(matriz[x][y]=='1' || matriz[x][y]=='2' || matriz[x][y]=='3' || matriz[x][y]=='4'){
			return 1;
		}
		return 0;
	}
	
	void Youwin(){
			gameover=1;
			mvaddstr(ancho,alto+2,"¡Congratulations! Great job, You win!!!");
			refresh();
			sleep(5);
			clear();
			erase();
			endwin();
			pthread_exit(NULL);
	}
	
	void *pacman(void *threadarg){
		
	struct thread_data_pacman *my_dataPac;
	my_dataPac = (struct thread_data_pacman *) threadarg;
	
	my_dataPac->puntaje=0;
	
	 char opcion;
	 
	 if(choqueGhost(my_dataPac->pos_x,my_dataPac->pos_y)==1){
		losser(); 
	 }
	
	scanf("%c",&opcion);
		
		while(opcion!='q'){
		
		switch(opcion){
		
		case 'w':
		
		if(matriz[my_dataPac->pos_x-1][my_dataPac->pos_y]!='*'){
     
          if(choqueGhost(my_dataPac->pos_x-1,my_dataPac->pos_y)==1){
		   losser();
	     }
         
        if(matriz[my_dataPac->pos_x-1][my_dataPac->pos_y]=='.'){
			my_dataPac->puntaje++;
			char *point = (char *)malloc(6*sizeof(char));
			sprintf( point, "%d", my_dataPac->puntaje);
			mvaddstr(0,ancho+11,point);
			refresh();
		}
         
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, ' ');
        usleep(50000);
        my_dataPac->pos_x--;
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, my_dataPac->id_pac);
        
			if(my_dataPac->puntaje == total){
			   Youwin();
			} 
		}
		
		break;
		
		case 's':
		
		if(matriz[my_dataPac->pos_x+1][my_dataPac->pos_y]!='*'){
       
		if(choqueGhost(my_dataPac->pos_x+1,my_dataPac->pos_y)==1){
			losser();
		}
		
		 if(matriz[my_dataPac->pos_x+1][my_dataPac->pos_y]=='.'){
			my_dataPac->puntaje++;
			char *point = (char *)malloc(6*sizeof(char));
			sprintf( point, "%d", my_dataPac->puntaje);
			mvaddstr(0,ancho+11,point);
			refresh();
		}
		
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, ' ');
        usleep(50000);
        my_dataPac->pos_x++;
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, my_dataPac->id_pac);
        
		if(my_dataPac->puntaje == total){
			   Youwin();
		} 
       
		}
		
		break;
		
		case 'a':
		
		if(matriz[my_dataPac->pos_x][my_dataPac->pos_y-1]!='*'){
      
       if(choqueGhost(my_dataPac->pos_x,my_dataPac->pos_y-1)==1){
			losser();
		}
		
		  if(matriz[my_dataPac->pos_x][my_dataPac->pos_y-1]=='.'){
			my_dataPac->puntaje++;
			char *point = (char *)malloc(6*sizeof(char));
			sprintf( point, "%d", my_dataPac->puntaje);
			mvaddstr(0,ancho+11,point);
			refresh();
		   }
		
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, ' ');
        usleep(50000);
        my_dataPac->pos_y--;
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, my_dataPac->id_pac);
        
			if(my_dataPac->puntaje == total){
			   Youwin();
			} 		
		}
		
		break;
		
		case 'd':
		
		if(matriz[my_dataPac->pos_x][my_dataPac->pos_y+1]!='*'){
			
		if(choqueGhost(my_dataPac->pos_x,my_dataPac->pos_y+1)==1){
			losser();
		}
        
         if(matriz[my_dataPac->pos_x][my_dataPac->pos_y+1]=='.'){
			my_dataPac->puntaje++;
			char *point = (char *)malloc(6*sizeof(char));
			sprintf( point, "%d", my_dataPac->puntaje);
			mvaddstr(0,ancho+11,point);
			refresh();
		}
        
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, ' ');
        usleep(50000);
        my_dataPac->pos_y++;
        setTablero(my_dataPac->pos_x, my_dataPac->pos_y, my_dataPac->id_pac);
        
		if(my_dataPac->puntaje == total){
			   Youwin();
		} 
	  }
		break;
      }
		scanf("%c",&opcion);
    }
    pthread_exit(NULL);

	}
	
	int numPuntos(int ancho, int alto){
		
		int i,j,cont=0;
		
		for(i=0;i<alto;i++){
			for(j=0;j<ancho;j++){
				if(matriz[i][j]=='.'){
				cont++;
				}
			}
		}
		return cont;
	}
	
	
	int validarMapa(){
	
	int i,j;
	
	for(i=0;i<ancho;i++){
		if(matriz[0][i] != '*'){
			printf("Error, no esta cerrado el muro norte.\n");
			exit(1);
		}
	}
	
	for(j=0;j<ancho;j++){
		if(matriz[alto-1][j] != '*'){
			printf("Error, no esta cerrado el muro sur.\n");
			exit(1);
		}
	}
	
	for(i=0;i<alto;i++){
		if(matriz[i][0] != '*'){
			printf("Error, no esta cerrado el muro este.\n");
			exit(1);
		}
	}
	
	for(j=0;j<alto;j++){
		if(matriz[j][ancho-1] != '*'){
			printf("Error, no esta cerrado el muro oeste.\n");
			exit(1);
		}
	}
	
	 return 1;
	}

int main(int argc, char *argv[])
{

		pthread_t threads [5];
		
		ancho = atoi(argv[2]);
		alto = atoi(argv[1]);
		
		matriz = InitMatriz(matriz, argv[3]);
		int val = validarMapa();
		
		total = numPuntos(ancho,alto);

		total = (total+4);
		
		data_ghost1 = findPos(data_ghost1,matriz,ancho,alto,'1');
		data_ghost2 = findPos(data_ghost2,matriz,ancho,alto,'2');
		data_ghost3 = findPos(data_ghost3,matriz,ancho,alto,'3');
		data_ghost4 = findPos(data_ghost4,matriz,ancho,alto,'4');
		data_pacman = findPosPacman(data_pacman,matriz,ancho,alto);

		/*printf("%c, %d, %d\n %c, %d, %d\n %c, %d, %d\n %c, %d, %d\n %c, %d, %d\n",
		data_ghost1->id_ghost,data_ghost1->pos_x,data_ghost1->pos_y,data_ghost2->id_ghost,
		data_ghost2->pos_x,data_ghost2->pos_y,data_ghost3->id_ghost,data_ghost3->pos_x,
		data_ghost3->pos_y ,data_ghost4->id_ghost, data_ghost4->pos_x,data_ghost4->pos_y,
		data_pacman->id_pac, data_pacman->pos_x, data_pacman->pos_y );*/
  
	    //printf("Total %d\n",total); 	 
		 // Inicializacion de semaforos
				sem_init(&ghos1, 0, 1);
				sem_init(&ghos2, 0, 0);
				sem_init(&ghos3, 0, 0);
				sem_init(&ghos4, 0, 0);
				
				
		initscr();
		curs_set(0);
		 
		 initTablero();
		
			pthread_create( &threads[0], NULL, initGame1, (void *)data_ghost1);
		
			pthread_create( &threads[1], NULL, initGame2, (void *)data_ghost2);
			
			pthread_create( &threads[2], NULL, initGame3, (void *)data_ghost3);
			
			pthread_create( &threads[3], NULL, initGame4, (void *)data_ghost4);
			
			pthread_create( &threads[4], NULL, pacman, (void *)data_pacman);
			
			pthread_join(threads[0],NULL);
			pthread_join(threads[1],NULL);
			pthread_join(threads[2],NULL);
			pthread_join(threads[3],NULL);
			pthread_join(threads[4],NULL);
			
		clear();
		erase();
		endwin();
		exit(0);
}

char **InitMatriz(char **matriz, char *pfile){

/*	*file_d --> es el descriptor del archivo
	*	letter --> es un caracter que se usara para leer letra por letra el archivo
	*	*word --> es el arreglo de caracteres, es decir el que contiene una linea del archivo
	*	**paragraph --> es un arreglo que contiene palabras
	*	index --> posicion del arreglo de la variable [Word]
	*	position --> posicion de la matriz [paragraph]
	*/

	FILE *file_d;
	char letter, *word;
	int	 index = 0, position=0;
	
	/*
	*	Inicializacion de las variables [Word] y [paragraph] en una (1) posicion
	*	y luego se incrementara dinamicamente con Realloc
	*/

	word = (char*)malloc(1*sizeof(char));
	matriz = (char**)malloc(1*sizeof(char*));

	/* fopen --> Abrimos el archivo en modo lectura*/

	file_d = fopen(pfile, "r");

	/*	Mientras no sea el fin del archivo leemos caracter por caracter el contenido,
	*	mientras no se consiga un salto de linea '\n' se guardara en la variable Word que
	*	significa que se esta formado una palabra, en caso contrario cuando se consiga el
	*	salto de linea se guardara la palabra en un arreglo (es decir una posicion del arreglo contiene
	*	un arreglo que contiene caracteres)
	*/

	while (!feof(file_d)){	
			fscanf(file_d,"%c",&letter);  
                if(letter == '\n' ){
					/*
					*	Se expande la variable con Realloc, se finaliza con '\0' para eliminar basura
					*	Al final se libera e inicializa la variable [Word], La liberacion de apuntadores
					*	se puede hacer mediante el uso de la funcion free(void* Nombre_variable) o delete[] nombre_variable,
					*	en nuestro caso colocamos el contenido de [Word] en NULL
					*/
					word = (char*)realloc(word, (index+1)*sizeof(char));
					word[index] = '\0';
					matriz = (char**)realloc(matriz, (position+1)*sizeof(char*));
					matriz[position] = word;
					position++;
					index=0;
					word = NULL;
					word = (char*)malloc(1*sizeof(char));
				}else{
				   word = (char*)realloc(word,(index+1)*sizeof(char));
				   word[index] = letter;
				   index++;
				  
				}

	}
	fclose(file_d);

	return matriz;

}

 struct thread_data_ghost* findPos(struct thread_data_ghost *g1, char **matriz, int ancho, int alto, char id){
	 
	 int i,j;
	 
	 g1 = malloc(sizeof(struct thread_data_ghost));
	 
	 for(i=0;i<ancho;i++){
		for(j=0;j<alto;j++){
			
			if(matriz[i][j] == id){
				g1->pos_x = i;
				g1->pos_y = j;
				g1->id_ghost = id;
				return g1;
			}
			
		}
	 }
	 printf("Error, no se encontro el personaje: %c en el mapa. \n",id);
	 exit(0);
 }

 struct thread_data_pacman* findPosPacman(struct thread_data_pacman *pac, char **matriz, int ancho, int alto){
	 
	 int i,j;
	 
	 pac = malloc(sizeof(struct thread_data_pacman));
	 
	 for(i=0;i<ancho;i++){
		for(j=0;j<alto;j++){
			
			if(matriz[i][j] == 'P'){
				pac->pos_x = i;
				pac->pos_y = j;
				pac->id_pac = 'P';
				return pac;
			}
			
		}
	 }
	 printf("Error, no se encontro a Pacman en el mapa.\n");
	 exit(0);
 }
