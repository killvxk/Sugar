#!/usr/bin/env python2
# -*- coding:utf-8 -*-
import os
import json
import logging
import traceback
import sys
import re

from ConfidenceLevel import ConfidenceLevel
from Domestic.VAT.AmountDataFixed import AmountDataFixed as VAT_AmountDataFixed
from Domestic.VAT.DateDataFixed import DateDataFixed as VAT_DateDataFixed
from Domestic.VAT.CodeDataFixed import CodeDataFixed as VAT_CodeDataFixed
from Domestic.VAT.NumberDataFixed import NumberDataFixed as VAT_NumberDataFixed
from Domestic.VAT.CheckCodeFixed import CheckCodeFixed as VAT_CheckCodeFixed
from International.AmountDataFixed import AmountDataFixed as International_AmountDataFixed
from Domestic.Tax.AmountDataFixed import AmountDataFixed as Tax_AmountDataFixed
from Domestic.Tax.CodeDataFixed import CodeDataFixed as Tax_CodeDataFixed
from Domestic.Tax.DateDataFixed import DateDataFixed as Tax_DateDataFixed
from Domestic.Tax.MileageDataFixed import MileageDataFixed as Tax_MileageDataFixed
from Domestic.Tax.NumberDataFixed import NumberDataFixed as Tax_NumberDataFixed
from Domestic.Tax.TimeDataFixed import TimeDataFixed as Tax_TimeDataFixed
from Domestic.Train.NumberDataFixed import NumberDataFixed as Train_NumberDataFixed
from Domestic.Train.AmountDataFixed import AmountDataFixed as Train_AmountDataFixed
from Domestic.Train.DateDataFixed import DateDataFixed as Train_DateDataFixed
from Domestic.Train.NumberDataFixed import NumberDataFixed as Train_NumberDataFixed
from Domestic.Train.SeatDataFixed import SeatDataFixed as Train_SeatDataFixed
from Domestic.Train.StationDataFixed import StationDataFixed as Train_StationDataFixed
from Domestic.Train.TrainNumberDataFixed import TrainNumberDataFixed as Train_TrainNumberDataFixed

def FixedReceiptData(receiptData):
    try:
        
        serverType = receiptData.get(u'servertype', u'').lower()
        logging.info(u'servertype: {}'.format(serverType))

        if serverType == u'international':
            return FixedReceiptDataForInternational(receiptData)

        else:
            type = receiptData.get('type', [{'label':'0'}])[0]['label']

            if type == '10500':
                return FixedReceiptDataForTax(receiptData)
            elif type == '10503':
                return FixedReceiptDataForTrain(receiptData)
            elif serverType == u'domestic' or type == '10100' or type == '10101' or type == '10102':
                return FixedReceiptDataForVAT(receiptData)
        
        return receiptData
    
    except Exception as err:
        traceback.print_exc()
        logging.info(u'FixedReceiptData err: {}'.format(err))
        return receiptData
    

