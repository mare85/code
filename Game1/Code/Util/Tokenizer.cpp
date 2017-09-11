#include <Util/Tokenizer.h>
/*
 * Created by Marek Bielawski
 * Util::Collisions
 */
#include <string.h>
namespace Util {

Tokenizer::Tokenizer()
	: pointers_( nullptr )
	, nPointers_( 0 )
	, isRoot_( false )
{
}

Tokenizer::Tokenizer( char* data, const char* separators )
	: pointers_( nullptr )
	, nPointers_( 0 )
	, isRoot_( true )
{
size_t bufferLength = strlen( data );

	bool isSeparator = true;
	bool wasSeparator = true;
	size_t nSeparators = strlen( separators );

	// counting tokens
	for( size_t i = 0; i != bufferLength; ++i )
	{
		char c = data[ i ];
		isSeparator = false;
		for( size_t si = 0; si != nSeparators; ++si )
			isSeparator |= ( separators[ si ] == c );
		if( isSeparator )
			data[ i ] = 0;
		else if( wasSeparator )
			++nPointers_;
		wasSeparator = isSeparator;
	}

	if( ! nPointers_ )
		return;

	pointers_ = new char*[ nPointers_ ];

	// finding starts
	unsigned int nStarts = 0;
	wasSeparator = true;

	for( size_t i = 0; i != bufferLength; ++i )
	{
		isSeparator = ( data[ i ] == 0 );

		if( wasSeparator && ! isSeparator )
		{
			pointers_[ nStarts ] = data + i;
			++nStarts;
		}

		wasSeparator = isSeparator;
	}
}

unsigned int Tokenizer::numberOf( const char* value )
{
	unsigned int outVal = 0;
	
	for( char* token : *this )
	{
		if( strcmp( token, value ) == 0 )
			outVal++;
	}

	return outVal;
}

Tokenizer::Iterator Tokenizer::find(const char * value, Tokenizer::Iterator st, Tokenizer::Iterator en)
{
	for( Iterator it = st; it != en; ++it )
	{
		if( strcmp( *it, value ) == 0 )
			return it;
	}
	return en;
}

}
