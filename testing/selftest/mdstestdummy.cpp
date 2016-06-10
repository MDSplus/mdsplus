
#include <assert.h>
#include "mdstestdummy.h"



void generate_false_assert() {
    assert( "user generated false assert"&&0 );
}

void generate_true_assert() {
    assert( "user generated true assert"||1 );
}
