#include <Arduino.h>
#include <BMP180.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <custom_chars.h>
#include <RotaryEncoder.h>

#define printByte(args)  write(args);
#define DHTTYPE DHT11
#define DHTPIN 6
#define SWITCH 3

#define DEGREE          0
#define PLUS_MINUS      1
#define CAP             0
#define BOTTOM          1
#define RU_STEM         2
#define RL_STEM         3
#define LU_STEM         4
#define LL_STEM         5
#define CAP_E           6
#define BOTTOM_E        7
#define C               10

LiquidCrystal_I2C lcd(0x27,20,4);
BMP180 myBMP(BMP180_ULTRAHIGHRES);
DHT dht(DHTPIN, DHTTYPE);

RotaryEncoder encoder(4, 5);
unsigned long timer;
unsigned long timer_1;
int state = 0;

/* 
 * menu state
 * 0 off
 * 1 main menu
 * 11 main sub menu 1
 * 12 main sub menu 2 ...
 *
 */
int8_t position_in_menu = 0;
volatile _Bool on_sw = false;
_Bool show_all_data = true;
_Bool main_m = false;
_Bool settings_m = false;
_Bool debug_m = false;
int data = 1;

int enc_pos = 0; /* encoder position */
int new_enc_pos = 0; /* changed enboder position */

void state_zero();
void state_one();
void state_two();
void state_three();
void print_str(String str, int col, int line);
void print_digit(int digit, int col);
void ser_print();
void on_switch();
void main_menu();
void settings_menu();
void debug_menu();
void check_mpos();
void draw_main_menu(int position_in_menu);
void draw_settings_menu(int position_in_menu);
void draw_debug_menu();
void show_single(int data);
void start_menu();

void setup()
{
        Serial.begin(9600);
	Serial.println("Setting up...");
	lcd.init();
	lcd.backlight();
	/* creating custom char which is defined in custom_char.h */
	lcd.createChar(0, degree);
	lcd.createChar(1, plus_minus);
	attachInterrupt(digitalPinToInterrupt(SWITCH), on_switch, FALLING);
	
        dht.begin();
	myBMP.begin();
}

void loop()
{
	encoder.tick();  
        new_enc_pos = encoder.getPosition();
	if (data == 4) data = 1;
	if(millis() - timer >= 3000) {
                timer = millis();
		
		if (show_all_data) {
			state_zero();
		} else {
			show_single(data++);
		}
		
		ser_print(); /* put all data in serial */
	}
	start_menu();
}

void start_menu()
{
	if (main_m) main_menu();
	if (settings_m) settings_menu();
	if (debug_m) debug_menu();
}

void main_menu()
{
	position_in_menu = 1;
	draw_main_menu(position_in_menu);
	for (;;) {
		/*Serial.print(enc_pos); Serial.print(" -> ");
		Serial.println(new_enc_pos);*/
		encoder.tick();
		enc_pos = new_enc_pos;
		new_enc_pos = encoder.getPosition();

		if (new_enc_pos > enc_pos) { /* going up */
			position_in_menu--;
			check_mpos();
			draw_main_menu(position_in_menu);
			enc_pos = new_enc_pos;
		} else if (new_enc_pos < enc_pos) { /*going down */
			position_in_menu++;
			check_mpos();
			draw_main_menu(position_in_menu);
			enc_pos = new_enc_pos;
		}

		if (on_sw) {
			if (position_in_menu == 1) { /* settings */
				on_sw = false;
				settings_m = true;
				position_in_menu = 0;
				//settings_menu();
				return;
			} else if (position_in_menu == 2) { /* Debug */
				debug_m = true;
				on_sw = false;
				position_in_menu = 0;
				//debug_menu();
				return;
			} else if (position_in_menu == 3) { /* exit */
				main_m = false;
				on_sw = false;
				position_in_menu = 0;
				return;
			}
		}
	}
}

