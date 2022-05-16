const int nligne=4;
const int ncol=4;
//Lignes 9-12
//Colonnes:LOW->Allume 1-4:Orange
//                   5-8:Vert
int i=0;
int lign=0;
int col=0;
int coul=0;

void setup() {
  for(i=1;i<14;i++){
    pinMode(i,OUTPUT);
  }
  for(i=1;i<9;i++){
  digitalWrite(i,HIGH);
  }
}
bool mat[][nligne][ncol]={{
  {1,1,1,0},
  {1,0,0,0},
  {1,0,0,0},
  {0,0,0,0}}
  ,{
  {0,0,0,0},
  {0,0,0,1},
  {0,0,0,1},
  {0,1,1,1}}};
void loop() {
  for(int t=0;t<2;t++){
    for(int lign=0;lign<nligne;lign++){
      for(int col=0;col<ncol;col++){
        if(mat[t][lign][col]){//Si sa position dans la matrice est true
          //Allume la led
          digitalWrite(12-lign,HIGH);
          digitalWrite(8-col+coul*4,LOW);
        }
      }
      delay(2);
      //Éteint les leds
      for(int col=0;col<ncol;col++){
        digitalWrite(12-lign,LOW);
        digitalWrite(8-col+coul*4,HIGH);
      }
    }
    delay(100);
  }
  /*lign=lign%nligne;
  col=col%ncol;
  coul=coul%2;*/
}
