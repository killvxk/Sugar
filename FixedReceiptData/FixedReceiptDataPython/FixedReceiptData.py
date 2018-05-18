#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import os
import json

from Domestic.AmountDataFixed import AmountDataFixed
from Domestic.DateDataFixed import DateDataFixed
from Domestic.CodeDataFixed import CodeDataFixed
from Domestic.NumberDataFixed import NumberDataFixed
from Domestic.CheckCodeFixed import CheckCodeFixed

DataFixedList = [AmountDataFixed(), DateDataFixed(), CodeDataFixed(), NumberDataFixed(), CheckCodeFixed()]

def FixedReceiptDataFromJson(receiptDataJson):
    receiptData = json.loads(receiptDataJson)

    dataFixed = AmountDataFixed()
    amount_confidencelevel, result_before_tax, result_tax, result_after_tax = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = CheckCodeFixed()
    checkcode_confidencelevel, checkcode = dataFixed.StartFixedFromJson(receiptData)

    receiptData['fixedregions'] = [{'cls':1, 'confidencelevel': code_confidencelevel.value, 'result':code}, 
                                   {'cls':2, 'confidencelevel': number_confidencelevel.value, 'result':number}, 
                                   {'cls':3, 'confidencelevel': date_confidencelevel.value, 'result':date}, 
                                   {'cls':4, 'confidencelevel': amount_confidencelevel.value, 'result':result_before_tax}, 
                                   {'cls':5, 'confidencelevel': checkcode_confidencelevel.value, 'result':checkcode},
                                   {'cls':8, 'confidencelevel': amount_confidencelevel.value, 'result':result_tax},
                                   {'cls':9, 'confidencelevel': amount_confidencelevel.value, 'result':result_after_tax}
                                   ]

    return json.dumps(receiptData)

if __name__ == '__main__':
    receiptData = open('C:/Users/User/Desktop/receipt/Domestic/result/10100_1.jpg.json', encoding='utf-8').read()
    receiptData = FixedReceiptDataFromJson(receiptData)
    file = open('C:/Users/User/Desktop/output/10100_1.jpg.json', encoding='utf-8', mode='w')
    file.write(receiptData)
    file.close()
