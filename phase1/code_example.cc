
		for(int i=0;i<_bond_collection_size;i++){

			//
			// allocate a tmp ptr so I don't have to index into the array repeatedly
			//
			SBB_instrument_fields* bond_record_ptr = &records[i];

			//
			// calc number of periods between settle and maturity for each bond 
			// (assuming no partial periods)
			// I could also pass dates to calculator class and put inside but I like keeping date object
			// outside to minimize complexity and overhead (concept is to pass lower level "intermediate" 
			// data to analytics
			//
			settle_dt_obj.set_from_yyyymmdd(bond_record_ptr->SettlementDate() );
			mat_dt_obj.set_from_yyyymmdd(bond_record_ptr->MaturityDate() );
			num_periods = SBB_term::get_number_of_periods_semiannual(settle_dt_obj, mat_dt_obj);

			//	
			// branch off coupon value - if 0 then we know it's a zero coupon bond
			// could add a string type in the file if we need to support more bond types but not necessary
			// since we only support bullet coupon and zero coupon
			//
			// Given I have a top level interface class which all inherit from, allocate 1 of those,
			// new appropriate derived class and assign it to the absract class ptr...
			//
			SBB_bond_calculator_interface* bond_calc_ptr;

			if(0 == bond_record_ptr->Coupon() ) {

				bond_calc_ptr = new SBB_zero_coupon_calculator(
					num_periods,
					bond_record_ptr->Frequency() );

			}
			else 	// must be coupon bearing - only two types in the file
			{
				bond_calc_ptr = new SBB_coupon_bearing_calculator(
					num_periods,
					bond_record_ptr->Frequency(),
					bond_record_ptr->Coupon() );
			}

			if(SBB_is_debug_on()){
				printf("\nnum records in file is: %d in iter: %d\n", _bond_collection_size,i);
				bond_calc_ptr->show();
			}

			_bond_collection[i].set_state( bond_calc_ptr, bond_record_ptr );

			//
			// allocate a tmp ptr so I don't have to index into the array repeatedly
			//
			SBB_instrument_fields* bond_record_ptr = &records[i];

			//
			// calc number of periods between settle and maturity for each bond 
			// (assuming no partial periods)
			// I could also pass dates to calculator class and put inside but I like keeping date object
			// outside to minimize complexity and overhead (concept is to pass lower level "intermediate" 
			// data to analytics
			//
			settle_dt_obj.set_from_yyyymmdd(bond_record_ptr->SettlementDate() );
			mat_dt_obj.set_from_yyyymmdd(bond_record_ptr->MaturityDate() );
			num_periods = SBB_term::get_number_of_periods_semiannual(settle_dt_obj, mat_dt_obj);

			//	
			// branch off coupon value - if 0 then we know it's a zero coupon bond
			// could add a string type in the file if we need to support more bond types but not necessary
			// since we only support bullet coupon and zero coupon
			//
			// Given I have a top level interface class which all inherit from, allocate 1 of those,
			// new appropriate derived class and assign it to the absract class ptr...
			//
			SBB_bond_calculator_interface* bond_calc_ptr;

			if(0 == bond_record_ptr->Coupon() ) {

				bond_calc_ptr = new SBB_zero_coupon_calculator(
					num_periods,
					bond_record_ptr->Frequency() );

			}
			else 	// must be coupon bearing - only two types in the file
			{
				bond_calc_ptr = new SBB_coupon_bearing_calculator(
					num_periods,
					bond_record_ptr->Frequency(),
					bond_record_ptr->Coupon() );
			}

			if(SBB_is_debug_on()){
				printf("\nnum records in file is: %d in iter: %d\n", _bond_collection_size,i);
				bond_calc_ptr->show();
			}

			_bond_collection[i].set_state( bond_calc_ptr, bond_record_ptr );
		}

