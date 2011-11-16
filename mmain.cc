#include <stdio.h>
#include "tradingbook_collection.h"
#include "SBB_util.h"

int main(int argc, char **argv)
{
	SBB_util util;
	START_TIMER(util);
	tradingbook_collection tc(argv[1], argv[2]);
//	tc.show();
	tc.tradingbook_calc();
//	tc.print_results();
	tc.output(argv[1]);
	END_TIMER(util);
	return 0;
}

