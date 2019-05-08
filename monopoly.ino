#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Imposta l'indirizzo del display a 0x27 con 16 caratteri per 2 linee
LiquidCrystal_I2C lcd(0x27,16,2);  

uint8_t euro[8] = {0x3,0x4,0x8,0x1f,0x8,0x1f,0x4,0x3};

const byte ROWS = 4; //4 righe
const byte COLS = 3; //3 colonne
char test;
int n_giocatori = 0;
long soldi_iniziali = 0;
long soldi_giocatori[10];


char keys[ROWS][COLS] = 
{
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {8, 7, 6, 5}; // Pin di arduino connessi ai pin 1,2,3 e 4 delle righe del keypad
byte colPins[COLS] = {4, 3, 2};    // Pin di arduino connessi ai pin 5,6,7 delle righe del keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void stampa_msg(String msg,int line){
  Serial.println(msg);
  lcd.setCursor(0, line);
  lcd.print(msg);
}

void stampa_menu(){
  lcd.setCursor(0, 1);
  lcd.print('*');
  lcd.setCursor(15, 1);
  lcd.print('#');
}

int get_n_giocatori(){
  lcd.clear();
  stampa_msg("Numero giocatori:",0);
  stampa_menu();
  String n_player = "";
  while(1){
    test = keypad.waitForKey();
    if(test == '#') break; 
    if(test == '*') {
      n_player = n_player.substring(0, n_player.length()-1); 
      lcd.setCursor(n_player.length()+2, 1);
      lcd.print(' ');
      lcd.setCursor(0, 1);
    }
    else{
      if(n_player.length() < 12)
        n_player = String(n_player + test);
      
    }
    stampa_msg("* "+String(n_player),1);   
  }
  lcd.clear();
  Serial.println(n_player.toInt());
  if(n_player.toInt()>0 && n_player.toInt()<10 && n_player.length() < 10)
    return n_player.toInt();
  else{
    stampa_msg("    ERROR!!!",0);
    delay(1000);
    return get_n_giocatori();
  
  }
 
}

long get_soldi_iniziali(){
  lcd.clear();
  stampa_msg("Soldi iniziali:",0);
  stampa_menu();
  String money_init = "";
  while(1){
    test = keypad.waitForKey();
    if(test == '#') break; 
    if(test == '*') {
      money_init = money_init.substring(0, money_init.length()-1); 
      lcd.setCursor(money_init.length()+2, 1);
      lcd.print(' ');
      lcd.setCursor(0, 1);
    }
    else{
      if(money_init.length() < 10)
        money_init = String(money_init + test);
    }
    stampa_msg("* "+String(money_init),1);   
  }
  lcd.clear();
  Serial.println(money_init.toInt());
  if(money_init.toInt()> 100)
    return money_init.toInt();
  else{
    stampa_msg("    ERROR!!!",0);
    delay(1000);
    return get_soldi_iniziali();
  
  }
 
}

void visualizza_soldi_giocatore(int giocatore){
  lcd.clear();
  lcd.setCursor(0, 0);
  
  if(soldi_giocatori[giocatore] != 0){
    stampa_msg(String(giocatore)+": "+soldi_giocatori[giocatore],0);
    lcd.write(0);
    stampa_menu();
  }else{
    stampa_msg(String(giocatore)+": BANCAROTTA !!",0);
    lcd.setCursor(0, 1);
    lcd.print('*');
  }
  
  
  while(1){
    test = keypad.waitForKey();
    if(test == '#' && soldi_giocatori[giocatore] != 0){ transazione(giocatore);break;} 
    if(test == '*') break;     
  }
}

