#!/usr/bin/env python2
# -*- coding:utf-8 -*-
import os
import json
import logging
import traceback

from Domestic.AmountDataFixed import AmountDataFixed as Domestic_AmountDataFixed
from Domestic.DateDataFixed import DateDataFixed as Domestic_DateDataFixed
from Domestic.CodeDataFixed import CodeDataFixed as Domestic_CodeDataFixed
from Domestic.NumberDataFixed import NumberDataFixed as Domestic_NumberDataFixed
from Domestic.CheckCodeFixed import CheckCodeFixed as Domestic_CheckCodeFixed
from International.AmountDataFixed import AmountDataFixed as International_AmountDataFixed
from DataFixed import ConfidenceLevel

def FixedReceiptData(receiptData):
    try:
        
        serverType = receiptData.get(u'servertype', u'').lower()
        logging.info(u'servertype: {}'.format(serverType))
        if serverType == u'domestic':
            return FixedReceiptDataForDomestic(receiptData)
        
        if serverType == u'international':
            return FixedReceiptDataForInternational(receiptData)
        
        return receiptData
    
    except Exception as err:
        traceback.print_exc()
        logging.info(u'FixedReceiptData err: {}'.format(err))
        return receiptData
    

def FixedReceiptDataForDomestic(receiptData):
    dataFixed = Domestic_AmountDataFixed()
    amount_confidencelevel, result_before_tax, result_tax, result_after_tax = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Domestic_DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Domestic_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Domestic_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Domestic_CheckCodeFixed()
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
                                   {u'cls':4, u'confidencelevel': amount_confidencelevel.value, u'result':result_before_tax}, 
                                   {u'cls':5, u'confidencelevel': checkcode_confidencelevel.value, u'result':checkcode},
                                   {u'cls':8, u'confidencelevel': amount_confidencelevel.value, u'result':result_tax},
                                   {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result':result_after_tax},
                                   {u'confidencelevel': summaryconfidencelevel.value}
                                   ]

    return receiptData

    
def FixedReceiptDataFromJsonForDomestic(receiptDataJson):
    receiptData = json.loads(receiptDataJson)
    receiptData = FixedReceiptData(receiptData)

    return json.dumps(receiptData,  ensure_ascii=False, indent=4, sort_keys=True)


def FixedReceiptDataForInternational(receiptData):
    dataFixed = International_AmountDataFixed()
    confidencelevel, subtotal, tip, tip_rate, total = dataFixed.StartFixedFromJson(receiptData)
    receiptData[u'fixedregions'] = [{u'cls':3, u'result':subtotal}, 
                                   {u'cls':5, u'result':tip}, 
                                   {u'cls':6, u'result':tip_rate}, 
                                   {u'cls':11, u'result':total}, 
                                   {u'confidencelevel': confidencelevel.value}
                                   ]
    return receiptData


if __name__ == '__main__':
    #dataFixed = Domestic_AmountDataFixed()
    dataFixed = Domestic_DateDataFixed()
    #dataFixed = Domestic_CodeDataFixed()
    #dataFixed = Domestic_NumberDataFixed()
    #dataFixed = Domestic_CheckCodeFixed()
    dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/Domestic/result/', u'C:/Users/User/Desktop/receipt/Domestic/validated/')
    #dataFixed = International_AmountDataFixed()
    #dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/International/result/', 'C:/Users/User/Desktop/receipt/International/validated/')
    
    #receiptData = json.load(open(u'../data/11111.json'))
    #receiptData = FixedReceiptData(receiptData)
    #print receiptData
    #json.dump(receiptData,  open(u'../data/11111-2.json', "w"), indent = 4)
    #file.close()
    pass