void settings_menu()
{
	position_in_menu = 1;
	draw_settings_menu(position_in_menu);
	for (;;) {
		encoder.tick();
		enc_pos = new_enc_pos;
		new_enc_pos = encoder.getPosition();

		if (new_enc_pos > enc_pos) { /* going up */
			position_in_menu--;
			check_mpos();
			draw_settings_menu(position_in_menu);
			enc_pos = new_enc_pos;
		} else if (new_enc_pos < enc_pos) { /*going down */
			position_in_menu++;
			check_mpos();
			draw_settings_menu(position_in_menu);
			enc_pos = new_enc_pos;
		}

		if (on_sw) {
			if (position_in_menu == 1) { /* show all */
				on_sw = false;
				show_all_data = !show_all_data;
				draw_settings_menu(position_in_menu);
			} else if (position_in_menu == 2) { /* show sngl */
				on_sw = false;
				show_all_data = !show_all_data;
				draw_settings_menu(position_in_menu);
			} else if (position_in_menu == 3) { /* exit */
				settings_m = false;
				main_m = true;
				on_sw = false;
				return;
			}
		}
	}
}

void debug_menu()
{
	draw_debug_menu();
	for (;;) {
		if (on_sw) {
			debug_m = false;
			on_sw = false;
			return;
		}
	}
}

void draw_main_menu(int position_in_menu)
{
	print_str("Main menu:          ", 0, 0);
	print_str((position_in_menu == 1 ? ">Settings           " :
		   "Settings            "), 0, 1);
	print_str((position_in_menu == 2 ? ">Debug              " :
		   "Debug               "), 0, 2);
	print_str((position_in_menu == 3 ? ">Exit menu          " :
		   "Exit menu           "), 0, 3);
}

void draw_settings_menu(int position_in_menu)
{
	print_str("Settings menu:          ", 0, 0);
	if (show_all_data) {
		print_str((position_in_menu == 1 ? ">Show all data    on" :
		   "Show all data     on"), 0, 1);
		print_str((position_in_menu == 2 ? ">Show sngl. data off" :
		   "Show sngl. data  off"), 0, 2);
	} else {
		print_str((position_in_menu == 1 ? ">Show all data   off" :
		   "Show all data    off"), 0, 1);
		print_str((position_in_menu == 2 ? ">Show single data on" :
		   "Show single data  on"), 0, 2);
	}
	print_str((position_in_menu == 3 ? ">Exit menu          " :
		   "Exit menu           "), 0, 3);
}

void draw_debug_menu()
{
	print_str("Debug menu:         ", 0, 0);
	print_str("It's a place holder!", 0, 1);
	print_str("Be nice!    =D      ", 0, 2);
	print_str(">Exit menu          ", 0, 3);
}

void check_mpos()
{
	if (position_in_menu > 3) {
		position_in_menu = 1;
	} else if (position_in_menu < 1) {
		position_in_menu = 3;
	}
}

void on_switch()
{
	if (!main_m) {
		main_m = true;
	} else if (main_m || settings_m || debug_m) {
		on_sw = true;
	}
}

void state_zero ()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.println("Meteostat statistics"); 
        lcd.createChar(0, degree);
        lcd.createChar(1, plus_minus);
	lcd.setCursor(0, 1);
	lcd.print("Temp....");
	lcd.print((int)myBMP.getTemperature());
	lcd.printByte(PLUS_MINUS); lcd.print("1 "); lcd.printByte(DEGREE);
                lcd.print("C");

	lcd.setCursor(0, 2);
	lcd.print("Press...");
	lcd.print(myBMP.getPressure() / 133.3224);
	lcd.print(" mmHg");

	lcd.setCursor(0, 3);
	lcd.print("Humid...");
	lcd.print((int)dht.readHumidity());
	lcd.print("%");
}

void state_one()
{
	lcd.clear();
        int t = 0;
        int pos = 5;
        int last = 0;
        int first = 0;

        t = myBMP.getTemperature();
        last = t % 10;
        t /= 10;
        first = t % 10;

        print_str("Temperature:        ", 0, 0);
        print_digit(first, pos);
        print_digit(last, pos + 4);
        lcd.setCursor(pos + 8, 1);
        lcd.print("o");
        print_digit(C, pos + 9);
}

void state_two()
{
	lcd.clear();
        float p = 0;
        int pos = 5;

        p = myBMP.getPressure() / 133.3224;
        print_str("Pressure:           ", 0, 0);
        print_digit((int)(p / 100) % 10, pos);
        print_digit((int)(p / 10) % 10, pos + 4);
        print_digit((int)p % 10, pos + 8);
        lcd.setCursor(16, 2);
        lcd.print("mmHg");
}

