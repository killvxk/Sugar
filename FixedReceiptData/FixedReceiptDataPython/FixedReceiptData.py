#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import os
import json

from Domestic.AmountDataFixed import AmountDataFixed
from Domestic.DateDataFixed import DateDataFixed
from Domestic.CodeDataFixed import CodeDataFixed
from Domestic.NumberDataFixed import NumberDataFixed
from Domestic.CheckCodeFixed import CheckCodeFixed

if __name__ == '__main__':
    dataFixed = AmountDataFixed()
    #dataFixed = DateDataFixed()
    #dataFixed = CodeDataFixed()
    #dataFixed = NumberDataFixed()
    #dataFixed = CheckCodeFixed()
    #dataFixed.StartFixedFromPath('C:/Users/User/Desktop/receipt/Domestic/result/', 'C:/Users/User/Desktop/receipt/Domestic/validated/')
    result_before_tax, result_tax, result_after_tax = dataFixed.StartFixedFromJson(json.load(open('C:/Users/User/Desktop/receipt/Domestic/result/10100_1.jpg.json', encoding='utf-8')));
