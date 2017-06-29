#include "Constants.h"

char Input::Key::getChar(Keycode code, bool shift)
{
	char c = 0;
	switch (code)
	{
	case Q: c = shift ? 'Q' : 'q'; break;
	case W: c = shift ? 'W' : 'w'; break;
	case E: c = shift ? 'E' : 'e'; break;
	case R: c = shift ? 'R' : 'r'; break;
	case T: c = shift ? 'T' : 't'; break;
	case Y: c = shift ? 'Y' : 'y'; break;
	case U: c = shift ? 'U' : 'u'; break;
	case I: c = shift ? 'I' : 'i'; break;
	case O: c = shift ? 'O' : 'o'; break;
	case P: c = shift ? 'P' : 'p'; break;
	case A: c = shift ? 'A' : 'a'; break;
	case S: c = shift ? 'S' : 's'; break;
	case D: c = shift ? 'D' : 'd'; break;
	case F: c = shift ? 'F' : 'f'; break;
	case G: c = shift ? 'G' : 'g'; break;
	case H: c = shift ? 'H' : 'h'; break;
	case J: c = shift ? 'J' : 'j'; break;
	case K: c = shift ? 'K' : 'k'; break;
	case L: c = shift ? 'L' : 'l'; break;
	case Z: c = shift ? 'Z' : 'z'; break;
	case X: c = shift ? 'X' : 'x'; break;
	case C: c = shift ? 'C' : 'c'; break;
	case V: c = shift ? 'V' :' v'; break;
	case B: c = shift ? 'B' :' b'; break;
	case N: c = shift ? 'N' : 'n'; break;
	case M: c = shift ? 'M' : 'm'; break;
	case NO0: c = shift ? ')' : '0'; break;
	case NO1: c = shift ? '!' : '1'; break;
	case NO2: c = shift ? '@' : '2'; break;
	case NO3: c = shift ? '#' : '3'; break;
	case NO4: c = shift ? '$' : '4'; break;
	case NO5: c = shift ? '%' : '5'; break;
	case NO6: c = shift ? '^' : '6'; break;
	case NO7: c = shift ? '&' : '7'; break;
	case NO8: c = shift ? '*' : '8'; break;
	case NO9: c = shift ? '(' : '9'; break;
	case MINUS: c = shift ? '_' : '-'; break;
	case PLUS: c = shift ? '+' : '='; break;
	case LBRACKET: c = shift ? '{' : '['; break;
	case RBRACKET: c = shift ? '}' : ']'; break;
	case BACKSPACE: c = shift ? '|' : '\\'; break;
	case COLON: c = shift ? ':' : ';'; break;
	case QUOTATION_MARK: c = shift ? '\"' : '\''; break;
	case COMA: c = shift ? '<' : ','; break;
	case DOT: c = shift ? '>' : '.'; break;
	case SLASH: c = shift ? '?' : '/'; break;
	}
	return c;
}
