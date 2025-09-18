enum arrow_key {
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
}; 

void init_termios(void);
int read_arrow_key_in(void);