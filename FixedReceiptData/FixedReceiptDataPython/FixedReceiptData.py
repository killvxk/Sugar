#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import re

from Domestic.AmountDataFixed import AmountDataFixed
from Domestic.DateDataFixed import DateDataFixed

if __name__ == '__main__':
    year = list(re.findall('[月|-|\.](\d*)日?', '2018年06月03'))
    #dataFixed = AmountDataFixed('C:/Users/User/Desktop/receipt/Domestic/validated/', 'C:/Users/User/Desktop/receipt/Domestic/result/')
    dataFixed = DateDataFixed('C:/Users/User/Desktop/receipt/Domestic/validated/', 'C:/Users/User/Desktop/receipt/Domestic/result/')
    dataFixed.StartFixed()
