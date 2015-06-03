/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2008, 2009 StatPro Italia srl
 Copyright (C) 2009, 2015 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/indexes/iborindex.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>

namespace QuantLib {

    IborIndex::IborIndex(const std::string& familyName,
                         const Period& tenor,
                         Natural settlementDays,
                         const Currency& currency,
                         const Calendar& fixingCalendar,
                         BusinessDayConvention convention,
                         bool endOfMonth,
                         const DayCounter& dayCounter,
                         const Handle<ForwardRateCurve>& fwd)
    : InterestRateIndex(familyName, tenor, settlementDays, currency,
                        fixingCalendar, dayCounter),
      convention_(convention), fwdCurve_(fwd), endOfMonth_(endOfMonth) {
        registerWith(fwdCurve_);
      }

    IborIndex::IborIndex(const std::string& familyName,
                         const Period& tenor,
                         Natural settlementDays,
                         const Currency& currency,
                         const Calendar& fixingCalendar,
                         BusinessDayConvention convention,
                         bool endOfMonth,
                         const DayCounter& dayCounter,
                         const Handle<YieldTermStructure>& fwd)
    : InterestRateIndex(familyName, tenor, settlementDays, currency,
                        fixingCalendar, dayCounter),
      convention_(convention), fwdCurve_(convertIntoFRCHandle(fwd)),
      endOfMonth_(endOfMonth) {
        registerWith(fwdCurve_);
      }

    Rate IborIndex::forecastFixing(const Date& fixingDate) const {
        Date d1 = valueDate(fixingDate);
        Date d2 = maturityDate(d1);
        Time t = dayCounter_.yearFraction(d1, d2);
        QL_REQUIRE(t>0.0,
                   "\n cannot calculate forward rate between " <<
                   d1 << " and " << d2 <<
                   ":\n non positive time (" << t <<
                   ") using " << dayCounter_.name() << " daycounter");
        return forecastFixing(d1, d2, t);
    }

    Date IborIndex::maturityDate(const Date& valueDate) const {
        return fixingCalendar().advance(valueDate,
                                        tenor_,
                                        convention_,
                                        endOfMonth_);
    }

    boost::shared_ptr<IborIndex> IborIndex::clone(
                               const Handle<ForwardRateCurve>& h) const {
        return boost::shared_ptr<IborIndex>(
                                        new IborIndex(familyName(),
                                                      tenor(),
                                                      fixingDays(),
                                                      currency(),
                                                      fixingCalendar(),
                                                      businessDayConvention(),
                                                      endOfMonth(),
                                                      dayCounter(),
                                                      h));
    }


    OvernightIndex::OvernightIndex(const std::string& familyName,
                                   Natural settlementDays,
                                   const Currency& curr,
                                   const Calendar& fixCal,
                                   const DayCounter& dc,
                                   const Handle<ForwardRateCurve>& h)
   : IborIndex(familyName, 1*Days, settlementDays, curr,
               fixCal, Following, false, dc, h) {}

    OvernightIndex::OvernightIndex(const std::string& familyName,
                                   Natural settlementDays,
                                   const Currency& curr,
                                   const Calendar& fixCal,
                                   const DayCounter& dc,
                                   const Handle<YieldTermStructure>& h)
   : IborIndex(familyName, 1*Days, settlementDays, curr,
               fixCal, Following, false, dc, h) {}

    boost::shared_ptr<IborIndex> OvernightIndex::clone(
                               const Handle<ForwardRateCurve>& h) const {
        return boost::shared_ptr<IborIndex>(
                                        new OvernightIndex(familyName(),
                                                           fixingDays(),
                                                           currency(),
                                                           fixingCalendar(),
                                                           dayCounter(),
                                                           h));
    }

}
