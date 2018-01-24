#ifndef UTIL_TOKENIZER_DEF
#define UTIL_TOKENIZER_DEF
#include <iterator>
/*
 * Created by Marek Bielawski
 * Util::Collisions
 */
namespace Util {

class Tokenizer
{
	char** pointers_;
	unsigned int nPointers_;
	bool isRoot_;
	Tokenizer();
public:
	Tokenizer( char* data, const char* separators = " \n\t\r" );
	~Tokenizer() { if( isRoot_ ) delete [] pointers_; }
	char* operator[]( unsigned int i ) { return pointers_[i]; }
	unsigned int size() { return nPointers_; }
	unsigned int numberOf( const char* value );
	class Iterator {
		Tokenizer* tokenizer_ = nullptr;
		unsigned int index_ = 0;
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef char* value_type;
		typedef int difference_type;
		typedef char** pointer;
		typedef char*& reference;


		char* operator * () const { return (*tokenizer_)[ index_]; }
		Iterator operator++ () { ++index_; return *this; } 
		Iterator operator++ ( int ) { Iterator out = *this; ++index_; return out; }
		Iterator operator+ ( int val ) { Iterator out = *this; out.index_ += val; return out; }
		int operator-( const Iterator& val ) { return (int)index_ - (int)(val.index_); }
		bool operator == ( const Iterator& val ) { return tokenizer_ == val.tokenizer_ && index_ == val.index_; }
		bool operator != ( const Iterator& val ) { return tokenizer_ != val.tokenizer_ || index_ != val.index_; }
		Iterator( Tokenizer* tokenizer, unsigned int ind ) : tokenizer_( tokenizer ), index_( ind ) {  }
		unsigned int getIndex() const { return index_; }
	};

	Iterator find( const char * value, Iterator st, Iterator en );
	inline Iterator find( const char * value, Iterator st );
	inline Iterator find( const char * value );
	inline Iterator findNext( const char* value, Iterator cur );
	Tokenizer createChild( Iterator st, Iterator en );

};
inline Tokenizer::Iterator begin( Tokenizer& tokenizer ) { return Tokenizer::Iterator( &tokenizer, 0 ); }
inline Tokenizer::Iterator end( Tokenizer& tokenizer ) { return Tokenizer::Iterator( &tokenizer, tokenizer.size() ); }

inline Tokenizer::Iterator Tokenizer::find( const char* value, Tokenizer::Iterator st ) { return find( value, st, end( *this ) ); }
inline Tokenizer::Iterator Tokenizer::find( const char* value ) { return find( value, begin( *this ), end( *this ) ); }
inline Tokenizer::Iterator Tokenizer::findNext( const char* value, Tokenizer::Iterator cur )
{
	Tokenizer::Iterator next = cur;
	++next;
	return find( value, next );
}

inline Tokenizer Tokenizer::createChild(Iterator st, Iterator en)
{
	Tokenizer out;
	out.pointers_ = pointers_ + st.getIndex();
	out.nPointers_ = en - st;
	out.isRoot_ = false;
	return out;
}

}
#endif
