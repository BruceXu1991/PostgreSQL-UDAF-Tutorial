#include <float.h>
#include "c.h"
#include "postgres.h"
#include "fmgr.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


typedef struct
{
    float count;
    float sum;
} trans_obj;

int isNaN(float x) {
    return x != x; 
}

PG_FUNCTION_INFO_V1(trans_obj_in);

// Input function
Datum
trans_obj_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	float		count,
			sum;
	trans_obj    *result;

	if (sscanf(str, "%f,%f", &count, &sum) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for trans_obj: \"%s\"",
						str)));

	result = (trans_obj *) palloc(sizeof(trans_obj));
	result->count = count;
	result->sum = sum;
	PG_RETURN_POINTER(result);
}

// Output function
PG_FUNCTION_INFO_V1(trans_obj_out);

Datum
trans_obj_out(PG_FUNCTION_ARGS)
{
	trans_obj    *trans = (trans_obj *) PG_GETARG_POINTER(0);
	char	   	 *result;

	result = psprintf("%g,%g", trans->count, trans->sum);
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(my_float8_accum);

Datum
my_float8_accum(PG_FUNCTION_ARGS)
{   
    // result object
    trans_obj *new_agg_state = (trans_obj *) palloc(sizeof(trans_obj));

    // transit object
    trans_obj *agg_state = (trans_obj *) PG_GETARG_POINTER(0);
    
    // input value
    float  newval = PG_GETARG_FLOAT8(1);

    // deal with NaN error
    if(isNaN(agg_state->count)) {
        agg_state->count = 0;
    }

    new_agg_state->count = agg_state->count + 1.0;
    new_agg_state->sum = agg_state->sum + newval;

    PG_RETURN_POINTER(new_agg_state);	
}


PG_FUNCTION_INFO_V1(my_float8_avg);

Datum
my_float8_avg(PG_FUNCTION_ARGS)
{
    trans_obj *ts;
    float8  N;

    ts = (trans_obj *ts) PG_GETARG_POINTER(0);
    N = ts->count;

    /* SQL92 defines AVG of no values to be NULL */
    if (N == 0.0)
	PG_RETURN_NULL();
    elog(INFO, "the Count final is %f, sum-final is %f", ts->count, ts->sum);
    PG_RETURN_FLOAT8(ts->sum / ts->count);
}