def FixedReceiptDataForVAT(receiptData):
    dataFixed = VAT_AmountDataFixed()
    amount_confidencelevel, before_tax, tax, after_tax = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VAT_DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VAT_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VAT_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VAT_CheckCodeFixed()
    checkcode_confidencelevel, checkcode = dataFixed.StartFixedFromJson(receiptData)

    summaryconfidencelevel = ConfidenceLevel.Bad
    if checkcode_confidencelevel == ConfidenceLevel.Bad or date_confidencelevel == ConfidenceLevel.Bad:
        summaryconfidencelevel = ConfidenceLevel.Bad
    else:
        confidencelevellist = [amount_confidencelevel, code_confidencelevel, number_confidencelevel]
        if ConfidenceLevel.Bad in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Bad
        elif ConfidenceLevel.Fixed in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Fixed
        else:
            summaryconfidencelevel = ConfidenceLevel.Confident
        
    receiptData[u'fixedregions'] = [{u'cls':1, u'confidencelevel': code_confidencelevel.value, u'result':code}, 
                                   {u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                   {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date}, 
                                   {u'cls':4, u'confidencelevel': amount_confidencelevel.value, u'result':before_tax}, 
                                   {u'cls':5, u'confidencelevel': checkcode_confidencelevel.value, u'result':checkcode},
                                   {u'cls':8, u'confidencelevel': amount_confidencelevel.value, u'result':tax},
                                   {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result':after_tax}
                                   ]
    receiptData[u'confidencelevel'] = summaryconfidencelevel.value

    return receiptData


def FixedReceiptDataForInternational(receiptData):
    dataFixed = International_AmountDataFixed()
    confidencelevel, subtotal, tip, tip_rate, total = dataFixed.StartFixedFromJson(receiptData)
    receiptData[u'fixedregions'] = [{u'cls':3, u'result':subtotal}, 
                                   {u'cls':5, u'result':tip}, 
                                   {u'cls':6, u'result':total}, 
                                   {u'cls':11, u'result':tip_rate}
                                   ]
    receiptData[u'confidencelevel'] = confidencelevel.value
    return receiptData


def FixedReceiptDataForTax(receiptData):
    dataFixed = Tax_AmountDataFixed()
    amount_confidencelevel, amount = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_MileageDataFixed()
    mileage_confidencelevel, mileage = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_TimeDataFixed()
    time_confidencelevel, starttime, endtime = dataFixed.StartFixedFromJson(receiptData)

    receiptData[u'fixedregions'] = [{u'cls':1, u'confidencelevel': code_confidencelevel.value, u'result':code}, 
                                   {u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                   {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date}, 
                                   {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result':amount}, 
                                   {u'cls':10, u'confidencelevel': mileage_confidencelevel.value, u'result':mileage},
                                   {u'cls':31, u'confidencelevel': time_confidencelevel.value, u'result':starttime},
                                   {u'cls':32, u'confidencelevel': time_confidencelevel.value, u'result':endtime},
                                   ]
    receiptData[u'confidencelevel'] = ConfidenceLevel.Bad.value
    return receiptData


def FixedReceiptDataForTrain(receiptData):
    dataFixed = Train_AmountDataFixed()
    amount_confidencelevel, amount = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_SeatDataFixed()
    seat_confidencelevel, seat = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_StationDataFixed()
    station_confidencelevel, startstation, endstation = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_TrainNumberDataFixed()
    trainnumber_confidencelevel, trainnumber = dataFixed.StartFixedFromJson(receiptData)

    receiptData[u'fixedregions'] = [{u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                   {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date}, 
                                   {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result':amount}, 
                                   {u'cls':15, u'confidencelevel': station_confidencelevel.value, u'result':startstation},
                                   {u'cls':16, u'confidencelevel': station_confidencelevel.value, u'result':endstation},
                                   {u'cls':17, u'confidencelevel': trainnumber_confidencelevel.value, u'result':trainnumber},
                                   {u'cls':18, u'confidencelevel': seat_confidencelevel.value, u'result':seat},
                                   ]
    receiptData[u'confidencelevel'] = ConfidenceLevel.Bad.value

    return receiptData


if __name__ == '__main__':
    logging.basicConfig(stream=sys.stdout,)
    logging.getLogger().setLevel(logging.INFO)
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    formatter = logging.Formatter(
        "%(asctime)s;%(levelname)s;%(message)s", "%Y-%m-%d %H:%M:%S")
    ch.setFormatter(formatter)
    logging.getLogger().addHandler(ch)

    houselist = re.findall(u'[日| ](\\d*):', '2017年02月23日 20:57')
    #dataFixed = VAT_AmountDataFixed()
    #dataFixed = VAT_DateDataFixed()
    #dataFixed = VAT_CodeDataFixed()
    #dataFixed = VAT_NumberDataFixed()
    #dataFixed = VAT_CheckCodeFixed()
    #dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/Domestic/result/', u'C:/Users/User/Desktop/receipt/Domestic/validated/')
    #dataFixed = International_AmountDataFixed()
    #dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/International/result/', 'C:/Users/User/Desktop/receipt/International/validated/')
    #dataFixed = Tax_AmountDataFixed()
    #dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/Tax/result/', 'C:/Users/User/Desktop/receipt/Tax/validated/')
    dataFixed = Train_DateDataFixed()
    dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/Train/result/', 'C:/Users/User/Desktop/receipt/Train/validated/')
    
    