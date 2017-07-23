#include "Constants.h"

char Input::getKeyChar(Key code, bool shift)
{
	char c = 0;
	switch (code)
	{
	case Key::Q: c = shift ? 'Q' : 'q'; break;
	case Key::W: c = shift ? 'W' : 'w'; break;
	case Key::E: c = shift ? 'E' : 'e'; break;
	case Key::R: c = shift ? 'R' : 'r'; break;
	case Key::T: c = shift ? 'T' : 't'; break;
	case Key::Y: c = shift ? 'Y' : 'y'; break;
	case Key::U: c = shift ? 'U' : 'u'; break;
	case Key::I: c = shift ? 'I' : 'i'; break;
	case Key::O: c = shift ? 'O' : 'o'; break;
	case Key::P: c = shift ? 'P' : 'p'; break;
	case Key::A: c = shift ? 'A' : 'a'; break;
	case Key::S: c = shift ? 'S' : 's'; break;
	case Key::D: c = shift ? 'D' : 'd'; break;
	case Key::F: c = shift ? 'F' : 'f'; break;
	case Key::G: c = shift ? 'G' : 'g'; break;
	case Key::H: c = shift ? 'H' : 'h'; break;
	case Key::J: c = shift ? 'J' : 'j'; break;
	case Key::K: c = shift ? 'K' : 'k'; break;
	case Key::L: c = shift ? 'L' : 'l'; break;
	case Key::Z: c = shift ? 'Z' : 'z'; break;
	case Key::X: c = shift ? 'X' : 'x'; break;
	case Key::C: c = shift ? 'C' : 'c'; break;
	case Key::V: c = shift ? 'V' : 'v'; break;
	case Key::B: c = shift ? 'B' : 'b'; break;
	case Key::N: c = shift ? 'N' : 'n'; break;
	case Key::M: c = shift ? 'M' : 'm'; break;
	case Key::NO0: c = shift ? ')' : '0'; break;
	case Key::NO1: c = shift ? '!' : '1'; break;
	case Key::NO2: c = shift ? '@' : '2'; break;
	case Key::NO3: c = shift ? '#' : '3'; break;
	case Key::NO4: c = shift ? '$' : '4'; break;
	case Key::NO5: c = shift ? '%' : '5'; break;
	case Key::NO6: c = shift ? '^' : '6'; break;
	case Key::NO7: c = shift ? '&' : '7'; break;
	case Key::NO8: c = shift ? '*' : '8'; break;
	case Key::NO9: c = shift ? '(' : '9'; break;
	case Key::MINUS: c = shift ? '_' : '-'; break;
	case Key::PLUS: c = shift ? '+' : '='; break;
	case Key::LBRACKET: c = shift ? '{' : '['; break;
	case Key::RBRACKET: c = shift ? '}' : ']'; break;
	case Key::BACKSPACE: c = shift ? '|' : '\\'; break;
	case Key::COLON: c = shift ? ':' : ';'; break;
	case Key::QUOTATION_MARK: c = shift ? '\"' : '\''; break;
	case Key::COMA: c = shift ? '<' : ','; break;
	case Key::DOT: c = shift ? '>' : '.'; break;
	case Key::SLASH: c = shift ? '?' : '/'; break;
	}
	return c;
}
