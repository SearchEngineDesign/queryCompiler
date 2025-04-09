#pragma once

#include "tokenstream.h"

class Tuple
{
public:
Tuple *Next;
    virtual int Eval( );
    Tuple( );
    virtual ~Tuple( );
};

class TupleList : Tuple
{
public:
    Tuple *Top,
    *Bottom;
    void Empty( );
    void Append( Tuple *t );
    TupleList( );
    ~TupleLIst( );  
};

class OrNode : TupleList {
    public:
    OrNode(TupleList *l, TupleList *r) {
        Top = l->Top;
        
    }
}   