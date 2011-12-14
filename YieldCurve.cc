#include "YieldCurve.h"
#include "SBB_date.h"
#include "string.h"
#include "bond.h"

void
YieldCurve::getYieldID(int term, int& idt, double& yield)
{
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

        if(id==-1){
                yield = mtbs[iu].Rate+shift_arr[iu];
				idt = iu;
		}
        else{
                yield = mtbs[id].Rate+shift_arr[id];
				idt = id;
		}
}

double
YieldCurve::getYield(int term) {
	int id;
	double yield;
	getYieldID(term,id,yield);
	return yield;
}

int 
YieldCurve::getID(int term) {
	int id;
	double yield;
	getYieldID(term,id,yield);
	return id;
}

double
YieldCurve::getYieldUseID(int id)
{
	return mtbs[id].Rate+shift_arr[id];
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

void
YieldCurve::set_shift_arr(double s1, double s2, double s3, double s4)
{
	for(int i=0;i<length;i++) {
		switch (mtbs[i].SecurityID[1]) {
			case '2': shift_arr[i]=s1;break;
			case '5': shift_arr[i]=s2;break;
			case '1': shift_arr[i]=s3;break;
			case '3': shift_arr[i]=s4;break;
			default: break;
		}
	}
}