void transazione(int giocatore_1){
  lcd.clear();
  stampa_menu();
  lcd.cursor();
  
  if(giocatore_1 != 0){
    //GIOCATORE
    stampa_msg("   "+String(giocatore_1)+"   ->   ",0);
  }
  else{
    //BANCA
    stampa_msg(" BANCA ->  ",0);
  }

  String s_giocatore = "99";
  
  while(1){
    lcd.setCursor(12, 0);
    test = keypad.waitForKey();
    if(test == '#'){
      if(s_giocatore.toInt() != giocatore_1 && s_giocatore.toInt()<=n_giocatori){
        break;
      }
      lcd.setCursor(10, 0);
      lcd.print("     ");
      continue;
    } 
    if(test == '*') {return;}
    lcd.setCursor(10, 0);
    lcd.print("     ");

    
    s_giocatore = String(test);
    if(test == '0'){
      lcd.setCursor(10, 0);
      lcd.print("BANCA");
    }
    else{
        lcd.setCursor(12, 0);
        lcd.print(s_giocatore);     
    }
    
  }
  
  int giocatore_2 = s_giocatore.toInt();
  Serial.println(giocatore_2);


  String s_money = "";
  while(1){
    if(s_money.length()==0){
      lcd.cursor();
      lcd.setCursor(s_money.length()+2, 1);
    }else{
      lcd.noCursor();
    }
    
    test = keypad.waitForKey();
    if(test == '#') {
      if(s_money.length() != 0)
        break; 
      continue;
    }
    if(test == '*') {
      s_money = s_money.substring(0, s_money.length()-1); 
      lcd.setCursor(s_money.length()+2, 1);
      lcd.print(' ');
    }
    else{
      if(s_money.length() < 10){
        s_money = String(s_money + test);
      }
    }
    stampa_msg("* "+String(s_money),1);   
  }

  long soldi = s_money.toInt();
  Serial.println(soldi);

  if(soldi_giocatori[giocatore_2]>2147483646 - soldi){
    lcd.clear();
    stampa_msg("   OPERAZIONE",0);
    stampa_msg("  NON RIUSCITA",1);
    delay(2000);
    transazione(giocatore_1);
    return;
    
  }
  
  if(giocatore_1 != 0)
    soldi_giocatori[giocatore_1] = soldi_giocatori[giocatore_1] - soldi;
    
  if(giocatore_2 != 0)  
    soldi_giocatori[giocatore_2] = soldi_giocatori[giocatore_2] + soldi;
  
  lcd.clear();
  stampa_msg("   OPERAZIONE",0);
  stampa_msg("    RIUSCITA",1);
  delay(2000);  


}

void setup()
{
  
  Serial.begin(9600);
  lcd.init();// inizializza il display
  lcd.backlight();//accende la retroilluminazione

  lcd.createChar(0, euro);
  
  stampa_msg("   Monopoly!!",0);
  
  stampa_msg("# per continuare ",1);
  

  while(keypad.getKey() != '#');
  lcd.clear();
  
  n_giocatori = get_n_giocatori();
  //Serial.println(n_giocatori);
  soldi_iniziali = get_soldi_iniziali();

  Serial.println("Giocatori : "+String(n_giocatori));
  Serial.println("Soldi iniziali : "+String(soldi_iniziali));
  
  for(int a = 1; a < 10 ; a = a + 1){
    soldi_giocatori[a] = soldi_iniziali;
  }
  
  stampa_msg("Gioco avviato",0);

  delay(2000);
  
  
}
  
void loop(){
  stampa_msg("   Monopoly!!",0);
  stampa_msg("Giocatore: ",1);

  test = keypad.waitForKey();
  if(test != '0' && test != '*' && test != '#' && (String(test).toInt()<= n_giocatori)){
    visualizza_soldi_giocatore(String(test).toInt());   
  }
  if(test == '0'){
    transazione(0);
  }

  lcd.clear();

  for(int a = 1; a< 10; a++){
    if(soldi_giocatori[a] < 0){
      soldi_giocatori[a] = 0;
      stampa_msg("Giocatore "+String(a),0);
      stampa_msg("BANCAROTTA !!",1);
      delay(5000);
      lcd.clear();
    }
  }
  
}







