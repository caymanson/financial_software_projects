#include "YieldCurve.h"
#include "SBB_date.h"
#include "string.h"
#include "bond.h"

double
YieldCurve::getYield(int term) {
	SBB_date settleDtObj, matDtObj;
        int termd=0, termu=0, id=-1, iu=-1;

        for(int i=0;i<length;i++) {
        	settleDtObj.set_from_yyyymmdd(mtbs[i].SettlementDate);
                matDtObj.set_from_yyyymmdd(mtbs[i].MaturityDate);
                int t = term - Term::getNumberOfPeriods(settleDtObj, matDtObj, 1);

                if(t<0)
                	if(-1==iu) {
                        	iu = i;
                                termu = t;
                        }
                        else if(t<termu) {
                                iu = i;
                                termu = t;
                        }
                else {
                        if(-1==id) {
                                id = i;
                                termd = t;
                        }
                        else if(t>termd) {
                                id = i;
                                termd = t;
                        }
                }
        }

        if(id==-1)
                return mtbs[iu].Rate;
        else
                return mtbs[id].Rate;
}

double
YieldCurve::getDV01(int term){
	char sid[INSTRUMENT_ID_LENGTH];
	sprintf(sid,"T%d",term);

	for (int i=0; i<length; i++)
	{
		if (0==strcmp(mtbs[i].SecurityID,sid))
		{
			InstrumentFields* instr = &mtbs[i];
			CouponBearingBondCalculator CBB_cal_ptr(instr);
			return CBB_cal_ptr.getDVO1();
		}
	}
}