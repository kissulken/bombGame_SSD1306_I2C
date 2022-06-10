#include <TrueRandom.h>
#include <OLED_I2C.h>
#include <LiquidCrystal_I2C.h>
#define button_pin 8
#define buzzer_pin 6
OLED myOLED(SDA, SCL);
LiquidCrystal_I2C lcd(0x3F, 16, 2);
extern uint8_t MegaNumbers[];
extern uint8_t MediumNumbers[];
extern uint8_t SmallFont[];
extern uint8_t RusFont[];
byte players = 2, count = 1, j = 0, k = 0, current_player = 0, win_index;
bool flag = true, fin = false;
short my_millis, target, results[3], max, press_count = 0, perc = 42, my_millis_now;

void setup()
{
    Serial.begin(9600);
    myOLED.begin();
    lcd.init();
    lcd.backlight();
    Greetings();
    target = TrueRandom.random(1000, 7000);
}

void loop()
{
    myOLED.setFont(MediumNumbers);
    myOLED.printNumI(target, RIGHT, 0);
    myOLED.update();
    myOLED.setFont(MegaNumbers);
    my_millis = millis();
    while (digitalRead(button_pin))
    {
        fin = true;
        myOLED.printNumI(millis() - my_millis, CENTER, 25);
        results[current_player] = millis() - my_millis - 25;
        myOLED.update();
        if (millis() - my_millis % perc)
        {
            tone(6, 200);
            perc--;
        }
        if (!digitalRead(button_pin))
            delay(20);
        if (millis() - my_millis >= target)
        {
            BOOM();
            results[current_player] = -1;
            break;
        }
        if (millis() - my_millis % perc)
            noTone(6);
    }
    if (fin)
    {
        result(current_player, results[current_player]);
        if (current_player == players - 1)
            winner();
        current_player++;
        fin = false;
        Greetings();
    }
}

void SetPlayersCount()
{
    myOLED.setFont(RusFont);
    myOLED.print("Rjkbxtcndj buhjrjd", CENTER, 8);
    myOLED.setFont(MegaNumbers);
    myOLED.printNumI(players, CENTER, 25);
    myOLED.update();
    while (true)
    {
        if (digitalRead(button_pin))
        {
            my_millis = millis();
            while (digitalRead(button_pin));
            if (millis() - my_millis < 2000)
            {
                if (press_count % 2)
                    players = 2;
                else
                    players = 3;
                press_count++;
                myOLED.printNumI(players, CENTER, 25);
                myOLED.update();
                delay(50);
            }
            else
                break;
        }
    }
    myOLED.clrScr();
}

void Greetings()
{
    myOLED.clrScr();
    if (flag)
        SetPlayersCount();
    myOLED.setFont(SmallFont);
    myOLED.print("TARGET:", LEFT, 0);
    myOLED.setFont(MegaNumbers);
    myOLED.printNumI(0, CENTER, 25);
    myOLED.update();
    if (flag)
    {
        lcd.setCursor(4, 0);
        lcd.print("DA_BOMB!");
        lcd.setCursor(3, 1);
        lcd.print("GET READY!");
        lcd.clear();
        for (int i = 0; i < players; i++)
        {
            lcd.setCursor(i * 6, 0);
            lcd.print("PL");
            lcd.setCursor(i * 6 + 2, 0);
            lcd.print(count);
            lcd.setCursor(i * 6 + 3, 0);
            lcd.print(':');
            count++;
        }
        flag = false;
    }
}

void BOOM()
{
    myOLED.fillScr();
    myOLED.update();
    tone(6, 65);
    delay(3000);
    noTone(6);
    myOLED.clrScr();
    Greetings();
}

void result(int pos, int res)
{
    lcd.setCursor(pos * 6, 1);
    if (res == -1)
        lcd.print("BOOM");
    else
        lcd.print(res);
}

void winner()
{
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("THE WINNER IS:");
    lcd.setCursor(3, 1);
    lcd.print("PLAYER ");
    max = results[0];
    for (int i = 0; i < players; i++)
    {
        if (results[i] > max)
        {
            max = results[i];
            win_index = i;
        }
    }
    lcd.setCursor(10, 1);
    lcd.print(win_index + 1);
    exit(0);
}