void state_three()
{
	lcd.clear();
        int h = 0;
        int pos = 5;
        int last = 0;
        int first = 0;

        h = dht.readHumidity();
        last = h % 10;
        h /= 10;
        first = h % 10;

        print_str("Humidity:           ", 0, 0);
        print_digit(first, pos);
        print_digit(last, pos + 4);
        lcd.setCursor(pos + 8, 2);
        lcd.print("%");
}
void show_single(int data) {
	switch (data) {
	case 1:
		state_one();
		break;
	case 2:	       
		state_two();
		break;
	case 3:
		state_three();
	}
}

void ser_print()
{
	Serial.print(F("Temperature.......: "));
	  Serial.print(myBMP.getTemperature(), 1);
	    Serial.println(F(" +-1.0C"));

	Serial.print(F("Pressure..........: "));
	  Serial.print(myBMP.getPressure() / 133.3224);
	    Serial.println(F(" mm Hg"));
	    
	Serial.print(F("Sea level pressure: "));
	  Serial.print(myBMP.getSeaLevelPressure(115) / 133.3224);
	    Serial.println(F(" mm Hg"));
	    
	Serial.print(F("Humidity: "));
	  Serial.print(dht.readHumidity());
	    Serial.println(F(" %"));
}

void print_str(String str, int col, int line)
{
	lcd.setCursor(col, line);
	lcd.print(str);
}

void print_digit(int digit, int col)
{
        lcd.createChar(0, cap);
        lcd.createChar(1, bottom);
        lcd.createChar(2, ru_stem);
        lcd.createChar(3, rl_stem);
        lcd.createChar(4, lu_stem);
        lcd.createChar(5, ll_stem);
        lcd.createChar(6, cap_e);
        lcd.createChar(7, bottom_e);

        switch (digit)
        {
        case 0:
                lcd.setCursor(col, 1);
                lcd.printByte(LU_STEM);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(RU_STEM);

                lcd.setCursor(col, 2);
                lcd.printByte(LL_STEM);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 1:
                lcd.setCursor(col, 1);
                lcd.printByte(CAP);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(RU_STEM);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(RL_STEM);
                break;
        case 2:
                lcd.setCursor(col, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(RU_STEM);

                lcd.setCursor(col, 2);
                lcd.printByte(LL_STEM);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(BOTTOM_E);
                break;
        case 3:
                lcd.setCursor(col, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(RU_STEM);

                lcd.setCursor(col, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 4:
                lcd.setCursor(col, 1);
                lcd.printByte(LL_STEM);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(BOTTOM);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(RU_STEM);

                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 5:
                lcd.setCursor(col, 1);
                lcd.printByte(LU_STEM);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(CAP_E);

                lcd.setCursor(col, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 6:
                lcd.setCursor(col, 1);
                lcd.printByte(LU_STEM);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(CAP_E);

                lcd.setCursor(col, 2);
                lcd.printByte(LL_STEM);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 7:
                lcd.setCursor(col, 1);
                lcd.printByte(CAP);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(RU_STEM);

                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 8:
                lcd.setCursor(col, 1);
                lcd.printByte(LU_STEM);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(RU_STEM);

                lcd.setCursor(col, 2);
                lcd.printByte(LL_STEM);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 9:
                lcd.setCursor(col, 1);
                lcd.printByte(LU_STEM);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP_E);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(RU_STEM);

                lcd.setCursor(col, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM_E);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(RL_STEM);
                break;
        case 10:
                lcd.setCursor(col, 1);
                lcd.printByte(LU_STEM);
                lcd.setCursor(col + 1, 1);
                lcd.printByte(CAP);
                lcd.setCursor(col + 2, 1);
                lcd.printByte(CAP);

                lcd.setCursor(col, 2);
                lcd.printByte(LL_STEM);
                lcd.setCursor(col + 1, 2);
                lcd.printByte(BOTTOM);
                lcd.setCursor(col + 2, 2);
                lcd.printByte(BOTTOM);
                break;
        
        default:
                break;
        }
}
