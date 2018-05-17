#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import re

from Domestic.AmountDataFixed import AmountDataFixed
from Domestic.DateDataFixed import DateDataFixed
from Domestic.CodeDataFixed import CodeDataFixed
from Domestic.NumberDataFixed import NumberDataFixed

if __name__ == '__main__':
    #dataFixed = AmountDataFixed('C:/Users/User/Desktop/receipt/Domestic/validated/', 'C:/Users/User/Desktop/receipt/Domestic/result/')
    #dataFixed = DateDataFixed('C:/Users/User/Desktop/receipt/Domestic/validated/', 'C:/Users/User/Desktop/receipt/Domestic/result/')
    dataFixed = CodeDataFixed('C:/Users/User/Desktop/receipt/Domestic/validated/', 'C:/Users/User/Desktop/receipt/Domestic/result/')
    dataFixed = NumberDataFixed('C:/Users/User/Desktop/receipt/Domestic/validated/', 'C:/Users/User/Desktop/receipt/Domestic/result/')
    dataFixed.StartFixed()
