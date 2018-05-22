#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import os
import json

from Domestic.AmountDataFixed import AmountDataFixed as Domestic_AmountDataFixed
from Domestic.DateDataFixed import DateDataFixed as Domestic_DateDataFixed
from Domestic.CodeDataFixed import CodeDataFixed as Domestic_CodeDataFixed
from Domestic.NumberDataFixed import NumberDataFixed as Domestic_NumberDataFixed
from Domestic.CheckCodeFixed import CheckCodeFixed as Domestic_CheckCodeFixed
from International.AmountDataFixed import AmountDataFixed as International_AmountDataFixed
from DataFixed import ConfidenceLevel


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
        
    receiptData['fixedregions'] = [{'cls':1, 'confidencelevel': code_confidencelevel.value, 'result':code}, 
                                   {'cls':2, 'confidencelevel': number_confidencelevel.value, 'result':number}, 
                                   {'cls':3, 'confidencelevel': date_confidencelevel.value, 'result':date}, 
                                   {'cls':4, 'confidencelevel': amount_confidencelevel.value, 'result':result_before_tax}, 
                                   {'cls':5, 'confidencelevel': checkcode_confidencelevel.value, 'result':checkcode},
                                   {'cls':8, 'confidencelevel': amount_confidencelevel.value, 'result':result_tax},
                                   {'cls':9, 'confidencelevel': amount_confidencelevel.value, 'result':result_after_tax},
                                   {'confidencelevel': summaryconfidencelevel}
                                   ]

    return receiptData

    
def FixedReceiptDataFromJsonForDomestic(receiptDataJson):
    receiptData = json.loads(receiptDataJson)
    receiptData = FixedReceiptData(receiptData)

    return json.dumps(receiptData,  ensure_ascii=False, indent=4, sort_keys=True)


def FixedReceiptDataForInternational(receiptData):
    dataFixed = International_AmountDataFixed()
    confidencelevel, subtotal, tip, tip_rate, total = dataFixed.StartFixedFromJson(receiptData)
    receiptData['fixedregions'] = [{'cls':3, 'result':subtotal}, 
                                   {'cls':5, 'result':tip}, 
                                   {'cls':6, 'result':tip_rate}, 
                                   {'cls':11, 'result':total}, 
                                   {'confidencelevel': confidencelevel}
                                   ]
    return receiptData


if __name__ == '__main__':
    #dataFixed = Domestic_AmountDataFixed()
    #dataFixed = Domestic_DateDataFixed()
    #dataFixed = Domestic_CodeDataFixed()
    #dataFixed = Domestic_NumberDataFixed()
    #dataFixed = Domestic_CheckCodeFixed()
    #dataFixed.StartFixedFromPath('C:/Users/User/Desktop/receipt/Domestic/result/', 'C:/Users/User/Desktop/receipt/Domestic/validated/')
    #dataFixed = International_AmountDataFixed()
    #dataFixed.StartFixedFromPath('C:/Users/User/Desktop/receipt/International/result/', 'C:/Users/User/Desktop/receipt/International/validated/')
    
    #receiptData = open('C:/Users/User/Desktop/receipt/Domestic/result/10100_1.jpg.json', encoding='utf-8').read()
    #receiptData = FixedReceiptDataFromJson(receiptData)
    #file = open('C:/Users/User/Desktop/output/10100_1.jpg.json', encoding='utf-8', mode='w')
    #file.write(receiptData)
    #file.close()
