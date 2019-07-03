#include <LiquidCrystal.h>
#define CHAR_INTERVAL 500


enum Signal {
    DOT,
    DASH,
    SIGNAL_BREAK,
    LETTER_BREAK,
    WORD_BREAK,
    END,
    INVALID,
};

enum Phase {
    WAITING,
    RECEIVING,
    TRANSLATING,
    DISPLAYING,
};

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

unsigned char current_push = 0;

unsigned int current_time = 0;

Phase current_phase = WAITING;

Signal signals[ 500 ];

unsigned short last_signal_position = 0;

struct Node {
   char data = '^';
   struct Node *dot;
   struct Node *dash;
};

Node root;

char text[ 100 ];

void create_all_children(Node* root_node, char depth)
{
    root_node->dot = new Node;
    root_node->dash = new Node;

    --depth;

    if (depth > 0)
    {
        create_all_children(root_node->dot, depth);
        create_all_children(root_node->dash, depth);
    }
}

void setup()
{   
    create_all_children(&root, 4);
    
    root.dot->data = 'E';
    root.dash->data = 'T';

    root.dot->dot->data = 'I';
    root.dot->dash->data = 'A';
    root.dash->dot->data = 'N';
    root.dash->dash->data = 'M';

    root.dot->dot->dot->data = 'S';
    root.dot->dot->dash->data = 'U';
    root.dot->dash->dot->data = 'R';
    root.dot->dash->dash->data = 'W';
    root.dash->dot->dot->data = 'D';
    root.dash->dot->dash->data = 'K';
    root.dash->dash->dot->data = 'G';
    root.dash->dash->dash->data = 'O';
    
    root.dot->dot->dot->dot->data = 'H';
    root.dot->dot->dot->dash->data = 'V';
    root.dot->dot->dash->dot->data = 'F';
    root.dot->dash->dot->dot->data = 'L';
    root.dot->dash->dash->dash->data = 'J';
    root.dot->dash->dash->dot->data = 'P';
    root.dash->dot->dot->dot->data = 'B';
    root.dash->dot->dot->dash->data = 'X';
    root.dash->dot->dash->dot->data = 'C';
    root.dash->dot->dash->dash->data = 'Y';
    root.dash->dash->dot->dot->data = 'Z';
    root.dash->dash->dot->dash->data = 'Q';

    Serial.begin(9600);
    
    lcd.begin(16, 2);
}

void loop()
{
    if (current_phase == TRANSLATING)
    {
        translate();
    }
    else if (current_phase == DISPLAYING)
    {
        manage_scroll(text);
    }
    else if (current_phase == RECEIVING)
    {
        manage_receive();
    }
    else
    {
        check_if_started();
    }
}

void check_if_started()
{

}

void manage_receive()
{
    Signal signal = interpret_signal();

    signals[ last_signal_position ] = signal;

    ++last_signal_position;
}

Signal interpret_signal()
{

}

void translate()
{
    Node* current_node = &root;

    unsigned char last_character_position = 0;

    for (unsigned short i = 0; i < last_signal_position; ++i)
    {
        Signal s = signals[i];

        Serial.println(s);
        
        if (s == DOT)
        {
            current_node = current_node->dot;
        }
        else if (s == DASH)
        {
            current_node = current_node->dash;
        }
        else if (s == LETTER_BREAK)
        {
            text[ last_character_position ] = current_node->data;
            current_node = &root;
            ++last_character_position;
        }
        else if (s == WORD_BREAK)
        {
            text[ last_character_position ] = current_node->data;
            text[ ++last_character_position ] = ' ';
            ++last_character_position;
        }
        else if (s == INVALID)
        {
            text[ last_character_position ] = '^';
            ++last_character_position;
        }
    }

    last_signal_position = 0;
    current_phase = DISPLAYING;

    Serial.println(text);
}

// returns if text has ended or not
int scroll_text(const char* text)
{
    unsigned short text_length = strlen(text);

    if (current_push > 16 + text_length)
    {
        reset_text();
        return 1;
    }
    else if (current_push > 16)
    {
        lcd.setCursor(0, 0);
        
        for (int i = current_push - 16; i < text_length; ++i)
        {
            lcd.write(text[i]);
        }

        return 0;
    }
    else
    {
        lcd.setCursor(16 - current_push, 0);
        lcd.write(text);

        return 0;
    }
}

void manage_scroll(const char* text)
{
    if ((millis() - current_time) > CHAR_INTERVAL)
    {
        lcd.clear();
        ++current_push;
        current_time = millis();

        if (scroll_text(text))
            current_phase = WAITING;
    }
}

void reset_text()
{
    current_push = 0;
}
